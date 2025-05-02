#if 0
#define STBTT_STATIC
#endif
#define STB_RECT_PACK_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "font_t.h"

#if defined( WRITE_FONT_TO_IMAGE )

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb/stb_image_write.h>

#endif

#include "log.h"

static FORCE_INLINE uint8_t* font_t$bake$range( font_t* restrict _font,
                                                const size_t _start,
                                                const size_t _end ) {
    uint8_t* l_returnValue = NULL;

    if ( UNLIKELY( !_font ) ) {
        goto EXIT;
    }

    {
        const size_t l_fontAtlasWidth = FONT_ATLAS_WIDTH( *_font );
        const size_t l_fontAtlasHeight = FONT_ATLAS_HEIGHT( *_font );
        const size_t l_bitmapSize =
            ( ( l_fontAtlasWidth * l_fontAtlasHeight ) * sizeof( uint8_t ) );

        log$transaction$query$format( ( logLevel_t )debug,
                                      "Font bitmap size: %zu\n", l_bitmapSize );

        uint8_t* l_bitmap = ( uint8_t* )malloc( l_bitmapSize );

        __builtin_memset( l_bitmap, 0, l_bitmapSize );

        stbtt_pack_context l_context;

        {
            bool l_result = stbtt_PackBegin(
                &l_context, l_bitmap, l_fontAtlasWidth, l_fontAtlasHeight,
                0, // Stribe in bytes
                1, // Character padding
                NULL );

            if ( UNLIKELY( !l_result ) ) {
                free( l_bitmap );

                goto EXIT_PACKING;
            }

            l_result = stbtt_PackFontRange(
                &l_context, _font->info.data, 0, _font->height, _start,
                CHARACTERS_COUNT( _start, _end ), _font->glyphs );

            if ( UNLIKELY( !l_result ) ) {
                free( l_bitmap );

                log$transaction$query( ( logLevel_t )error, "Packing font\n" );

                goto EXIT_PACKING;
            }
        }

        l_returnValue = l_bitmap;

    EXIT_PACKING:
        stbtt_PackEnd( &l_context );
    }

EXIT:
    return ( l_returnValue );
}

font_t font_t$create( void ) {
    font_t l_returnValue = DEFAULT_FONT;

    return ( l_returnValue );
}

