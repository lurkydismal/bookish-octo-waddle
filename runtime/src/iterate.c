#include "iterate.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "FPS.h"
#include "HUD_t.h"
#include "character_t.h"
#include "log.h"
#include "player_t.h"
#include "stdfunc.h"

bool iterate( applicationState_t* restrict _applicationState ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = vsync$begin();

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Vsync begin" );

            goto EXIT;
        }

#if defined( DEBUG )
        // Hot reload
        {
#define TRY_CHECK_WATCH_OR_EXIT( _field )                                  \
    do {                                                                   \
        FOR_ARRAY( watch_t* const*, _applicationState->_field->watches ) { \
            l_returnValue = watch_t$check( *_element, false );             \
            if ( UNLIKELY( !l_returnValue ) ) {                            \
                log$transaction$query( ( logLevel_t )error,                \
                                       "Checking " #_field " watch" );     \
                goto EXIT;                                                 \
            }                                                              \
        }                                                                  \
    } while ( 0 )

            TRY_CHECK_WATCH_OR_EXIT( background );
            TRY_CHECK_WATCH_OR_EXIT( HUD );
            TRY_CHECK_WATCH_OR_EXIT( character );

#undef TRY_CHECK_WATCH_OR_EXIT
        }
#endif

        if ( !_applicationState->isPaused ) {
            camera_t$update( &( _applicationState->camera ),
                             &( _applicationState->localPlayer ) );
        }

        // Render
        {
            SDL_RenderClear( _applicationState->renderer );

            // Background
            {
                l_returnValue = background_t$render(
                    _applicationState->background,
                    &( _applicationState->camera.rectangle ), false );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Rendering background" );

                    goto EXIT;
                }
            }

            // HUD
            {
                l_returnValue = HUD_t$render( _applicationState->HUD );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Rendering HUD" );

                    goto EXIT;
                }
            }

            // TODO: Players

            SDL_RenderPresent( _applicationState->renderer );
        }

        if ( !_applicationState->isPaused ) {
            // Step
            {
                // Background
                {
                    l_returnValue =
                        background_t$step( _applicationState->background );

                    if ( UNLIKELY( !l_returnValue ) ) {
                        log$transaction$query( ( logLevel_t )error,
                                               "Stepping background" );

                        goto EXIT;
                    }
                }

                // HUD
                {
                    l_returnValue = HUD_t$step( _applicationState->HUD );

                    if ( UNLIKELY( !l_returnValue ) ) {
                        log$transaction$query( ( logLevel_t )error,
                                               "Stepping HUD" );

                        goto EXIT;
                    }
                }

                // TODO: Players
            }
        }

        l_returnValue = vsync$end();

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Vsync end" );

            goto EXIT;
        }

        if ( ( _applicationState->totalFramesRendered % 60 ) == 0 ) {
            log$transaction$commit();
        }

        ( _applicationState->totalFramesRendered )++;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
