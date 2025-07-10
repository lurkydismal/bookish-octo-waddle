#pragma once

#include <SDL3/SDL_scancode.h>
#include <stdbool.h>
#include <stddef.h>

#include "stdfloat16.h"
#include "stdfunc.h"
#include "vsync.h"

#define settingsOptionType_t$getType$fromStorage( _storage ) \
    _Generic( ( _storage ),                                  \
        bool: ( settingsOptionType_t )( boolean ),           \
        size_t: ( settingsOptionType_t )( size ),            \
        float16_t: ( settingsOptionType_t )( float16 ),      \
        SDL_Scancode: ( settingsOptionType_t )( scancode ),  \
        char*: ( settingsOptionType_t )( string ),           \
        vsync_t: ( settingsOptionType_t )( vsync ),          \
        default: ( settingsOptionType_t )( unknownSettingsOptionType ) )

#define DEFAULT_SETTINGS_OPTION            \
    {                                      \
        .key = NULL,                       \
        .storage = NULL,                   \
        .type = unknownSettingsOptionType, \
    }

#define settingsOption_t$map( _settingsOption, _key, _storage ) \
    _settingsOption_t$map(                                      \
        _settingsOption, _key, ( void** )( _storage ),          \
        settingsOptionType_t$getType$fromStorage( *_storage ) )

typedef enum {
    string,
    size,
    float16,
    scancode,
    boolean,
    vsync,
    unknownSettingsOptionType
} settingsOptionType_t;

typedef struct {
    char* key;
    void** storage;
    settingsOptionType_t type;
} settingsOption_t;

settingsOption_t settingsOption_t$create( void );
bool settingsOption_t$destroy( settingsOption_t* restrict _settingsOption );

bool _settingsOption_t$map( settingsOption_t* restrict _settingsOption,
                            const char* restrict _key,
                            void** restrict _storage,
                            const settingsOptionType_t _settingsOptionType );
bool settingsOption_t$unmap( settingsOption_t* restrict _settingsOption );

bool settingsOption_t$bind( settingsOption_t* restrict _settingsOption,
                            const char* restrict _key,
                            const char* restrict _value );

static FORCE_INLINE const char* settingsOptionType_t$convert$toStaticString(
    const settingsOptionType_t _settingsOptionType ) {
    switch ( _settingsOptionType ) {
        case boolean: {
            return ( "boolean" );
        }

        case size: {
            return ( "size" );
        }

        case float16: {
            return ( "float16" );
        }

        case string: {
            return ( "string" );
        }

        case scancode: {
            return ( "scancode" );
        }

        case vsync: {
            return ( "vsync" );
        }

        default: {
            return ( "unknownSettingsOptionType" );
        }
    }
}
