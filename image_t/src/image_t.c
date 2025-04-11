#include "image_t.h"

#include "log.h"
#include "stdfunc.h"

#include <jxl/decode.h>

JxlDecoder* g_decoder = NULL;

bool image_t$loader$init( const char* _applicationName ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationName ) ) {
        goto EXIT;
    }

    {
        g_decoder = JxlDecoderCreate( NULL );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool image_t$loader$quit( void ) {
    bool l_returnValue = false;

    {
        JxlDecoderDestroy(g_decoder);

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

// TODO: Error handling
bool image_t$load$fromAsset( image_t* _image, asset_t* _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_image ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
        // Set up the decoder
        JxlDecoderSetInput(g_decoder, _asset->data, _asset->size);

        // Get image metadata (width, height, channels, etc.)
        JxlDecoderStatus status;
        JxlDecoderCloseInput(g_decoder);

        // Read the basic information about the image
        JxlBasicInfo basic_info;
        status = JxlDecoderGetBasicInfo(g_decoder, &basic_info);

        if (status != JXL_DEC_SUCCESS) {
            log$transaction$query$format((logLevel_t)debug, "Failed to get basic info: %u\n", (status));
            goto EXIT;
        }

        _image->width = basic_info.xsize;
        _image->height = basic_info.ysize;
        size_t img_channels = basic_info.num_color_channels;

        // Prepare the image buffer (RGBA)
        _image->data = (uint8_t*)malloc(_image->width * _image->height * img_channels * sizeof(uint8_t));

        if (!_image->data) {
            log$transaction$query((logLevel_t)debug,"Failed to allocate memory for image data\n");
            goto EXIT;
        }

        // Decode the image data
        status = JxlDecoderDecodePixels(g_decoder, _image->data);

        if (status != JXL_DEC_SUCCESS) {
            log$transaction$query$format((logLevel_t)debug, "Failed to decode image: %u\n", (status));
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
