#pragma once

#include <stddef.h>
#include <stdbool.h>

#define DEFAULT_WINDOW { \
    640, 480, 60, true \
}

typedef struct {
    size_t width;
    size_t height;
    size_t desiredFPS;
    bool vsync;
} window_t;

window_t window_t$create( void );
bool window_t$destroy( window_t* _window );
