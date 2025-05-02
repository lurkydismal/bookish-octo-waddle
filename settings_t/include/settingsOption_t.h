#pragma once

#include <stdbool.h>
#include <stddef.h>

#define settingsOptionType_t$getType$fromStorage( _storage ) \
    _Generic( ( _storage ),                                  \
        char*: ( settingsOptionType_t )( string ),           \
        size_t: ( settingsOptionType_t )( size ),            \
        float16_t: ( settingsOptionType_t )( float16 ),      \
        bool: ( settingsOptionType_t )( boolean ),           \
        vsync_t: ( settingsOptionType_t )( vsync ),          \
        default: ( settingsOptionType_t )( unknownSettingsOptionType ) )

#define DEFAULT_SETTINGS_OPTION \
    { .key = NULL, .storage = NULL, .type = unknownSettingsOptionType }

#define settingsOption_t$map( _settingsOption, _key, _storage ) \
    _settingsOption_t$map(                                      \
        _settingsOption, _key, ( void** )( _storage ),          \
        settingsOptionType_t$getType$fromStorage( *_storage ) )

typedef enum {
    string,
    size,
    float16,
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
bool settingsOption_t$destroy( settingsOption_t* _settingsOption );

bool _settingsOption_t$map( settingsOption_t* _settingsOption,
                            const char* _key,
                            void** _storage,
                            const settingsOptionType_t _settingsOptionType );
bool settingsOption_t$unmap( settingsOption_t* _settingsOption );

bool settingsOption_t$bind( settingsOption_t* _settingsOption,
                            const char* _key,
                            const char* _value );
