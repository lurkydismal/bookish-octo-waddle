#include "image_t.h"

#include "log.h"
#include "stdfunc.h"

bool image_t$loader$init( const char* _applicationName ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationName ) ) {
        goto EXIT;
    }

    {
        VIPS_INIT( _applicationName );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool image_t$loader$quit( void ) {
    bool l_returnValue = false;

    {
        vips_shutdown();

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
        _image->width = 0;
        _image->height = 0;
        _image->data = NULL;

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
        _image->id = vips_image_new();

        l_returnValue = ( vips_jxlload_buffer( _asset->data, _asset->size,
                                               &( _image->id ), NULL ) != -1 );

        if ( !l_returnValue ) {
            image_t$unload( _image );

            log$transaction$query$format( ( logLevel_t )info,
                                          "TEST1: %p %u %p\n", _asset->data,
                                          _asset->size, _image->id );

            goto EXIT;
        }

        _image->width = vips_image_get_width( _image->id );
        _image->height = vips_image_get_height( _image->id );

        VipsImage* rgba = NULL;
        {
            vips_colourspace( _image->id, &rgba, VIPS_INTERPRETATION_sRGB,
                              NULL );

            // Ensure 4 channels ( RGBA )
            vips_copy( rgba, &rgba, "bands", 4, NULL );

            // Ensure 8-bit
            vips_cast( rgba, &rgba, VIPS_FORMAT_UCHAR, NULL );

            _image->data =
                vips_image_write_to_memory( rgba, &( _image->size ) );

            l_returnValue = !!( _image->data );

            if ( !l_returnValue ) {
                image_t$unload( _image );

                log$transaction$query$format( ( logLevel_t )info,
                                              "TEST2: %p %u\n", _image->id,
                                              _asset->size );
                goto EXIT;
            }
        }

        log$transaction$query$format( ( logLevel_t )debug,
                                      "Image width: %d, height: %d\n",
                                      _image->width, _image->height );
        log$transaction$query$format( ( logLevel_t )debug, "Image format: %d\n",
                                      vips_image_get_format( rgba ) );

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
#if 0
        g_free( _image->data );

        // TODO: Find out if really is needed
        g_object_unref( _image->id );
#endif

        l_returnValue = true;
    }

    return ( l_returnValue );
}
