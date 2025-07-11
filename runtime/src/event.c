#include "event.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <stddef.h>

#include "log.h"

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
