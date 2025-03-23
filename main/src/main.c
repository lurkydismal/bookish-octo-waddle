#define GLFW_INCLUDE_NONE
#define GLAD_GL_IMPLEMENTATION

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

#include "applicationState_t.h"
#include "callbacks.h"
#include "log.h"
#include "stdfunc.h"

#define DESIRED_FPS 60

applicationState_t g_applicationState;

static void errorCallback( int _code, const char* _description ) {
    log$transaction$query$format( ( logLevel_t )error, "%d: %s\n", _code,
                                  _description );
}

static void keyCallback( GLFWwindow* _window,
                         int _key,
                         int _scancode,
                         int _action,
                         int _modifiers ) {
    ( void )( sizeof( _scancode ) );
    ( void )( sizeof( _action ) );

    callbackResult_t l_callbackResult =
        event( &g_applicationState, _key, _modifiers );

    if ( UNLIKELY( l_callbackResult != ( callbackResult_t )remain ) ) {
        glfwSetWindowShouldClose( _window, GLFW_TRUE );
    }
}

void* limitedIterate( void* _data ) {
    ( void )( sizeof( _data ) );

    callbackResult_t l_callbackResult = ( callbackResult_t )success;

    struct timespec l_sleepTime;

    l_sleepTime.tv_sec = 0;
    l_sleepTime.tv_nsec =
        MILLISECONDS_TO_NANOSECONDS( ONE_SECOND_IN_MILLISECONDS / DESIRED_FPS );

    while ( !glfwWindowShouldClose( g_applicationState.window ) ) {
        clock_nanosleep( CLOCK_MONOTONIC, 0, &l_sleepTime, NULL );

        l_callbackResult = iterate$limited( &g_applicationState );

        if ( UNLIKELY( l_callbackResult != ( callbackResult_t )remain ) ) {
            break;
        }
    }

    return ( NULL );
}

int main( void ) {
    callbackResult_t l_callbackResult = ( callbackResult_t )success;

    if ( UNLIKELY( glfwSetErrorCallback( errorCallback ) ) ) {
        goto EXIT;
    }

    l_callbackResult = init( &g_applicationState );

    if ( UNLIKELY( l_callbackResult != ( callbackResult_t )remain ) ) {
        goto EXIT;
    }

    glfwSetKeyCallback( g_applicationState.window, keyCallback );

    {
        pthread_t l_limitedIterateThread;

        // Limited iteration
        if ( pthread_create( &l_limitedIterateThread, NULL, limitedIterate,
                             NULL ) ) {
            errorCallback( 11,
                           "Insufficient resources to create another thread, "
                           "or a system-imposed limit on the number of threads "
                           "was encountered" );

            l_callbackResult = ( callbackResult_t )failure;

            goto EXIT_THREADS;
        }

        // Not limited iteration
        while ( !glfwWindowShouldClose( g_applicationState.window ) ) {
            l_callbackResult = iterate$unlimited( &g_applicationState );

            if ( UNLIKELY( l_callbackResult != ( callbackResult_t )remain ) ) {
                break;
            }

            glfwPollEvents();
        }

    EXIT_THREADS:
        pthread_join( l_limitedIterateThread, NULL );
    }

EXIT:
    return ( quit( &g_applicationState, l_callbackResult ) ==
             ( callbackResult_t )failure );
}
