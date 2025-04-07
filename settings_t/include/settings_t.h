#pragma once

#include <stdbool.h>

#include "window_t.h"

#define DEFAULT_SETTINGS \
    { .window = DEFAULT_WINDOW, .limitedLoopDesiredFPS = 60 }

// All available customization
typedef struct {
    window_t window;
    size_t limitedLoopDesiredFPS;
} settings_t;

settings_t settings_t$create( void );
bool settings_t$destroy( settings_t* _settings );

bool settings_t$load( settings_t* _settings,
                      const char* _fileName,
                      const char* _fileExtension );
bool settings_t$unload( settings_t* _settings );
