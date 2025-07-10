#include "iterate.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "FPS.h"
#include "log.h"
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

#undef TRY_CHECK_WATCH_OR_EXIT
        }
#endif

        if ( !_applicationState->isPaused ) {
        }

        // Render
        {
            SDL_RenderClear( _applicationState->renderer );

            SDL_RenderPresent( _applicationState->renderer );
        }

        if ( !_applicationState->isPaused ) {
            // Step
            {
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
