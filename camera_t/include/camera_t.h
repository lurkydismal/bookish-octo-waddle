#pragma once

#include <SDL3/SDL_rect.h>
#include <stdbool.h>

#include "player_t.h"
#include "stdfloat16.h"

#define DEFAULT_CAMERA  \
    {                   \
        .zoom = 1,      \
        .zoomMin = 0.1, \
        .zoomMax = 5,   \
        .rectangle =    \
            {           \
                0,      \
                0,      \
                0,      \
                0,      \
            },          \
    }

typedef struct {
    float16_t zoom;
    float16_t zoomMin;
    float16_t zoomMax;
    SDL_FRect rectangle;
} camera_t;

camera_t camera_t$create( void );
bool camera_t$destroy( camera_t* restrict _camera );

bool camera_t$update( camera_t* restrict _camera, const player_t* _player );
