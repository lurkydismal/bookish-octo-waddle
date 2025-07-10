#include "window_t.h"

#include <stdlib.h>

#include "log.h"

window_t window_t$create( void ) {
    window_t l_returnValue = DEFAULT_WINDOW;

    {
        l_returnValue.name = duplicateString( DEFAULT_WINDOW_NAME );
    }

    return ( l_returnValue );
}

bool window_t$destroy( window_t* restrict _window ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_window ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        if ( UNLIKELY( !( _window->name ) ) ) {
            log$transaction$query( ( logLevel_t )error, "Invalid window name" );

            goto EXIT;
        }

        free( _window->name );

        _window->name = NULL;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