bool font_t$destroy( font_t* restrict _font ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_font ) ) {
        goto EXIT;
    }

    {
        _font->texture = 0;
        _font->height = 0;
        _font->ascent = 0;
        _font->atlasWidth = 0;
        _font->atlasHeight = 0;
        _font->descent = 0;
        _font->lineGap = 0;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool font_t$load$fromAsset( font_t* restrict _font, asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_font ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
        int32_t l_fontCount = stbtt_GetNumberOfFonts( _asset->data );

        if ( UNLIKELY( l_fontCount == -1 ) ) {
            l_returnValue = false;

            goto EXIT;
        }

        l_returnValue = stbtt_InitFont( &( _font->info ), _asset->data,
                                        0 // Font offset
        );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        if ( UNLIKELY( !( _font->height ) ) ) {
            l_returnValue = false;

            goto EXIT;
        }

        const float l_scale =
            stbtt_ScaleForPixelHeight( &( _font->info ), _font->height );

        if ( UNLIKELY( !l_scale ) ) {
            l_returnValue = false;

            goto EXIT;
        }

        stbtt_GetFontVMetrics( &( _font->info ), ( int* )( &( _font->ascent ) ),
                               ( int* )( &( _font->descent ) ),
                               ( int* )( &( _font->lineGap ) ) );

        _font->ascent = ( _font->ascent * l_scale );
        _font->descent = ( _font->descent * l_scale );

        {
            uint8_t* l_bitmapTrimmed = NULL;

            // Bitmap
            {
                uint8_t* l_bitmap =
                    font_t$bake$range( _font, ASCII_START, ASCII_END );

                if ( UNLIKELY( !l_bitmap ) ) {
                    l_returnValue = false;

                    goto EXIT;
                }

                // Trimmed bitmap
                {
                    size_t l_maxX = 0;
                    size_t l_maxY = 0;

                    FOR_RANGE( size_t, 0,
                               CHARACTERS_COUNT( ASCII_START, ASCII_END ) ) {
                        l_maxX = max( _font->glyphs[ _index ].x1, l_maxX );
                        l_maxY = max( _font->glyphs[ _index ].y1, l_maxY );
                    }

                    l_bitmapTrimmed = ( uint8_t* )malloc( l_maxX * l_maxY );

                    FOR_RANGE( size_t, 0, l_maxY ) {
                        __builtin_memcpy(
                            ( l_bitmapTrimmed + _index * l_maxX ),
                            ( l_bitmap + _index * FONT_ATLAS_WIDTH( *_font ) ),
                            l_maxX );
                    }

                    _font->atlasWidth = l_maxX;
                    _font->atlasHeight = l_maxY;

                    log$transaction$query$format(
                        ( logLevel_t )debug, "Trimmed font bitmap size: %zu\n",
                        ( ( _font->atlasWidth * _font->atlasHeight ) *
                          sizeof( uint8_t ) ) );
                }

                free( l_bitmap );
            }

#if defined( WRITE_FONT_TO_IMAGE )

            const char* l_exportFileName = "fontAtlas.png";

            stbi_write_png( l_exportFileName, _font->atlasWidth,
                            _font->atlasHeight, 1, l_bitmapTrimmed,
                            _font->atlasWidth );

            log$transaction$query$format(
                ( logLevel_t )info,
                "'%s' file was created with trimmed fomt bitmap\n",
                l_exportFileName );

#endif

            // Texture
            {
                glGenTextures( 1, &( _font->texture ) );

                if ( UNLIKELY( !( _font->texture ) ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Generating font texture\n" );

                    l_returnValue = false;

                    goto EXIT_BITMAP_TRIMMED;
                }

                glBindTexture( GL_TEXTURE_2D, _font->texture );

                if ( UNLIKELY( glGetError() != GL_NO_ERROR ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Binding font texture\n" );

                    l_returnValue = false;

                    goto EXIT_BITMAP_TRIMMED;
                }

                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                 GL_LINEAR );

                if ( UNLIKELY( glGetError() != GL_NO_ERROR ) ) {
                    log$transaction$query(
                        ( logLevel_t )error,
                        "Setting font texture parameters\n" );

                    l_returnValue = false;

                    goto EXIT_BITMAP_TRIMMED;
                }

                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                 GL_LINEAR );

                if ( UNLIKELY( glGetError() != GL_NO_ERROR ) ) {
                    log$transaction$query(
                        ( logLevel_t )error,
                        "Setting font texture parameters\n" );

                    l_returnValue = false;

                    goto EXIT_BITMAP_TRIMMED;
                }

                glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

                if ( UNLIKELY( glGetError() != GL_NO_ERROR ) ) {
                    log$transaction$query(
                        ( logLevel_t )error,
                        "Setting font texture pixel store alignment\n" );

                    l_returnValue = false;

                    goto EXIT_BITMAP_TRIMMED;
                }

                glTexImage2D( GL_TEXTURE_2D,
                              0, // LOD
                              GL_ALPHA, _font->atlasWidth, _font->atlasHeight,
                              0, // Border width ( up to OGL 3.0 )
                              GL_ALPHA, GL_UNSIGNED_BYTE, l_bitmapTrimmed );

                if ( UNLIKELY( glGetError() != GL_NO_ERROR ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Uploading font texture data\n" );

                    l_returnValue = false;

                    goto EXIT_BITMAP_TRIMMED;
                }
            }

            l_returnValue = true;

        EXIT_BITMAP_TRIMMED:
            free( l_bitmapTrimmed );
        }
    }

EXIT:
    return ( l_returnValue );
}

bool font_t$load$fromPath( font_t* restrict _font,
                           const char* restrict _path ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_font ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_path ) ) {
        goto EXIT;
    }

    {
        asset_t l_fontAsset = asset_t$create();

        {
            l_returnValue = asset_t$load( &l_fontAsset, _path );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_FONT_ASSET;
            }

            l_returnValue = font_t$load$fromAsset( _font, &l_fontAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_FONT_ASSET;
            }

            l_returnValue = asset_t$unload( &l_fontAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_FONT_ASSET;
            }
        }

        l_returnValue = true;

    EXIT_FONT_ASSET:
        if ( UNLIKELY( !asset_t$destroy( &l_fontAsset ) ) ) {
            l_returnValue = false;

            goto EXIT;
        }
    }

EXIT:
    return ( l_returnValue );
}

bool font_t$unload( font_t* restrict _font ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_font ) ) {
        goto EXIT;
    }

    {
        glDeleteTextures( 1, &( _font->texture ) );

        if ( UNLIKELY( glGetError() != GL_NO_ERROR ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Deleting font texture\n" );

            l_returnValue = false;

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
