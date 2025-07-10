#pragma once

#include <SDL3/SDL_render.h>
#include <stdbool.h>
#include <stddef.h>

#include "stdfloat16.h"
#include "stdfunc.h"
#include "vsync_t.h"

static FORCE_INLINE const char* vsync$convert$toStaticString(
    const vsync_t _vsync ) {
    switch ( _vsync ) {
        case ( vsync_t )off: {
            return ( VSYNC_TYPE_AS_STRING_OFF );
        }

        default: {
            return ( VSYNC_TYPE_AS_STRING_UNKNOWN );
        }
    }
}

static FORCE_INLINE vsync_t
vsync_t$convert$fromString( const char* restrict _string ) {
    if ( UNLIKELY( !_string ) ) {
        return ( ( vsync_t )unknownVsync );
    }

    if ( __builtin_strcmp( _string, VSYNC_TYPE_AS_STRING_OFF ) == 0 ) {
        return ( ( vsync_t )off );

    } else {
        return ( ( vsync_t )unknownVsync );
    }
}

bool vsync$init( const vsync_t _vsync,
                 const float16_t _desiredFPS,
                 SDL_Renderer* _renderer );
bool vsync$quit( void );

bool vsync$begin( void );
bool vsync$end( void );
