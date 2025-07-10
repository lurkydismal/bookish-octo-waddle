#include "event.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <stddef.h>

#include "controls_t.h"
#include "inputBuffer_t.h"
#include "log.h"
#include "stdfunc.h"

static FORCE_INLINE bool onWindowResize(
    applicationState_t* restrict _applicationState,
    float _width,
    float _height ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        static size_t l_lastResizeFrame = 0;
        const size_t l_totalFramesRendered =
            _applicationState->totalFramesRendered;

        if ( l_lastResizeFrame < l_totalFramesRendered ) {
            const float l_logicalWidth = _applicationState->logicalWidth;
            const float l_logicalHeigth = _applicationState->logicalHeight;

            const float l_scaleX = ( _width / l_logicalWidth );
            const float l_scaleY = ( _height / l_logicalHeigth );

            if ( !SDL_SetRenderScale( _applicationState->renderer, l_scaleX,
                                      l_scaleY ) ) {
                l_returnValue = false;

                log$transaction$query$format( ( logLevel_t )error,
                                              "Setting render scale: '%s'",
                                              SDL_GetError() );

                goto EXIT;
            }
        }

        l_lastResizeFrame = l_totalFramesRendered;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool handleKeyboardState(
    applicationState_t* restrict _applicationState ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        static size_t l_lastInputFrame = 0;
        const size_t l_totalFramesRendered =
            _applicationState->totalFramesRendered;

        if ( l_lastInputFrame < l_totalFramesRendered ) {
            input_t l_input = input_t$create();

            {
                int l_keysAmount = 0;
                const bool* l_keysState = SDL_GetKeyboardState( &l_keysAmount );

                ASSUME( l_keysAmount == SDL_SCANCODE_COUNT );

                const size_t l_SDLScancodeFirst = SDL_SCANCODE_A;

                FOR_RANGE( int, l_SDLScancodeFirst, l_keysAmount ) {
                    // If pressed
                    if ( l_keysState[ _index ] ) {
                        SDL_Scancode l_scancode = _index;

                        const control_t* l_control =
                            controls_t$control_t$convert$fromScancode(
                                &( _applicationState->settings.controls ),
                                l_scancode );

                        if ( l_control ) {
                            l_input.data |= l_control->input.data;
                        }
                    }
                }
            }

            l_returnValue =
                player_t$input$add( &( _applicationState->localPlayer ),
                                    &l_input, l_totalFramesRendered );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Adding player input" );

                goto EXIT;
            }

            {
                // TODO: Fix
#if 1
                input_t** l_inputs = player_t$inputsSequences$get$withLimit(
                    &( _applicationState->localPlayer ),
                    _applicationState->totalFramesRendered + 1, 8 );

                if ( arrayLength( l_inputs ) ) {
                    static bool asd = false;
                    static char* l_result = NULL;

                    if ( !asd ) {
                        asd = true;

                        l_result = ( char* )malloc( 100 );
                    }

                    size_t l_len = 0;

                    FOR_ARRAY_REVERSE( input_t* const*, l_inputs ) {
                        const char* s =
                            input_t$convert$toStaticString( *_element );
                        const size_t sl = __builtin_strlen( s );

                        __builtin_memcpy( ( l_result + l_len ), s, sl );

                        l_len += sl;
                    }

                    l_result[ l_len ] = '\0';

                    log$transaction$query$format( ( logLevel_t )debug, "SEQ %s",
                                                  l_result );
                }

                FREE_ARRAY( l_inputs );
#endif
            }
        }

        l_lastInputFrame = l_totalFramesRendered;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool event( applicationState_t* restrict _applicationState,
            const event_t* restrict _event ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        // TODO: Improve
        if ( !_event ) {
            l_returnValue = handleKeyboardState( _applicationState );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Handling keyboard state" );

                goto EXIT;
            }

        } else {
            switch ( _event->type ) {
                case SDL_EVENT_QUIT: {
                    _applicationState->status = true;

                    l_returnValue = false;

                    goto EXIT;
                }

                case SDL_EVENT_WINDOW_RESIZED: {
                    const float l_newWidth = _event->window.data1;
                    const float l_newHeight = _event->window.data2;

                    l_returnValue = onWindowResize( _applicationState,
                                                    l_newWidth, l_newHeight );

                    if ( UNLIKELY( !l_returnValue ) ) {
                        log$transaction$query( ( logLevel_t )error,
                                               "Handling window resize" );

                        goto EXIT;
                    }

                    break;
                }

                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP: {
                    l_returnValue = handleKeyboardState( _applicationState );

                    if ( UNLIKELY( !l_returnValue ) ) {
                        log$transaction$query( ( logLevel_t )error,
                                               "Handling keyboard state" );

                        goto EXIT;
                    }
                }

                default: {
                }
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
