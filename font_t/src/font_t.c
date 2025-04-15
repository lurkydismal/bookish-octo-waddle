#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "font_t.h"

#include <stb/stb_image_write.h>

#include "log.h"

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

        if( UNLIKELY( l_fontCount == -1 ) ){
        }

        l_returnValue = stbtt_InitFont(&( _font->info ), _asset->data, 0);

        if ( UNLIKELY( !l_returnValue)) {
            goto EXIT;
        }

        float l_scale = stbtt_ScaleForPixelHeight(&info, _font->height);

        if ( UNLIKELY( !l_scale )) {
            l_returnValue = false;

            goto EXIT;
        }

        {
            uint8_t temp_bitmap[ FONT_ATLAS_W * FONT_ATLAS_H ];

            stbtt_BakeFontBitmap( _asset->data,
                                  0, // Font location offset
                                  _font->height, temp_bitmap, FONT_ATLAS_W,
                                  FONT_ATLAS_H, ASCII_START, CHARACTERS_COUNT,
                                  _font->glyphs );

            glGenTextures( 1, &( _font->texture ) );

            if ( UNLIKELY( !( _font->texture ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Generating font texture\n" );

                l_returnValue = false;

                goto EXIT;
            }

            glBindTexture( GL_TEXTURE_2D, _font->texture );

            if ( UNLIKELY( glGetError() != GL_NO_ERROR ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Binding font texture\n" );

                l_returnValue = false;

                goto EXIT;
            }

            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

            if ( UNLIKELY( glGetError() != GL_NO_ERROR ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Setting font texture parameters\n" );

                l_returnValue = false;

                goto EXIT;
            }

            glTexImage2D( GL_TEXTURE_2D,
                          0, // LOD
                          GL_ALPHA, FONT_ATLAS_W, FONT_ATLAS_H,
                          0, // Border width ( up to OGL 3.0 )
                          GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap );

            if ( UNLIKELY( glGetError() != GL_NO_ERROR ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Uploading font texture data\n" );

                l_returnValue = false;

                goto EXIT;
            }

        stbi_write_png( "fontAtlas.png", FONT_ATLAS_W, FONT_ATLAS_H, 1,
                        _font->glyphs, FONT_ATLAS_W );
        }

        l_returnValue = true;
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
                goto EXIT_IMAGE_ASSET;
            }

            l_returnValue = font_t$load$fromAsset( _font, &l_fontAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_IMAGE_ASSET;
            }

            l_returnValue = asset_t$unload( &l_fontAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_IMAGE_ASSET;
            }
        }

        l_returnValue = true;

    EXIT_IMAGE_ASSET:
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
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
