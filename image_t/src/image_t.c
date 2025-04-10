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

bool image_t$load( image_t* _image, uint8_t* _data ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_image ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_data ) ) {
        goto EXIT;
    }

    {
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
