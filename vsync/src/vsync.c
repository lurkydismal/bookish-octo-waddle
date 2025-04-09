#define GLFW_INCLUDE_NONE

#include "vsync.h"

#include <GLFW/glfw3.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>

#include "FPS.h"
#include "log.h"

#if 0
static pthread_t g_adaptiveVsyncCheckThread;
static bool g_shouldAdapriveVsyncCheck = false;
#endif

static size_t g_desiredFPS = 0;
static vsync_t g_vsync = VSYNC_LEVEL_DEFAULT;

static struct timespec g_sleepTime, g_startTime, g_endTime;

vsync_t vsync_t$fromString( const char* _string ) {
    if ( UNLIKELY( !_string ) ) {
        return ( ( vsync_t )unknownVsync );
    }

    if ( __builtin_strcmp( _string, VSYNC_TYPE_AS_STRING_OFF ) == 0 ) {
        return ( ( vsync_t )off );

    } else if ( __builtin_strcmp( _string, VSYNC_TYPE_AS_STRING_NORMAL ) ==
                0 ) {
        return ( ( vsync_t )normal );

    } else if ( __builtin_strcmp( _string, VSYNC_TYPE_AS_STRING_ADAPTIVE ) ==
                0 ) {
        return ( ( vsync_t )adaptive );
    }

    return ( ( vsync_t )unknownVsync );
}

#if 0
static void* adapriveVsyncCheck( void* _data ) {
    ( void )( sizeof( _data ) );

    struct timespec l_sleepTime = { .tv_sec = 1, .tv_nsec = 0 };

    while ( LIKELY( g_shouldAdapriveVsyncCheck ) ) {
        if ( FPS$get$current() > g_desiredFPS ) {
            glfwSwapInterval( 1 );

        } else {
            glfwSwapInterval( 0 );
        }

        clock_nanosleep( CLOCK_MONOTONIC, 0, &l_sleepTime, NULL );
    }

    return ( NULL );
}
#endif

bool vsync$init( const vsync_t _vsync, const size_t _desiredFPS ) {
    bool l_returnValue = false;

    {
        g_desiredFPS = _desiredFPS;
        g_vsync = _vsync;

        if ( ( _vsync == ( vsync_t )off ) ||
             ( _vsync == ( vsync_t )adaptive ) ) {
            g_sleepTime.tv_sec = 0;
            g_sleepTime.tv_nsec = MILLISECONDS_TO_NANOSECONDS(
                ONE_SECOND_IN_MILLISECONDS / _desiredFPS );

            glfwSwapInterval( 0 );

        } else if ( _vsync == ( vsync_t )normal ) {
            glfwSwapInterval( 1 );
        }

#if 0
        if ( _vsync == ( vsync_t )adaptive ) {
            g_shouldAdapriveVsyncCheck = true;

            if ( UNLIKELY( pthread_create( &g_adaptiveVsyncCheckThread, NULL,
                                           adapriveVsyncCheck, NULL ) ) ) {
                log$transaction$query$format(
                    ( logLevel_t )error,
                    "%d: Insufficient resources to create another thread, or a "
                    "system-imposed limit on the number of threads was "
                    "encountered\n",
                    EAGAIN );

                vsync$quit();

                goto EXIT;
            }
        }
#endif

        l_returnValue = true;
    }

    return ( l_returnValue );
}

bool vsync$quit( void ) {
    bool l_returnValue = false;

    {
        g_desiredFPS = 0;

        if ( ( g_vsync == ( vsync_t )off ) ||
             ( g_vsync == ( vsync_t )adaptive ) ) {
            g_sleepTime.tv_nsec = 0;
        }

        // Default value
        if ( g_vsync != ( vsync_t )normal ) {
            glfwSwapInterval( 1 );
        }

#if 0
        if ( g_vsync == ( vsync_t )adaptive ) {
            g_shouldAdapriveVsyncCheck = false;

            pthread_join( g_adaptiveVsyncCheckThread, NULL );
        }
#endif

        l_returnValue = true;
    }

    return ( l_returnValue );
}

bool vsync$begin( void ) {
    bool l_returnValue = false;

    {
        if ( g_vsync == ( vsync_t )off ) {
            clock_gettime( CLOCK_MONOTONIC, &g_startTime );
        }

        l_returnValue = true;
    }

    return ( l_returnValue );
}

bool vsync$end( void ) {
    bool l_returnValue = false;

    {
        if ( g_vsync == ( vsync_t )off ) {
            clock_gettime( CLOCK_MONOTONIC, &g_endTime );

            struct timespec l_adjustedSleepTime;

            {
                const size_t l_iterationTimeNano =
                    ( ( g_endTime.tv_sec - g_startTime.tv_sec ) *
                          ( ONE_SECOND_IN_MILLISECONDS *
                            ONE_MILLISECOND_IN_NANOSECONDS ) +
                      ( g_endTime.tv_nsec - g_startTime.tv_nsec ) );

                long long l_adjustedSleepNano =
                    ( g_sleepTime.tv_nsec - l_iterationTimeNano );

                l_adjustedSleepNano &= -( l_adjustedSleepNano > 0 );

                l_adjustedSleepTime.tv_sec = 0;
                l_adjustedSleepTime.tv_nsec = l_adjustedSleepNano;
            }

            clock_nanosleep( CLOCK_MONOTONIC, 0, &l_adjustedSleepTime, NULL );
        }

        l_returnValue = true;
    }

    return ( l_returnValue );
}
