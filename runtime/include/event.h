#pragma once

#include <SDL3/SDL_events.h>
#include <stdbool.h>

#include "applicationState_t.h"
#include "stdfunc.h"

typedef SDL_Event event_t;

EXPORT bool event( applicationState_t* restrict _applicationState,
            const event_t* restrict _event );
