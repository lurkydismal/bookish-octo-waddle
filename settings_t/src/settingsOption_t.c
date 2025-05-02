#include "settingsOption_t.h"

#include <stdlib.h>

#include "stdfunc.h"

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

bool settingsOption_t$map( settingsOption_t* _settingsOption,
                           const char* _key,
                           void* const* _storage,
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

        _settingsOption->storage = NULL;
        _settingsOption->type =
            ( settingsOptionType_t )unknownSettingsOptionType;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool settingsOption_t$tryBind( settingsOption_t* _settingsOption,
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

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
