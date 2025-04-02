#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "applicationState_t.h"

typedef bool ( *renderFunction_t )( applicationState_t* _applicationState );

typedef struct {
    renderFunction_t renderFunction;
} gameState_t;
