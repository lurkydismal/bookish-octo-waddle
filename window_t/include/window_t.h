#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "stdfunc.h"
#include "vsync.h"

#define DEFAULT_WINDOW_NAME "Gamuingu"

#define DEFAULT_WINDOW          \
    {                           \
        .name = NULL,           \
        .width = 640,           \
        .height = 480,          \
        .desiredFPS = 60,       \
        .vsync = DEFAULT_VSYNC, \
    }

typedef struct {
    char* name;
    size_t width;
    size_t height;
    size_t desiredFPS;
    vsync_t vsync;
} window_t;

window_t window_t$create( void );
bool window_t$destroy( window_t* restrict _window );
