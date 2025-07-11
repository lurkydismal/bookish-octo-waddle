#include "vsync.h"

#include <stdint.h>
#include <time.h>

#include "FPS.h"
#include "log.h"

#if defined( HOT_RELOAD )

#include "applicationState_t.h"

#endif

static float16_t g_desiredFPS = 0;
static vsync_t g_vsync = VSYNC_LEVEL_DEFAULT;

static struct timespec g_sleepTime, g_startTime, g_endTime;

bool vsync$init( const vsync_t _vsync,
                 const float16_t _desiredFPS,
                 SDL_Renderer* _renderer ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( g_desiredFPS ) ) {
        log$transaction$query( ( logLevel_t )error, "Alraedy initialized" );

        goto EXIT;
    }

    {
        g_desiredFPS = _desiredFPS;
        g_vsync = _vsync;

        if ( _vsync == ( vsync_t )off ) {
            g_sleepTime.tv_sec = 0;
            g_sleepTime.tv_nsec = MILLISECONDS_TO_NANOSECONDS(
                ONE_SECOND_IN_MILLISECONDS / ( float )_desiredFPS );

            l_returnValue = SDL_SetRenderVSync(
                _renderer, SDL_WINDOW_SURFACE_VSYNC_DISABLED );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query$format( ( logLevel_t )error,
                                              "Setting renderer vsync: '%s'",
                                              SDL_GetError() );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool vsync$quit( void ) {
    bool l_returnValue = false;

    {
        g_desiredFPS = 0;

        if ( g_vsync == ( vsync_t )off ) {
            g_sleepTime.tv_nsec = 0;
        }

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
                    ( g_endTime.tv_nsec - g_startTime.tv_nsec );

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

#if defined( HOT_RELOAD )

struct state {
    float16_t g_desiredFPS;
    vsync_t g_vsync;
    struct timespec g_sleepTime, g_startTime, g_endTime;
};

EXPORT bool hotReload$unload( void** restrict _state,
                              size_t* restrict _stateSize,
                              applicationState_t* restrict _applicationState ) {
    UNUSED( _applicationState );

    *_stateSize = sizeof( struct state );
    *_state = malloc( *_stateSize );

    struct state l_state = {
        .g_desiredFPS = g_desiredFPS,
        .g_vsync = g_vsync,
        .g_sleepTime = g_sleepTime,
        .g_startTime = g_startTime,
        .g_endTime = g_endTime,
    };

    __builtin_memcpy( *_state, clone( &l_state ), *_stateSize );

    return ( true );
}

EXPORT bool hotReload$load( void* restrict _state,
                            size_t _stateSize,
                            applicationState_t* restrict _applicationState ) {
    UNUSED( _applicationState );

    bool l_returnValue = false;

    {
        const size_t l_stateSize = sizeof( struct state );

        if ( UNLIKELY( _stateSize != l_stateSize ) ) {
            trap( "Corrupted state" );

            goto EXIT;
        }

        struct state* l_state = ( struct state* )_state;

        g_desiredFPS = l_state->g_desiredFPS;
        g_vsync = l_state->g_vsync;
        g_sleepTime = l_state->g_sleepTime, g_startTime = l_state->g_startTime,
        g_endTime = l_state->g_endTime,

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

#endif
