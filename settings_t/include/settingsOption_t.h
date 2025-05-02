#pragma once

#include <stdbool.h>
#include <stddef.h>

#define DEFAULT_SETTINGS_OPTION \
    { .key = NULL, .storage = NULL, .type = unknownSettingsOptionType }

typedef enum {
    string,
    uint32,
    float16,
    boolean,
    unknownSettingsOptionType
} settingsOptionType_t;

typedef struct {
    char* key;
    void** storage;
    settingsOptionType_t type;
} settingsOption_t;

settingsOption_t settingsOption_t$create( void );
bool settingsOption_t$destroy( settingsOption_t* _settingsOption );

bool settingsOption_t$map( settingsOption_t* _settingsOption,
                           const char* _key,
                           void* const* _storage,
                           const settingsOptionType_t _settingsOptionType );
bool settingsOption_t$unmap( settingsOption_t* _settingsOption );

bool settingsOption_t$tryBind( settingsOption_t* _settingsOption,
                               const char* _key,
                               const char* _value );
