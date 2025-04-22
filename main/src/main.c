#define GLFW_INCLUDE_NONE
#define GLAD_GL_IMPLEMENTATION

#include <GLFW/glfw3.h>
#include <errno.h>
#include <glad/gl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

#include "applicationState_t.h"
#include "callbacks.h"
#include "log.h"
#include "stdfunc.h"

static applicationState_t g_applicationState;

static FORCE_INLINE void errorCallback( int _code, const char* _description ) {
    log$transaction$query$format( ( logLevel_t )error, "%d: %s\n", _code,
                                  _description );
}

static FORCE_INLINE void frameBufferSizeCallback( GLFWwindow* _window,
                                                  int _width,
                                                  int _height ) {
    ( void )( sizeof( _window ) );

    g_applicationState.settings.window.width = _width;
    g_applicationState.settings.window.height = _height;

    glViewport( 0, // X
                0, // Y
                _width, _height );
}

static FORCE_INLINE void keyCallback( GLFWwindow* _window,
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

    struct timespec l_sleepTime, l_startTime, l_endTime, l_adjustedSleepTime;

    l_sleepTime.tv_sec = 0;
    l_sleepTime.tv_nsec = MILLISECONDS_TO_NANOSECONDS(
        ONE_SECOND_IN_MILLISECONDS /
        g_applicationState.settings.limitedLoopDesiredFPS );

    while ( LIKELY( !glfwWindowShouldClose( g_applicationState.window ) ) ) {
        {
            clock_gettime( CLOCK_MONOTONIC, &l_startTime );

            l_callbackResult = iterate$limited( &g_applicationState );

            clock_gettime( CLOCK_MONOTONIC, &l_endTime );
        }

        {
            const size_t l_iterationTimeNano =
                ( ( l_endTime.tv_sec - l_startTime.tv_sec ) *
                      ( ONE_SECOND_IN_MILLISECONDS *
                        ONE_MILLISECOND_IN_NANOSECONDS ) +
                  ( l_endTime.tv_nsec - l_startTime.tv_nsec ) );

            long long l_adjustedSleepNano =
                ( l_sleepTime.tv_nsec - l_iterationTimeNano );

            l_adjustedSleepNano &= -( l_adjustedSleepNano > 0 );

            l_adjustedSleepTime.tv_sec = 0;
            l_adjustedSleepTime.tv_nsec = l_adjustedSleepNano;
        }

        clock_nanosleep( CLOCK_MONOTONIC, 0, &l_adjustedSleepTime, NULL );

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

    glfwSetFramebufferSizeCallback( g_applicationState.window,
                                    frameBufferSizeCallback );

    glfwSetKeyCallback( g_applicationState.window, keyCallback );

    {
        pthread_t l_limitedIterateThread = 0;

        // Limited iteration
        if ( UNLIKELY( pthread_create( &l_limitedIterateThread, NULL,
                                       limitedIterate, NULL ) ) ) {
            errorCallback( EAGAIN,
                           "Insufficient resources to create another thread, "
                           "or a system-imposed limit on the number of threads "
                           "was encountered" );

            l_callbackResult = ( callbackResult_t )failure;

            goto EXIT_THREADS;
        }

        // Unlimited iteration
        {
            while ( LIKELY(
                !glfwWindowShouldClose( g_applicationState.window ) ) ) {
                if ( UNLIKELY( !vsync$begin() ) ) {
                    l_callbackResult = ( callbackResult_t )failure;

                    goto LOOP_CONTINUE;
                }

                l_callbackResult = iterate$unlimited( &g_applicationState );

                if ( UNLIKELY( !vsync$end() ) ) {
                    l_callbackResult = ( callbackResult_t )failure;

                    goto LOOP_CONTINUE;
                }

            LOOP_CONTINUE:
                if ( UNLIKELY( l_callbackResult !=
                               ( callbackResult_t )remain ) ) {
                    break;
                }

                glfwPollEvents();
            }
        }

    EXIT_THREADS:
        pthread_join( l_limitedIterateThread, NULL );
    }

EXIT:
    return ( quit( &g_applicationState, l_callbackResult ) ==
             ( callbackResult_t )failure );
}
