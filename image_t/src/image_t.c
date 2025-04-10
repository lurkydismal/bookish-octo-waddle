#include "image_t.h"

#include <vips/vips.h>

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

bool image_t$destroy( void ) {
    bool l_returnValue = false;

    {
        l_returnValue = true;
    }

    return ( l_returnValue );
}

bool image_t$load( image_t* _image, asset_t* _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_image ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
        VipsImage* l_image = vips_image_new();

        vips_jxlload_buffer( _asset->data, _asset->size, &l_image );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool image_t$unload( image_t* _image ) {
    bool l_returnValue = false;

    {
        l_returnValue = true;
    }

    return ( l_returnValue );
}
