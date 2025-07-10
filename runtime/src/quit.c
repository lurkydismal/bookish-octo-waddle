#include "quit.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdlib.h>

#include "FPS.h"
#include "asset_t.h"
#include "log.h"
#include "stdfunc.h"
#include "vsync.h"

static FORCE_INLINE const char* result$convert$toStaticString(
    const bool _result ) {
    return ( ( _result ) ? ( "SUCCESS" ) : ( "FAILURE" ) );
}

bool quit( applicationState_t* restrict _applicationState,
           const bool _result ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        // Report if SDL error occured before quitting
        {
            const char* l_SDLErrorMessage = SDL_GetError();

            if ( l_SDLErrorMessage[ 0 ] != '\0' ) {
                log$transaction$query$format(
                    ( logLevel_t )error, "Application exited with %s: '%s'",
                    result$convert$toStaticString( _result ),
                    l_SDLErrorMessage );
            }
        }

        // FPS
        {
            if ( UNLIKELY( !FPS$quit() ) ) {
                log$transaction$query( ( logLevel_t )error, "Quitting FPS" );

                goto EXIT;
            }
        }

        // Vsync
        {
            if ( UNLIKELY( !vsync$quit() ) ) {
                log$transaction$query( ( logLevel_t )error, "Quitting Vsync" );

                goto EXIT;
            }
        }

        // Settings
        {
            if ( UNLIKELY( !settings_t$unload(
                     &( _applicationState->settings ) ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Unloading settings" );

                goto EXIT;
            }
        }

        // Asset loader
        {
            if ( UNLIKELY( !asset_t$loader$quit() ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Quitting asset loader" );

                goto EXIT;
            }
        }

        // Application state
        if ( LIKELY( _applicationState ) ) {
            if ( UNLIKELY( !applicationState_t$unload( _applicationState ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Unloading application state" );
            }

            if ( LIKELY( _applicationState->renderer ) ) {
                SDL_DestroyRenderer( _applicationState->renderer );
            }

            if ( LIKELY( _applicationState->window ) ) {
                SDL_DestroyWindow( _applicationState->window );
            }

            if ( UNLIKELY(
                     !applicationState_t$destroy( _applicationState ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Destroying application state" );
            }
        }

        // Report application result
        if ( !_result ) {
            const char* l_SDLErrorMessage = SDL_GetError();

            if ( l_SDLErrorMessage[ 0 ] != '\0' ) {
                log$transaction$query$format(
                    ( logLevel_t )error, "Application shutdown with %s: '%s'",
                    result$convert$toStaticString( _result ),
                    l_SDLErrorMessage );
            }
        }

        // Log
        {
            if ( UNLIKELY( !log$quit() ) ) {
                trap( "Log quitting" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    SDL_Quit();

    return ( l_returnValue );
}
