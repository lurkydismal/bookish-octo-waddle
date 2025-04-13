#include "image_t.h"

#include <simde/x86/sse2.h>

#include "log.h"
#include "stdfunc.h"

// Constants
#define RGBA_PIXEL_SIZE ( 4 )
#define BGR_PIXEL_SIZE ( 3 )

#define BMP_DATA_START_VALUE_OFFsET ( 10 )
#define BMP_WIDTH_OFFSET ( 18 )
#define BMP_HEIGHT_OFFSET ( 22 )

#define RGBA_ROW_SIZE( _width ) ( ( _width ) * RGBA_PIXEL_SIZE )

// Computes total RGBA data size
#define RGBA_DATA_SIZE( _image ) \
    ( RGBA_ROW_SIZE( ( _image ).width ) * ( _image ).height )

/**
 * @brief Computes the size of a single row (scanline) in a BMP image, including
 * padding.
 *
 * BMP images store pixel data in rows, where each row size must be **aligned to
 * a multiple of 4 bytes**. This macro calculates the **padded row size** to
 * meet that alignment requirement.
 *
 * Calculation:
 * - `( _width * BGR_PIXEL_SIZE )` → Computes the actual number of bytes
 * required for pixel data.
 * - `+ BGR_PIXEL_SIZE` → Ensures that any remainder when divided by 4 is
 * accounted for.
 * - `& ~BGR_PIXEL_SIZE` → Applies bitwise alignment to the nearest multiple
 * of 4.
 *
 * Padding is BMP Format Requirement
 * - BMP rows must be aligned to 4-byte boundaries for proper memory access.
 * - If a row's raw pixel data is not a multiple of 4, extra padding bytes
 * (0x00) are added.
 *
 * @param _width The width of the image in pixels.
 * @return The total row size in bytes, including padding.
 */
#define BGR_ROW_SIZE( _width ) \
    ( ( ( _width * BGR_PIXEL_SIZE ) + BGR_PIXEL_SIZE ) & ~BGR_PIXEL_SIZE )

// Computes total BMP data size (including padding)
#define BGR_DATA_SIZE( _image )                                        \
    ( size_t )( BGR_ROW_SIZE( ( _image ).width ) * ( _image ).height * \
                sizeof( uint8_t ) )

/**
 * @brief Loads a 24-bit BMP image from a file.
 *
 * @param _image Pointer to the BMP image structure.
 * @param _fileName Name of the BMP file.
 * @return success if the image is loaded successfully, otherwise failure.
 */
