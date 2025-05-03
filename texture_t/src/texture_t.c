#include "texture_t.h"

#include <glad/gl.h>

#include "image_t.h"
#include "log.h"
#include "stdfunc.h"

texture_t texture_t$create( void ) {
    texture_t l_returnValue = DEFAULT_TEXTURE;

    return ( l_returnValue );
}

bool texture_t$destroy( texture_t* restrict _texture ) {
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

bool texture_t$load$fromAsset( texture_t* restrict _texture,
                               asset_t* restrict _asset ) {
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
            log$transaction$query$format( ( logLevel_t )error,
                                          "Creating texture from memory: %s\n",
                                          ktxErrorString( l_result ) );

            goto EXIT;
        }

        // TODO: Refactor
        if ( ktxTexture2_NeedsTranscoding( _texture->data ) ) {
            GLint numFormats = 0;
            glGetIntegerv( GL_NUM_COMPRESSED_TEXTURE_FORMATS, &numFormats );
            GLint* formats = malloc( numFormats * sizeof( GLint ) );
            glGetIntegerv( GL_COMPRESSED_TEXTURE_FORMATS, formats );

            bool hasASTC = false, hasETC2 = false, hasBC3 = false;
            for ( int i = 0; i < numFormats; ++i ) {
                switch ( formats[ i ] ) {
                    case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
                        hasASTC = true;
                        break;
                    case GL_COMPRESSED_RGBA8_ETC2_EAC:
                        hasETC2 = true;
                        break;
                    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
                        hasBC3 = true;
                        break;
                    default:
                        break;
                }
            }
            free( formats );

            // 2) Pick a transcode target
            ktx_transcode_fmt_e tf = KTX_TTF_RGBA32; // fallback to raw
            if ( _texture->data->supercompressionScheme == KTX_SS_BASIS_LZ ) {
                if ( hasASTC )
                    tf = KTX_TTF_ASTC_4x4_RGBA;
                else if ( hasETC2 )
                    tf = KTX_TTF_ETC2_RGBA;
                else if ( hasBC3 )
                    tf = KTX_TTF_BC3_RGBA;
                // else leave as RGBA32
            }

            // 3) Transcode
            l_result = ktxTexture2_TranscodeBasis( _texture->data, tf, 0 );
            if ( UNLIKELY( l_result != KTX_SUCCESS ) ) {
                log$transaction$query$format( ( logLevel_t )error,
                                              "TranscodeBasis failed: %s\n",
                                              ktxErrorString( l_result ) );
                goto EXIT;
            }
        }

        GLenum l_glError = GL_NO_ERROR;

        l_result = ktxTexture_GLUpload( ktxTexture( _texture->data ),
                                        &( _texture->texture ),
                                        &( _texture->type ), &l_glError );

        if ( UNLIKELY( l_glError != GL_NO_ERROR ) ) {
            log$transaction$query$format(
                ( logLevel_t )error, "Uploading texture to OpenGL: '0x%X'\n",
                l_glError );

            goto EXIT;
        }

        if ( UNLIKELY( l_result != KTX_SUCCESS ) ) {
            log$transaction$query$format( ( logLevel_t )error,
                                          "Uploading texture to OpenGL: %s\n",
                                          ktxErrorString( l_result ) );

            goto EXIT;
        }

        log$transaction$query$format(
            ( logLevel_t )debug, "Texture width: %d, height: %d\n",
            _texture->data->baseWidth, _texture->data->baseHeight );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool texture_t$load$fromPath( texture_t* restrict _texture,
                              const char* restrict _path ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_texture ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_path ) ) {
        goto EXIT;
    }

    {
        asset_t l_textureAsset = asset_t$create();

        {
            l_returnValue = asset_t$load( &l_textureAsset, _path );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_TEXTURE_ASSET;
            }

            l_returnValue =
                texture_t$load$fromAsset( _texture, &l_textureAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_TEXTURE_ASSET;
            }

            l_returnValue = asset_t$unload( &l_textureAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_TEXTURE_ASSET;
            }
        }

        l_returnValue = true;

    EXIT_TEXTURE_ASSET:
        if ( UNLIKELY( !asset_t$destroy( &l_textureAsset ) ) ) {
            l_returnValue = false;

            goto EXIT;
        }
    }

EXIT:
    return ( l_returnValue );
}

bool texture_t$unload( texture_t* restrict _texture ) {
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
