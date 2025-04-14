#include <glad/gl.h>

#include "FPS.h"
#include "asset_t.h"
#include "callbacks.h"
#include "image_t.h"
#include "log.h"
#include "stdfunc.h"

#define LOG_FILE_NAME_DEFAULT "log"
#define LOG_FILE_EXTENSION_DEFAULT "txt"

#define ASSETS_DIRECTORY "assets"

#define SETTINGS_FILE_NAME "settings"
#define SETTINGS_FILE_EXTENSION "ini"

callbackResult_t init( applicationState_t* restrict _applicationState ) {
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

            const logLevel_t l_logLevel = debug;

#elif defined( PROFILE )

            const logLevel_t l_logLevel = info;

#endif

#if ( defined( DEBUG ) || defined( PROFILE ) )

            if ( UNLIKELY( !log$level$set( l_logLevel ) ) ) {
                log$transaction$query$format(
                    ( logLevel_t )error, "Setting log level to %s\n",
                    log$level$convert$toString( l_logLevel ) );

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
            _applicationState->glVersion = gladLoadGL( glfwGetProcAddress );

            if ( UNLIKELY( !( _applicationState->glVersion ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Initializing OpenGL with glad\n" );

                goto EXIT;
            }

            log$transaction$query$format(
                ( logLevel_t )info, "GL version: %d.%d\n",
                GLAD_VERSION_MAJOR( _applicationState->glVersion ),
                GLAD_VERSION_MINOR( _applicationState->glVersion ) );
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
