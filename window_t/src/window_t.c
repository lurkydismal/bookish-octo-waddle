#include "window_t.h"

#include <stdlib.h>

#include "stdfunc.h"

window_t window_t$create( void ) {
    window_t l_returnValue = DEFAULT_WINDOW;

    {
        l_returnValue.name = duplicateString( DEFAULT_WINDOW_NAME );
    }

    return ( l_returnValue );
}

bool window_t$destroy( window_t* _window ) {
    bool l_returnValue = false;

    {
        if ( !( _window->name ) ) {
            goto EXIT;
        }

        free( _window->name );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
