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
#include "stdfunc.h"

#define ONE_MILLISECOND_IN_NANOSECONDS 1000000
#define MILLISECONDS_TO_NANOSECONDS( _milliseconds ) \
    ( _milliseconds * ONE_MILLISECOND_IN_NANOSECONDS )

applicationState_t g_applicationState;

void errorCallback( int error, const char* description ) {
    fprintf( stderr, "Error: %d %s\n", error, description );
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

void* limitedIterate( void* data ) {
    ( void )( sizeof( data ) );

    struct timespec next_frame;

    next_frame.tv_sec = 0;
    next_frame.tv_nsec = MILLISECONDS_TO_NANOSECONDS( 16.6666667 );

    while ( !glfwWindowShouldClose( g_applicationState.window ) ) {
        clock_nanosleep( CLOCK_MONOTONIC, 0, &next_frame, NULL );
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
            printf( "error: \n" );

            l_callbackResult = ( callbackResult_t )failure;

            goto EXIT_THREADS;
        }

        // Not limited iteration
        while ( !glfwWindowShouldClose( g_applicationState.window ) ) {
            l_callbackResult = iterate( &g_applicationState );

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
