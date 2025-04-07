#define GLFW_INCLUDE_NONE

#include "vsync.h"

#include <GLFW/glfw3.h>
#include <stdint.h>
#include <time.h>

static vsync_t g_vsync = VSYNC_LEVEL_DEFAULT;

static struct timespec g_sleepTime, g_startTime, g_endTime;

vsync_t vsync_t$fromString( const char* _string ) {
    if ( UNLIKELY( !_string ) ) {
        return ( ( vsync_t )unknownVsync );
    }

    if ( __builtin_strcmp( _string, VSYNC_TYPE_AS_STRING_OFF ) ) {
        return ( ( vsync_t )off );

    } else if ( __builtin_strcmp( _string, VSYNC_TYPE_AS_STRING_NORMAL ) ) {
        return ( ( vsync_t )normal );

    } else if ( __builtin_strcmp( _string, VSYNC_TYPE_AS_STRING_ADAPTIVE ) ) {
        return ( ( vsync_t )adaptive );
    }

    return ( ( vsync_t )unknownVsync );
}

bool vsync$init( const vsync_t _vsync, const size_t _desiredFPS ) {
    bool l_returnValue = false;

    {
        if ( ( _vsync == ( vsync_t )off ) ||
             ( _vsync == ( vsync_t )adaptive ) ) {
            g_sleepTime.tv_sec = 0;
            g_sleepTime.tv_nsec = MILLISECONDS_TO_NANOSECONDS(
                ONE_SECOND_IN_MILLISECONDS / _desiredFPS );

        } else if ( _vsync == ( vsync_t )normal ) {
            glfwSwapInterval( 1 );
        }

        l_returnValue = true;
    }

    return ( l_returnValue );
}

bool vsync$quit( void ) {
    bool l_returnValue = false;

    {
        g_sleepTime.tv_nsec = 0;

        glfwSwapInterval( 0 );

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
