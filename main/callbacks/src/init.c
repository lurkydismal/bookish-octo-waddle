#include <glad/gl.h>
#include <stdio.h>

#include "asset_t.h"
#include "callbacks.h"
#include "log.h"

#define LOG_FILE_NAME_DEFAULT "log"
#define LOG_FILE_EXTENSION_DEFAULT "txt"
#define LOG_MAX_TRANSACTION_SIZE_DEFAULT ( 1024 * sizeof( char ) )

#define ASSETS_DIRECTORY "assets"

#define SETTINGS_FILE_NAME "settings"
#define SETTINGS_FILE_EXTENSION "ini"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGT 480
#define WINDOW_NAME "Window name"

// TODO: Comment
callbackResult_t init( applicationState_t* _applicationState ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    {
        // Log
        {
            if ( !log$init( LOG_FILE_NAME_DEFAULT, LOG_FILE_EXTENSION_DEFAULT,
                            LOG_MAX_TRANSACTION_SIZE_DEFAULT ) ) {
                log$transaction$query( ( logLevel_t )error, "Comment\n" );

                goto EXIT;
            }

#if defined( DEBUG )

            if ( !log$level$set( ( logLevel_t )debug ) ) {
                log$transaction$query( ( logLevel_t )error, "Comment\n" );

                goto EXIT;
            }

#endif
        }

        // Asset loader
        {
            if ( !asset_t$loader$init( ASSETS_DIRECTORY ) ) {
                log$transaction$query( ( logLevel_t )error, "Comment\n" );

                goto EXIT;
            }
        }

        // Application state
        {
            *_applicationState = applicationState_t$create();
        }

        // Settings
        {
            if ( !settings_t$load( &( _applicationState->settings ),
                                   SETTINGS_FILE_NAME,
                                   SETTINGS_FILE_EXTENSION ) ) {
                log$transaction$query( ( logLevel_t )error, "Comment\n" );

                goto EXIT;
            }
        }

        // GLFW
        {
            if ( !glfwInit() ) {
                log$transaction$query( ( logLevel_t )error, "Comment\n" );

                goto EXIT;
            }

            // TODO: Change this
            _applicationState->window = glfwCreateWindow(
                WINDOW_WIDTH, WINDOW_HEIGT, WINDOW_NAME, NULL, NULL );

            glfwMakeContextCurrent( _applicationState->window );
        }

        // glad
        {
            _applicationState->version = gladLoadGL( glfwGetProcAddress );

            log$transaction$query$format(
                ( logLevel_t )info, "GL: %d %d\n",
                GLAD_VERSION_MAJOR( _applicationState->version ),
                GLAD_VERSION_MINOR( _applicationState->version ) );
        }

        // TODO: Change this
        // Turn on Vsync
        glfwSwapInterval( 1 );

        l_returnValue = ( callbackResult_t )remain;
    }

EXIT:
    return ( l_returnValue );
}
