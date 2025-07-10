#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>

#include "settings_t.h"

#define DEFAULT_APPLICATION_STATE     \
    {                                 \
        .window = NULL,               \
        .renderer = NULL,             \
        .settings = DEFAULT_SETTINGS, \
        .logicalWidth = 1280,         \
        .logicalHeight = 720,         \
        .totalFramesRendered = 0,     \
        .isPaused = false,            \
        .status = false,              \
    }

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    settings_t settings;
    size_t logicalWidth;
    size_t logicalHeight;
    size_t totalFramesRendered;
    bool isPaused;
    bool status;
} applicationState_t;

applicationState_t applicationState_t$create( void );
bool applicationState_t$destroy(
    applicationState_t* restrict _applicationState );

bool applicationState_t$load( applicationState_t* restrict _applicationState );
bool applicationState_t$unload(
    applicationState_t* restrict _applicationState );
