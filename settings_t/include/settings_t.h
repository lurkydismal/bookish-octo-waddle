#pragma once

#include <stdbool.h>

#include "asset_t.h"
#include "window_t.h"

#define DEFAULT_SETTINGS_VERSION "0.1"
#define DEFAULT_SETTINGS_DESCRIPTION "brrr gamuingu"
#define DEFAULT_SETTINGS_CONTACT_ADDRESS "<lurkydismal@duck.com>"

#define DEFAULT_SETTINGS          \
    {                             \
        .window = DEFAULT_WINDOW, \
        .version = NULL,          \
        .identifier = NULL,       \
        .description = NULL,      \
        .contactAddress = NULL,   \
    }

// All available customization
typedef struct {
    window_t window;
    char* version;
    char* identifier;
    char* description;
    char* contactAddress;
} settings_t;

settings_t settings_t$create( void );
bool settings_t$destroy( settings_t* restrict _settings );

bool settings_t$load$fromAsset( settings_t* restrict _settings,
                                const asset_t* restrict _asset );
bool settings_t$load$fromPath( settings_t* restrict _settings,
                               const char* restrict _fileName,
                               const char* restrict _fileExtension );
bool settings_t$unload( settings_t* restrict _settings );
