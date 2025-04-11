#define STB_IMAGE_IMPLEMENTATION

#include "image_t.h"

#include <stb/stb_image.h>

#include "log.h"
#include "stdfunc.h"

bool image_t$loader$init( const char* _applicationName ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationName ) ) {
        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool image_t$loader$quit( void ) {
    bool l_returnValue = false;

    {
        l_returnValue = true;
    }

    return ( l_returnValue );
}

image_t image_t$create( void ) {
    image_t l_returnValue = DEFAULT_IMAGE;

    return ( l_returnValue );
}

bool image_t$destroy( image_t* _image ) {
    bool l_returnValue = false;

    {
        _image->id = NULL;
        _image->width = 0;
        _image->height = 0;
        _image->data = NULL;
        _image->size = 0;

        l_returnValue = true;
    }

    return ( l_returnValue );
}

bool image_t$load$fromAsset( image_t* _image, asset_t* _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_image ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
        stbi_set_flip_vertically_on_load( 1 );

        _image->data = stbi_load_from_memory(
            _asset->data, _asset->size, ( int* )( &( _image->width ) ),
            ( int* )( &( _image->height ) ),
            ( int* )( &( _image->channelCount ) ), 4 );

        if ( UNLIKELY( !( _image->data ) ) ) {
            log$transaction$query( ( logLevel_t )error, "Loading image\n" );

            l_returnValue = false;

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

// TODO: Improve
bool image_t$load$fromPath( image_t* _image, const char* _path ) {
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

bool image_t$unload( image_t* _image ) {
    bool l_returnValue = false;

    {
        free( _image->data );

        // TODO: Find out if really is needed
        free( _image->id );

        l_returnValue = true;
    }

    return ( l_returnValue );
}
