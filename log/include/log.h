#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "stdfunc.h"

#define LOG_COLOR_THREAD_ID ASCII_COLOR_PURPLE_LIGHT
#define LOG_COLOR_FILE_NAME ASCII_COLOR_PURPLE_LIGHT
#define LOG_COLOR_LINE_NUMBER ASCII_COLOR_PURPLE_LIGHT
#define LOG_COLOR_FUNCTION_NAME ASCII_COLOR_PURPLE_LIGHT

#define LOG_COLOR_DEBUG ASCII_COLOR_CYAN_LIGHT
#define LOG_COLOR_INFO ASCII_COLOR_GREEN
#define LOG_COLOR_WARN ASCII_COLOR_YELLOW
#define LOG_COLOR_ERROR ASCII_COLOR_RED
#define LOG_COLOR_UNKNOWN ASCII_COLOR_RESET_FOREGROUND

#define LOG_LEVEL_AS_STRING_DEBUG "DEBUG"
#define LOG_LEVEL_AS_STRING_INFO "INFO"
#define LOG_LEVEL_AS_STRING_WARN "WARN"
#define LOG_LEVEL_AS_STRING_ERROR "ERROR"
#define LOG_LEVEL_AS_STRING_UNKNOWN "UNKNOWN"

#if defined( DEBUG )

#define LOG_DEBUG_INFORMATION_FORMAT                                           \
    "Thread " LOG_COLOR_THREAD_ID "%zu" ASCII_COLOR_RESET_FOREGROUND           \
    ": File '" LOG_COLOR_FILE_NAME "%s" ASCII_COLOR_RESET_FOREGROUND           \
    "': line " LOG_COLOR_LINE_NUMBER "%u" ASCII_COLOR_RESET_FOREGROUND         \
    " in function '" LOG_COLOR_FUNCTION_NAME "%s" ASCII_COLOR_RESET_FOREGROUND \
    "' | Message: "

#define LOG_DEBUG_INFORMATION_ARGUMENTS \
    syscall( SYS_gettid ), __FILE__, __LINE__, __func__

#define log$transaction$query( _logLevel, _string ) \
    log$transaction$query$format( ( _logLevel ), "%s", _string )

#define log$transaction$query$format( _logLevel, _format, ... )  \
    ( {                                                          \
        _Static_assert( ( sizeof( #__VA_ARGS__ ) > 1 ),          \
                        "Missing variadic arguments" );          \
        _log$transaction$query$format(                           \
            ( _logLevel ), LOG_DEBUG_INFORMATION_FORMAT _format, \
            LOG_DEBUG_INFORMATION_ARGUMENTS, ##__VA_ARGS__ );    \
    } )

#else

#define log$transaction$query _log$transaction$query
#define log$transaction$query$format _log$transaction$query$format

#endif

#if defined( RELEASE )

#define LOG_MAX_TRANSACTION_SIZE_DEFAULT \
    ( ( size_t )( ( ( size_t )1024 * 10 ) * sizeof( char ) ) )

#else

#define LOG_MAX_TRANSACTION_SIZE_DEFAULT \
    ( ( size_t )( ( ( size_t )1024 * 10 * 100 ) * sizeof( char ) ) )

#endif

typedef enum { debug, info, warn, error, unknownLogLevel } logLevel_t;

static FORCE_INLINE const char* log$level$convert$toStaticString(
    const logLevel_t _logLevel ) {
    switch ( _logLevel ) {
        case ( logLevel_t )debug: {
            return ( LOG_LEVEL_AS_STRING_DEBUG );
        }

        case ( logLevel_t )info: {
            return ( LOG_LEVEL_AS_STRING_INFO );
        }

        case ( logLevel_t )warn: {
            return ( LOG_LEVEL_AS_STRING_WARN );
        }

        case ( logLevel_t )error: {
            return ( LOG_LEVEL_AS_STRING_ERROR );
        }

        default: {
            return ( LOG_LEVEL_AS_STRING_UNKNOWN );
        }
    }
}

static FORCE_INLINE logLevel_t
log$level$convert$fromString( const char* restrict _string ) {
    if ( UNLIKELY( !_string ) ) {
        return ( logLevel_t )unknownLogLevel;
    }

    if ( __builtin_strcmp( _string, LOG_LEVEL_AS_STRING_DEBUG ) == 0 ) {
        return ( ( logLevel_t )debug );

    } else if ( __builtin_strcmp( _string, LOG_LEVEL_AS_STRING_INFO ) == 0 ) {
        return ( ( logLevel_t )info );

    } else if ( __builtin_strcmp( _string, LOG_LEVEL_AS_STRING_WARN ) == 0 ) {
        return ( ( logLevel_t )warn );

    } else if ( __builtin_strcmp( _string, LOG_LEVEL_AS_STRING_ERROR ) == 0 ) {
        return ( ( logLevel_t )error );

    } else {
        return ( ( logLevel_t )unknownLogLevel );
    }
}

bool log$level$set( const logLevel_t _logLevel );
bool log$level$set$string( const char* restrict _string );

logLevel_t log$level$get( void );
const char* log$level$get$string( void );

bool log$init( const char* restrict _fileName,
               const char* restrict _fileExtension );
bool log$quit( void );

bool _log$transaction$query( const logLevel_t _logLevel,
                             const char* restrict _string );
bool _log$transaction$query$format( const logLevel_t _logLevel,
                                    const char* restrict _format,
                                    ... )
    __attribute__( ( format( printf,
                             2, // Format index
                             3  // First format argument index
                             ) ) );
bool log$transaction$commit( void );
