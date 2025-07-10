#include "FPS.h"

#include <errno.h>
#include <pthread.h>
#include <time.h>

#include "log.h"
#include "stdfunc.h"

#if defined( HOT_RELOAD )

#include "applicationState_t.h"

#endif

static pthread_t g_FPSCountThread;
static bool g_shouldFPSCountThreadWork = false;
static size_t g_currentFramesPerSecond = 0;
static size_t* g_totalFramesPassed = NULL;

static void* FPS$count( void* _data ) {
    ( void )( sizeof( _data ) );

    const struct timespec l_sleepTime = {
        .tv_sec = 1,
        .tv_nsec = 0,
    };

    size_t l_previousTotalFramesPerSecond = 0;

    while ( LIKELY( g_shouldFPSCountThreadWork ) ) {
        g_currentFramesPerSecond =
            ( *g_totalFramesPassed - l_previousTotalFramesPerSecond );

        log$transaction$query$format( ( logLevel_t )info, "FPS: %zu",
                                      g_currentFramesPerSecond );

        l_previousTotalFramesPerSecond = *g_totalFramesPassed;

        clock_nanosleep( CLOCK_MONOTONIC, 0, &l_sleepTime, NULL );
    }

    return ( NULL );
}

bool FPS$init( size_t* restrict _totalFramesPassed ) {
    bool l_returnValue = false;

    if ( UNLIKELY( g_totalFramesPassed ) ) {
        log$transaction$query( ( logLevel_t )error,
                               "Total frames passed already exists" );

        goto EXIT;
    }

    {
        g_currentFramesPerSecond = 0;

        g_totalFramesPassed = _totalFramesPassed;

        g_shouldFPSCountThreadWork = true;

        if ( UNLIKELY( pthread_create( &g_FPSCountThread, NULL, FPS$count,
                                       NULL ) ) ) {
            log$transaction$query$format(
                ( logLevel_t )error,
                "%d: Insufficient resources to create another thread, or a "
                "system-imposed limit on the number of threads was "
                "encountered",
                EAGAIN );

            FPS$quit();

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool FPS$quit( void ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !g_totalFramesPassed ) ) {
        log$transaction$query( ( logLevel_t )error, "No total frames passed" );

        goto EXIT;
    }

    {
        g_shouldFPSCountThreadWork = false;

        pthread_join( g_FPSCountThread, NULL );

        g_currentFramesPerSecond = 0;

        g_totalFramesPassed = NULL;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

size_t FPS$get$current( void ) {
    size_t l_returnValue = 0;

    if ( UNLIKELY( !g_totalFramesPassed ) ) {
        log$transaction$query( ( logLevel_t )error, "No total frames passed" );

        l_returnValue = SIZE_MAX;

        goto EXIT;
    }

    {
        l_returnValue = g_currentFramesPerSecond;
    }

EXIT:
    return ( l_returnValue );
}

size_t FPS$get$total( void ) {
    size_t l_returnValue = 0;

    if ( UNLIKELY( !g_totalFramesPassed ) ) {
        log$transaction$query( ( logLevel_t )error, "No total frames passed" );

        l_returnValue = SIZE_MAX;

        goto EXIT;
    }

    {
        l_returnValue = *g_totalFramesPassed;
    }

EXIT:
    return ( l_returnValue );
}

#if defined( HOT_RELOAD )

bool hotReload$unload( void** restrict _state,
                       size_t* restrict _stateSize,
                       applicationState_t* restrict _applicationState ) {
    UNUSED( _applicationState );

    *_stateSize = ( sizeof( g_totalFramesPassed ) );
    *_state = malloc( *_stateSize );

    void* l_pointer = *_state;

#define APPEND_TO_STATE( _variable )                                   \
    do {                                                               \
        const size_t l_variableSize = sizeof( _variable );             \
        __builtin_memcpy( l_pointer, &( _variable ), l_variableSize ); \
        l_pointer += l_variableSize;                                   \
    } while ( 0 )

    APPEND_TO_STATE( g_totalFramesPassed );

#undef APPEND_TO_STATE

    if ( LIKELY( g_totalFramesPassed ) ) {
        if ( UNLIKELY( !FPS$quit() ) ) {
            trap( "Quitting FPS" );

            return ( false );
        }
    }

    return ( true );
}

bool hotReload$load( void* restrict _state,
                     size_t _stateSize,
                     applicationState_t* restrict _applicationState ) {
    UNUSED( _applicationState );

    bool l_returnValue = false;

    {
        size_t* l_totalFramesPassed = NULL;

        const size_t l_stateSize = ( sizeof( l_totalFramesPassed ) );

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

        DESERIALIZE_NEXT( l_totalFramesPassed );

#undef DESERIALIZE_NEXT

        if ( LIKELY( l_totalFramesPassed ) ) {
            if ( UNLIKELY( !FPS$init( l_totalFramesPassed ) ) ) {
                trap( "Initializing FPS" );
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

#endif
