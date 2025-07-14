#include "controls_t.h"

#include "input_t.h"
#include "log.h"
#include "stdfunc.h"

controls_t controls_t$create( void ) {
    controls_t l_returnValue = DEFAULT_CONTROLS;

    {
        // Create
        {
            // Directions
            {
                l_returnValue.up = control_t$create();
                l_returnValue.down = control_t$create();
                l_returnValue.left = control_t$create();
                l_returnValue.right = control_t$create();
            }

            // Buttons
            {
                l_returnValue.A = control_t$create();
                l_returnValue.B = control_t$create();
                l_returnValue.C = control_t$create();
                l_returnValue.D = control_t$create();
            }
        }

        // Directions
        {
            l_returnValue.up.input.data = UP;
            l_returnValue.down.input.data = DOWN;
            l_returnValue.left.input.data = LEFT;
            l_returnValue.right.input.data = RIGHT;
        }

        // Buttons
        {
            l_returnValue.A.input.data = A;
            l_returnValue.B.input.data = B;
            l_returnValue.C.input.data = C;
            l_returnValue.D.input.data = D;
        }
    }

    return ( l_returnValue );
}

bool controls_t$destroy( controls_t* restrict _controls ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_controls ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
#define DESTROY_DONTROL_OR_EXIT( _field )                                     \
    do {                                                                      \
        if ( UNLIKELY( !( control_t$destroy( &( _controls->_field ) ) ) ) ) { \
            log$transaction$query( ( logLevel_t )error,                       \
                                   "Destroying control" #_field );            \
            goto EXIT;                                                        \
        }                                                                     \
    } while ( 0 )

        DESTROY_DONTROL_OR_EXIT( up );
        DESTROY_DONTROL_OR_EXIT( down );
        DESTROY_DONTROL_OR_EXIT( left );
        DESTROY_DONTROL_OR_EXIT( right );
        DESTROY_DONTROL_OR_EXIT( A );
        DESTROY_DONTROL_OR_EXIT( B );
        DESTROY_DONTROL_OR_EXIT( C );
        DESTROY_DONTROL_OR_EXIT( D );

#undef DESTROY_DONTROL_OR_EXIT

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
