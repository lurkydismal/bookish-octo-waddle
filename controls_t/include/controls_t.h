#pragma once

#include <SDL3/SDL_scancode.h>
#include <stdbool.h>

#include "control_t.h"
#include "stdfunc.h"

#define DEFAULT_CONTROLS          \
    {                             \
        .up = DEFAULT_CONTROL,    \
        .down = DEFAULT_CONTROL,  \
        .left = DEFAULT_CONTROL,  \
        .right = DEFAULT_CONTROL, \
        .A = DEFAULT_CONTROL,     \
        .B = DEFAULT_CONTROL,     \
        .C = DEFAULT_CONTROL,     \
        .D = DEFAULT_CONTROL,     \
    }

// All available controls
typedef struct {
    // Directions
    control_t up;
    control_t down;
    control_t left;
    control_t right;

    // Buttons
    control_t A;
    control_t B;
    control_t C;
    control_t D;
} controls_t;

controls_t controls_t$create( void );
bool controls_t$destroy( controls_t* restrict _controls );

static FORCE_INLINE control_t* controls_t$control_t$convert$fromScancode(
    controls_t* _controls,
    const SDL_Scancode _scancode ) {
    if ( UNLIKELY( !_controls ) ) {
        return ( NULL );
    }

    if ( UNLIKELY( !_scancode ) ) {
        return ( NULL );
    }

    {
#define RETURN_IF_SCANCODE_MATCHES( _field )             \
    do {                                                 \
        if ( _controls->_field.scancode == _scancode ) { \
            return ( &( _controls->_field ) );           \
        }                                                \
    } while ( 0 )

        // Directions
        {
            RETURN_IF_SCANCODE_MATCHES( up );
            RETURN_IF_SCANCODE_MATCHES( left );
            RETURN_IF_SCANCODE_MATCHES( right );
            RETURN_IF_SCANCODE_MATCHES( down );
        }

        // Buttons
        {
            RETURN_IF_SCANCODE_MATCHES( A );
            RETURN_IF_SCANCODE_MATCHES( B );
            RETURN_IF_SCANCODE_MATCHES( C );
            RETURN_IF_SCANCODE_MATCHES( D );
        }

#undef RETURN_IF_SCANCODE_MATCHES

        return ( NULL );
    }
}

static FORCE_INLINE control_t* controls_t$control_t$convert$fromInput(
    controls_t* _controls,
    const input_t* _input ) {
    if ( UNLIKELY( !_controls ) ) {
        return ( NULL );
    }

    if ( UNLIKELY( !_input ) ) {
        return ( NULL );
    }

    {
#define RETURN_IF_INPUT_MATCHES( _field )                     \
    do {                                                      \
        if ( _controls->_field.input.data == _input->data ) { \
            return ( &( _controls->_field ) );                \
        }                                                     \
    } while ( 0 )

        // Directions
        {
            RETURN_IF_INPUT_MATCHES( up );
            RETURN_IF_INPUT_MATCHES( left );
            RETURN_IF_INPUT_MATCHES( right );
            RETURN_IF_INPUT_MATCHES( down );
        }

        // Buttons
        {
            RETURN_IF_INPUT_MATCHES( A );
            RETURN_IF_INPUT_MATCHES( B );
            RETURN_IF_INPUT_MATCHES( C );
            RETURN_IF_INPUT_MATCHES( D );
        }

#undef RETURN_IF_INPUT_MATCHES

        return ( NULL );
    }
}
