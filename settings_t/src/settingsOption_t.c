#include "settingsOption_t.h"

#include <stdlib.h>

#include "controls_t.h"
#include "log.h"

settingsOption_t settingsOption_t$create( void ) {
    settingsOption_t l_returnValue = DEFAULT_SETTINGS_OPTION;

    return ( l_returnValue );
}

bool settingsOption_t$destroy( settingsOption_t* restrict _settingsOption ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_settingsOption ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        _settingsOption->key = NULL;
        _settingsOption->storage = NULL;
        _settingsOption->type =
            ( settingsOptionType_t )unknownSettingsOptionType;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool _settingsOption_t$map( settingsOption_t* restrict _settingsOption,
                            const char* restrict _key,
                            void** restrict _storage,
                            const settingsOptionType_t _settingsOptionType ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_settingsOption ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_key ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_storage ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        _settingsOption->key = duplicateString( _key );
        _settingsOption->storage = _storage;
        _settingsOption->type = _settingsOptionType;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool settingsOption_t$unmap( settingsOption_t* restrict _settingsOption ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_settingsOption ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        free( _settingsOption->key );

        _settingsOption->key = NULL;

        _settingsOption->storage = NULL;

        _settingsOption->type =
            ( settingsOptionType_t )unknownSettingsOptionType;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool settingsOption_t$bind( settingsOption_t* restrict _settingsOption,
                            const char* restrict _key,
                            const char* restrict _value ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_settingsOption ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_key ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_value ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = ( __builtin_strcmp( _key, _settingsOption->key ) == 0 );

        if ( !l_returnValue ) {
            goto EXIT;
        }

        switch ( _settingsOption->type ) {
            case boolean: {
                *( ( bool* )( _settingsOption->storage ) ) =
                    stringToBool( _value );

                break;
            }

            case size: {
                *( ( size_t* )( _settingsOption->storage ) ) =
                    strtoull( _value, NULL, 10 );

                break;
            }

            case float16: {
                *( ( float16_t* )( _settingsOption->storage ) ) =
                    strtof( _value, NULL );

                break;
            }

            case scancode: {
                *( ( SDL_Scancode* )( _settingsOption->storage ) ) =
                    control_t$scancode$convert$fromString( _value );

                break;
            }

            case string: {
                free( *( _settingsOption->storage ) );

                *( ( char** )( _settingsOption->storage ) ) =
                    duplicateString( _value );

                break;
            }

            case vsync: {
                *( ( vsync_t* )( _settingsOption->storage ) ) =
                    vsync_t$convert$fromString( _value );

                break;
            }

            default: {
                log$transaction$query$format(
                    ( logLevel_t )error, "Unkown settings option type for '%s'",
                    _settingsOption->key );

                l_returnValue = false;

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
