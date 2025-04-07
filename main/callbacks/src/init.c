#include <glad/gl.h>

#include "FPS.h"
#include "asset_t.h"
#include "callbacks.h"
#include "log.h"
#include "stdfunc.h"

#define LOG_FILE_NAME_DEFAULT "log"
#define LOG_FILE_EXTENSION_DEFAULT "txt"
#define LOG_MAX_TRANSACTION_SIZE_DEFAULT ( ( 1024 * 10 ) * sizeof( char ) )

#define ASSETS_DIRECTORY "assets"

#define SETTINGS_FILE_NAME "settings"
#define SETTINGS_FILE_EXTENSION "ini"

// TODO: Comment
callbackResult_t init( applicationState_t* _applicationState ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    {
        // Log
        {
            if ( UNLIKELY( !log$init( LOG_FILE_NAME_DEFAULT,
                                      LOG_FILE_EXTENSION_DEFAULT,
                                      LOG_MAX_TRANSACTION_SIZE_DEFAULT ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Initializing logging system\n" );

                goto EXIT;
            }

#if defined( DEBUG )

            if ( UNLIKELY( !log$level$set( ( logLevel_t )debug ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Setting log level to DEBUG\n" );

                goto EXIT;
            }

#endif
        }

        // Asset loader
        {
            if ( UNLIKELY( !asset_t$loader$init( ASSETS_DIRECTORY ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Initializing asset loader\n" );

                goto EXIT;
            }
        }

        // Application state
        {
            *_applicationState = applicationState_t$create();
        }

        // Settings
        {
            if ( UNLIKELY( !settings_t$load( &( _applicationState->settings ),
                                             SETTINGS_FILE_NAME,
                                             SETTINGS_FILE_EXTENSION ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Loading settings\n" );

                log$transaction$query( ( logLevel_t )info,
                                       "Loading default settings\n" );

                _applicationState->settings = settings_t$create();
            }
        }

        // GLFW
        {
            if ( UNLIKELY( !glfwInit() ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Initializing GLFW\n" );

                goto EXIT;
            }

            _applicationState->window = glfwCreateWindow(
                _applicationState->settings.window.width,
                _applicationState->settings.window.height,
                _applicationState->settings.window.name, NULL, NULL );

            if ( UNLIKELY( !_applicationState->window ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Creating application window\n" );

                goto EXIT;
            }

            glfwMakeContextCurrent( _applicationState->window );
        }

        // glad
        {
            _applicationState->version = gladLoadGL( glfwGetProcAddress );

            log$transaction$query$format(
                ( logLevel_t )info, "GL version: %d.%d\n",
                GLAD_VERSION_MAJOR( _applicationState->version ),
                GLAD_VERSION_MINOR( _applicationState->version ) );
        }

        // Vsync
        {
            if ( UNLIKELY( !vsync$init(
                     _applicationState->settings.window.vsync,
                     _applicationState->settings.window.desiredFPS ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Initializing Vsync\n" );

                goto EXIT;
            }
        }

        // FPS
        {
            if ( UNLIKELY( !FPS$init(
                     &( _applicationState->totalFramesRendered ) ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Initializing FPS\n" );

                goto EXIT;
            }
        }

        l_returnValue = ( callbackResult_t )remain;
    }

EXIT:
    return ( l_returnValue );
}
