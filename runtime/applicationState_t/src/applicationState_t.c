#include "applicationState_t.h"

#include "log.h"
#include "stdfunc.h"

applicationState_t applicationState_t$create( void ) {
    applicationState_t l_returnValue = DEFAULT_APPLICATION_STATE;

    {
        l_returnValue.settings = settings_t$create();
    }

    return ( l_returnValue );
}

bool applicationState_t$destroy(
    applicationState_t* restrict _applicationState ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
#define TRY_DESTROY_OR_EXIT( _field )                                         \
    do {                                                                      \
        l_returnValue = _field##_t$destroy( &( _applicationState->_field ) ); \
        if ( UNLIKELY( !l_returnValue ) ) {                                   \
            log$transaction$query( ( logLevel_t )error,                       \
                                   "Destroying " #_field );                   \
            goto EXIT;                                                        \
        }                                                                     \
    } while ( 0 )

        TRY_DESTROY_OR_EXIT( settings );

#undef TRY_DESTROY_OR_EXIT

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool applicationState_t$load( applicationState_t* restrict _applicationState ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
#define TRY_LOAD_OR_EXIT( _field )                                            \
    do {                                                                      \
        l_returnValue = _field##_t$load( _applicationState->_field,           \
                                         _applicationState->renderer );       \
        if ( UNLIKELY( !l_returnValue ) ) {                                   \
            log$transaction$query( ( logLevel_t )error, "Loading " #_field ); \
            goto EXIT;                                                        \
        }                                                                     \
    } while ( 0 )

#undef TRY_LOAD_OR_EXIT

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool applicationState_t$unload(
    applicationState_t* restrict _applicationState ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
#define TRY_UNLOAD_OR_EXIT( _field )                                         \
    do {                                                                     \
        l_returnValue = _field##_t$unload( &( _applicationState->_field ) ); \
        if ( UNLIKELY( !l_returnValue ) ) {                                  \
            log$transaction$query( ( logLevel_t )error,                      \
                                   "Unloading " #_field );                   \
            goto EXIT;                                                       \
        }                                                                    \
    } while ( 0 )

        TRY_UNLOAD_OR_EXIT( settings );

#undef TRY_UNLOAD_OR_EXIT

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
