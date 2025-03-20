#include <glad/gl.h>
#include <stdio.h>

#include "asset_t.h"
#include "callbacks.h"
#include "log.h"

#define LOG_FILE_NAME_DEFAULT "log"
#define LOG_FILE_EXTENSION_DEFAULT "txt"
#define LOG_MAX_TRANSACTION_SIZE_DEFAULT ( 1024 * sizeof( char ) )

#define ASSETS_DIRECTORY "assets"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGT 480
#define WINDOW_NAME "Window name"

callbackResult_t init( applicationState_t* _applicationState ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    {
        // Log
        {
            if ( !log$init( LOG_FILE_NAME_DEFAULT, LOG_FILE_EXTENSION_DEFAULT,
                            LOG_MAX_TRANSACTION_SIZE_DEFAULT ) ) {
                goto EXIT;
            }

#if defined( DEBUG )

            if ( !log$level$set( ( logLevel_t )debug ) ) {
                goto EXIT;
            }

#endif
        }

        // Asset loader
        {
            if ( !asset_t$loader$init( ASSETS_DIRECTORY ) ) {
                goto EXIT;
            }
        }

        // GLFW
        {
            if ( !glfwInit() ) {
                goto EXIT;
            }

            _applicationState->window = glfwCreateWindow(
                WINDOW_WIDTH, WINDOW_HEIGT, WINDOW_NAME, NULL, NULL );

            glfwMakeContextCurrent( _applicationState->window );
        }

        // glad
        {
            _applicationState->version = gladLoadGL( glfwGetProcAddress );

            {
                char l_string[ 256 ];

                snprintf( l_string, sizeof( l_string ), "GL %d.%d\n",
                          GLAD_VERSION_MAJOR( _applicationState->version ),
                          GLAD_VERSION_MINOR( _applicationState->version ) );

                log$transaction$query( ( logLevel_t )info, l_string );
            }
        }

        // Turn on Vsync
        glfwSwapInterval( 1 );

        l_returnValue = ( callbackResult_t )remain;
    }

EXIT:
    return ( l_returnValue );
}