static FORCE_INLINE bool BMP$load( image_t* restrict _image,
                                   asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_image ) ) {
        log$transaction$query( ( logLevel_t )error,
                               "BMP load: Invalid storage\n" );

        goto EXIT;
    }

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error,
                               "BMP load: Invalid asset\n" );

        goto EXIT;
    }

    {
        if ( UNLIKELY( _asset->size < BMP_DATA_START_VALUE_OFFsET ) ) {
            l_returnValue = false;

            goto EXIT;
        }

        const uint32_t l_headerSize =
            _asset->data[ BMP_DATA_START_VALUE_OFFsET ];

        if ( UNLIKELY( _asset->size < l_headerSize ) ) {
            l_returnValue = false;

            goto EXIT;
        }

        uint8_t* l_header = ( uint8_t* )malloc( l_headerSize );

        __builtin_memcpy( l_header, _asset->data, l_headerSize );

#if 0
        _image->width = ( size_t )( *(
            ( uint32_t* )( &( l_header )[ BMP_WIDTH_OFFSET ] ) ) );
        _image->height = ( size_t )( *(
            ( uint32_t* )( &( l_header )[ BMP_HEIGHT_OFFSET ] ) ) );
#endif

        // TODO: Decide on this
        __builtin_memcpy( &( _image->width ), ( l_header + BMP_WIDTH_OFFSET ),
                          sizeof( uint32_t ) );
        __builtin_memcpy( &( _image->height ), ( l_header + BMP_HEIGHT_OFFSET ),
                          sizeof( uint32_t ) );

        {
            const size_t l_dataSize = RGBA_DATA_SIZE( *_image );

            if ( UNLIKELY( _asset->size <
                           ( l_headerSize + BGR_DATA_SIZE( *_image ) ) ) ) {
                l_returnValue = false;

                goto EXIT_HEADER;
            }

            _image->data = ( uint8_t* )malloc( l_dataSize );

            const size_t l_rgbaRowSize = RGBA_ROW_SIZE( _image->width );
            const size_t l_bgrRowSize = BGR_ROW_SIZE( _image->width );

            FOR_RANGE( size_t, 0, _image->height ) {
                // Row index from the bottom of the image
                const size_t l_reverseY = ( _image->height - _index - 1 );
                const uint8_t* l_bgrRow =
                    ( _asset->data +
                      ( l_headerSize + ( l_reverseY * l_bgrRowSize ) ) );
                const size_t l_rowOffset = ( _index * l_rgbaRowSize );
                uint8_t* l_rgbaRow = ( _image->data + l_rowOffset );

#if 1
                // TODO: Add 256 and 512 AVX support
#ifdef __SSE2__

                {
#define SIMD_PIXELS ( 4 )

                    // Number of pixels not aligned for SIMD
                    const size_t l_noSIMDPixelsCount =
                        ( _image->width % SIMD_PIXELS );
                    size_t l_index = 0;

                    FOR_RANGE_BY( size_t, 0,
                                  ( _image->width - l_noSIMDPixelsCount ),
                                  SIMD_PIXELS ) {
                        // TODO: Improve l_tempBGR
                        uint8_t l_tempBGR[ SIMD_PIXELS * BGR_PIXEL_SIZE ];
                        __builtin_memcpy(
                            l_tempBGR, ( l_bgrRow + l_index * BGR_PIXEL_SIZE ),
                            ( SIMD_PIXELS * BGR_PIXEL_SIZE ) );

                        uint8_t r[ SIMD_PIXELS ];
                        uint8_t g[ SIMD_PIXELS ];
                        uint8_t b[ SIMD_PIXELS ];

                        FOR_RANGE( size_t, 0, SIMD_PIXELS ) {
                            b[ _index ] = l_tempBGR[ _index * 3 + 0 ];
                            g[ _index ] = l_tempBGR[ _index * 3 + 1 ];
                            r[ _index ] = l_tempBGR[ _index * 3 + 2 ];
                        }

                        // clang-format off
    simde__m128i l_rgbaVector = simde_mm_setr_epi8(
        r[ 0 ], g[ 0 ], b[ 0 ], ( char )0xFF,
        r[ 1 ], g[ 1 ], b[ 1 ], ( char )0xFF,
        r[ 2 ], g[ 2 ], b[ 2 ], ( char )0xFF,
        r[ 3 ], g[ 3 ], b[ 3 ], ( char )0xFF );
                        // clang-format on

                        simde_mm_storeu_si128(
                            ( simde__m128i* )( l_rgbaRow +
                                               l_index * RGBA_PIXEL_SIZE ),
                            l_rgbaVector );

                        l_index = _index;
                    }

                    FOR_RANGE( size_t, l_index, _image->width ) {
                        const size_t rgbaIndex = ( _index * RGBA_PIXEL_SIZE );
                        const size_t bgrIndex = ( _index * BGR_PIXEL_SIZE );

                        l_rgbaRow[ rgbaIndex + 0 ] = l_bgrRow[ bgrIndex + 0 ];
                        l_rgbaRow[ rgbaIndex + 1 ] = l_bgrRow[ bgrIndex + 1 ];
                        l_rgbaRow[ rgbaIndex + 2 ] = l_bgrRow[ bgrIndex + 2 ];
                        l_rgbaRow[ rgbaIndex + 3 ] = 0xFF;
                    }
                }

                // No vectorization
#else
                // Loop over each pixel in the row (x-axis) using FOR_RANGE
                FOR_RANGE( size_t, 0, _image->width ) {
                    const size_t l_rgbaRowIndex = ( _index * RGBA_PIXEL_SIZE );
                    const size_t l_bgrRowIndex = ( _index * BGR_PIXEL_SIZE );

                    uint8_t* l_rgbaPixel = ( l_rgbaRow + l_rgbaRowIndex );
                    const uint8_t* l_bgrPixel = ( l_bgrRow + l_bgrRowIndex );

                    l_rgbaPixel[ 0 ] = l_bgrPixel[ 0 ]; // B
                    l_rgbaPixel[ 1 ] = l_bgrPixel[ 1 ]; // G
                    l_rgbaPixel[ 2 ] = l_bgrPixel[ 2 ]; // R
                    l_rgbaPixel[ 3 ] = 0xFF;            // A
                }
#endif
#endif
            }

            _image->size = l_dataSize;
        }

        l_returnValue = true;

    EXIT_HEADER:
        free( l_header );
    }

