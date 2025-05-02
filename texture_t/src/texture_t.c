#include "texture_t.h"

#include "image_t.h"
#include "log.h"
#include "stdfunc.h"

#if defined( WRITE_TEXTURE_TO_IMAGE )

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb/stb_image_write.h>

#endif

texture_t texture_t$create( void ) {
    texture_t l_returnValue = DEFAULT_TEXTURE;

    return ( l_returnValue );
}

bool texture_t$destroy( texture_t* _texture ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_texture ) ) {
        goto EXIT;
    }

    {
        _texture->texture = 0;
        _texture->type = 0;
        _texture->data = NULL;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool texture_t$load$fromAsset( texture_t* _texture, asset_t* _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_texture ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
        KTX_error_code l_result = KTX_ERROR_MAX_ENUM;

        l_result = ktxTexture2_CreateFromMemory(
            _asset->data, _asset->size, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
            &( _texture->data ) );

        if ( UNLIKELY( l_result != KTX_SUCCESS ) ) {
            goto EXIT;
        }

        GLenum l_error;

        l_result = ktxTexture_GLUpload( ktxTexture( _texture->texture ),
                                        &( _texture->texture ),
                                        &( _texture->type ), &l_error );

        if ( UNLIKELY( l_result != KTX_SUCCESS ) ) {
            log$transaction$query( ( logLevel_t )error, "Error\n" );

            goto EXIT;
        }

        if ( UNLIKELY( l_error != GL_NO_ERROR ) ) {
            log$transaction$query( ( logLevel_t )error, "Error\n" );

            goto EXIT;
        }

        log$transaction$query$format(
            ( logLevel_t )debug, "Image width: %d, height: %d\n",
            _texture->data->baseWidth, _texture->data->baseHeight );

#if defined( WRITE_TEXTURE_TO_IMAGE )

        const size_t l_width = _texture->data->baseWidth;
        const size_t l_height = _texture->data->baseHeight;

        size_t l_imageOffset;

        if ( UNLIKELY( ktxTexture_GetImageOffset( ktxTexture( _texture->data ),
                                                  0, 0, 0, &l_imageOffset ) !=
                       KTX_SUCCESS ) ) {
            log$transaction$query( ( logLevel_t )error, "Error\n" );

            l_returnValue = false;

            goto EXIT;
        }

        const char* l_exportFileName = "texture.png";

        stbi_write_png( l_exportFileName, l_width, l_height, RGBA_PIXEL_SIZE,
                        ( ktxTexture_GetData( ktxTexture( _texture->data ) ) +
                          l_imageOffset ),
                        ( l_width * RGBA_PIXEL_SIZE ) );

        log$transaction$query$format(
            ( logLevel_t )info, "%s' file was created\n", l_exportFileName );

#endif

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool texture_t$unload( texture_t* _texture ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_texture ) ) {
        goto EXIT;
    }

    {
        ktxTexture2_Destroy( _texture->data );
        _texture->texture = 0;
        _texture->type = 0;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
