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

bool hotReload$unload( void** restrict _state,
                       size_t* restrict _stateSize,
                       applicationState_t* restrict _applicationState ) {
    UNUSED( _applicationState );

    *_stateSize =
        ( sizeof( g_desiredFPS ) + sizeof( g_vsync ) + sizeof( g_sleepTime ) +
          sizeof( g_startTime ) + sizeof( g_endTime ) );
    *_state = malloc( *_stateSize );

    void* l_pointer = *_state;

#define APPEND_TO_STATE( _variable )                                   \
    do {                                                               \
        const size_t l_variableSize = sizeof( _variable );             \
        __builtin_memcpy( l_pointer, &( _variable ), l_variableSize ); \
        l_pointer += l_variableSize;                                   \
    } while ( 0 )

    APPEND_TO_STATE( g_desiredFPS );
    APPEND_TO_STATE( g_vsync );
    APPEND_TO_STATE( g_sleepTime );
    APPEND_TO_STATE( g_startTime );
    APPEND_TO_STATE( g_endTime );

#undef APPEND_TO_STATE

    return ( true );
}

bool hotReload$load( void* restrict _state,
                     size_t _stateSize,
                     applicationState_t* restrict _applicationState ) {
    UNUSED( _applicationState );

    bool l_returnValue = false;

    {
        const size_t l_stateSize =
            ( sizeof( g_desiredFPS ) + sizeof( g_vsync ) +
              sizeof( g_sleepTime ) + sizeof( g_startTime ) +
              sizeof( g_endTime ) );

        if ( UNLIKELY( _stateSize != l_stateSize ) ) {
            trap( "Corrupted state" );

            goto EXIT;
        }

        void* l_pointer = _state;

#define DESERIALIZE_NEXT( _variable )                       \
    do {                                                    \
        const size_t l_variableSize = sizeof( _variable );  \
        _variable = *( ( typeof( _variable )* )l_pointer ); \
        l_pointer += l_variableSize;                        \
    } while ( 0 )

        DESERIALIZE_NEXT( g_desiredFPS );
        DESERIALIZE_NEXT( g_vsync );
        DESERIALIZE_NEXT( g_sleepTime );
        DESERIALIZE_NEXT( g_startTime );
        DESERIALIZE_NEXT( g_endTime );

#undef DESERIALIZE_NEXT

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

#endif