EXIT:
    return ( l_returnValue );
}

image_t image_t$create( void ) {
    image_t l_returnValue = DEFAULT_IMAGE;

    return ( l_returnValue );
}

bool image_t$destroy( image_t* restrict _image ) {
    bool l_returnValue = false;

    {
        _image->width = 0;
        _image->height = 0;
        _image->data = NULL;
        _image->size = 0;

        l_returnValue = true;
    }

    return ( l_returnValue );
}

bool image_t$load$fromAsset( image_t* restrict _image,
                             asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_image ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
        if ( UNLIKELY( !BMP$load( _image, _asset ) ) ) {
            goto EXIT;
        }

        log$transaction$query$format( ( logLevel_t )debug,
                                      "Image width: %d, height: %d\n",
                                      _image->width, _image->height );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool image_t$load$fromAsset$compressed( image_t* restrict _image,
                                        asset_t* restrict _asset ) {
    bool l_returnValue = false;

    {
        if ( UNLIKELY( !image_t$load$fromAsset( _image, _asset ) ) ) {
            l_returnValue = false;

            goto EXIT;
        }

        if ( UNLIKELY( image_t$compress( _image ) ) ) {
            l_returnValue = false;

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Improve
bool image_t$load$fromPath( image_t* restrict _image,
                            const char* restrict _path ) {
    bool l_returnValue = false;

    {
        asset_t l_imageAsset = asset_t$create();

        {
            l_returnValue = asset_t$load( &l_imageAsset, _path );

            if ( UNLIKELY( !l_returnValue ) ) {
                asset_t$destroy( &l_imageAsset );

                goto EXIT;
            }

            l_returnValue = image_t$load$fromAsset( _image, &l_imageAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                asset_t$destroy( &l_imageAsset );

                goto EXIT;
            }

            l_returnValue = asset_t$unload( &l_imageAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                asset_t$destroy( &l_imageAsset );

                goto EXIT;
            }
        }

        l_returnValue = asset_t$destroy( &l_imageAsset );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool image_t$load$fromPath$compressed( image_t* restrict _image,
                                       const char* restrict _path ) {
    bool l_returnValue = false;

    {
        if ( UNLIKELY( !image_t$load$fromPath( _image, _path ) ) ) {
            l_returnValue = false;

            goto EXIT;
        }

        if ( UNLIKELY( image_t$compress( _image ) ) ) {
            l_returnValue = false;

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool image_t$unload( image_t* restrict _image ) {
    bool l_returnValue = false;

    {
        free( _image->data );

        _image->size = 0;

        l_returnValue = true;
    }

    return ( l_returnValue );
}

bool image_t$compress( image_t* restrict _image ) {
    bool l_returnValue = false;

    {
        asset_t l_compressedAsset = asset_t$create();

        l_compressedAsset.data = _image->data;
        l_compressedAsset.size = _image->size;

        // TODO: Make only single destroy
        if ( UNLIKELY( !asset_t$compress( &l_compressedAsset ) ) ) {
            asset_t$destroy( &l_compressedAsset );

            l_returnValue = false;

            goto EXIT;
        }

        _image->data = l_compressedAsset.data;
        _image->size = l_compressedAsset.size;

        if ( UNLIKELY( !asset_t$destroy( &l_compressedAsset ) ) ) {
            l_returnValue = false;

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool image_t$uncompress( image_t* restrict _image ) {
    bool l_returnValue = false;

    {
        asset_t l_uncompressedAsset = asset_t$create();

        l_uncompressedAsset.data = _image->data;
        l_uncompressedAsset.size = _image->size;

        // TODO: Make only single destroy
        if ( UNLIKELY( !asset_t$uncompress( &l_uncompressedAsset ) ) ) {
            asset_t$destroy( &l_uncompressedAsset );

            l_returnValue = false;

            goto EXIT;
        }

        _image->data = l_uncompressedAsset.data;
        _image->size = l_uncompressedAsset.size;

        if ( UNLIKELY( !asset_t$destroy( &l_uncompressedAsset ) ) ) {
            l_returnValue = false;

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
