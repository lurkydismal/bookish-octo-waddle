#pragma once

#include <SDL3/SDL_scancode.h>
#include <stdbool.h>

#include "input_t.h"

#define CONTROL_AS_STRING_UNKNOWN "UNKNOWN"

#define DEFAULT_CONTROL                   \
    {                                     \
        .scancode = SDL_SCANCODE_UNKNOWN, \
        .input = DEFAULT_INPUT,           \
    }

typedef struct {
    SDL_Scancode scancode;
    input_t input;
} control_t;

control_t control_t$create( void );
bool control_t$destroy( control_t* restrict _control );

SDL_Scancode control_t$scancode$convert$fromString( const char* _string );
const char* control_t$scancode$convert$toStaticString( SDL_Scancode _scancode );
