#include "settingsOption_t.h"

#include <stdlib.h>

#include "log.h"
#include "stdfloat16.h"
#include "stdfunc.h"
#include "vsync.h"

static FORCE_INLINE const char* settingsOptionType_t$convert$toString(
    const settingsOptionType_t _settingsOptionType ) {
    switch ( _settingsOptionType ) {
        case ( string ): {
            return ( "string" );
        }

        case ( size ): {
            return ( "size" );
        }

        case ( float16 ): {
            return ( "float16" );
        }

        case ( boolean ): {
            return ( "boolean" );
        }

        case ( vsync ): {
            return ( "vsync" );
        }

        default: {
            return ( "unknownSettingsOptionType" );
        }
    }
}

settingsOption_t settingsOption_t$create( void ) {
    settingsOption_t l_returnValue = DEFAULT_SETTINGS_OPTION;

    return ( l_returnValue );
}

bool settingsOption_t$destroy( settingsOption_t* _settingsOption ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_settingsOption ) ) {
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

bool _settingsOption_t$map( settingsOption_t* _settingsOption,
                            const char* _key,
                            void** _storage,
                            const settingsOptionType_t _settingsOptionType ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_settingsOption ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_key ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_storage ) ) {
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

bool settingsOption_t$unmap( settingsOption_t* _settingsOption ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_settingsOption ) ) {
        goto EXIT;
    }

    {
        free( _settingsOption->key );

        switch ( _settingsOption->type ) {
            case ( string ): {
                free( *( _settingsOption->storage ) );

                break;
            }

            default: {
            }
        }

        _settingsOption->storage = NULL;
        _settingsOption->type =
            ( settingsOptionType_t )unknownSettingsOptionType;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool settingsOption_t$bind( settingsOption_t* _settingsOption,
                            const char* _key,
                            const char* _value ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_settingsOption ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_key ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_value ) ) {
        goto EXIT;
    }

    {
        if ( __builtin_strcmp( _key, _settingsOption->key ) != 0 ) {
            l_returnValue = false;

            goto EXIT;
        }

        switch ( _settingsOption->type ) {
            case ( string ): {
                free( *( _settingsOption->storage ) );

                *( ( char** )( _settingsOption->storage ) ) =
                    duplicateString( _value );

                break;
            }

            case ( size ): {
                *( ( size_t* )( _settingsOption->storage ) ) =
                    strtoull( _value, NULL, 10 );

                break;
            }

            case ( float16 ): {
                *( ( float16_t* )( _settingsOption->storage ) ) =
                    strtof( _value, NULL );

                break;
            }

            case ( boolean ): {
                *( ( bool* )( _settingsOption->storage ) ) =
                    stringToBool( _value );

                break;
            }

            case ( vsync ): {
                *( ( vsync_t* )( _settingsOption->storage ) ) =
                    vsync_t$fromString( _value );

                break;
            }

            default: {
                log$transaction$query$format(
                    ( logLevel_t )error, "Unkown settings option type '%s'\n",
                    settingsOptionType_t$convert$toString(
                        _settingsOption->type ) );

                l_returnValue = false;

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
