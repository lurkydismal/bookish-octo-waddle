#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "stdfunc.h"

#define COLOR_RED "\e[1;31m"
#define COLOR_GREEN "\e[1;32m"
#define COLOR_YELLOW "\e[1;33m"
#define COLOR_CYAN_LIGHT "\e[1;36m"
#define COLOR_RESET "\e[0m"

#define LOG_COLOR_DEBUG COLOR_CYAN_LIGHT
#define LOG_COLOR_INFO COLOR_GREEN
#define LOG_COLOR_WARN COLOR_YELLOW
#define LOG_COLOR_ERROR COLOR_RED
#define LOG_COLOR_UNKNOWN COLOR_RESET

#define LOG_LEVEL_AS_STRING_DEBUG "DEBUG"
#define LOG_LEVEL_AS_STRING_INFO "INFO"
#define LOG_LEVEL_AS_STRING_WARN "WARN"
#define LOG_LEVEL_AS_STRING_ERROR "ERROR"
#define LOG_LEVEL_AS_STRING_UNKNOWN "UNKNOWN"

#define LOG_COLOR_MAX_LENGTH __builtin_strlen( LOG_COLOR_DEBUG )
#define LOG_LEVEL_AS_STRING_MAX_LENGTH \
    __builtin_strlen( LOG_LEVEL_AS_STRING_UNKNOWN )

#define LOG_LEVEL_DEFAULT ( ( logLevel_t )warn )

#if defined( DEBUG )

#define DEBUG_INFORMATION_FORMAT \
    "Thread %lu: File '%s': line %u in function '%s' | Message: "
#define DEBUG_INFORMATION_TO_PRINT \
    syscall( SYS_gettid ), __FILE__, __LINE__, __func__

#define log$transaction$query( _logLevel, _string ) \
    log$transaction$query$format( _logLevel, _string )

#define log$transaction$query$format( _logLevel, _format, ... )      \
    _log$transaction$query$format( _logLevel,                        \
                                   DEBUG_INFORMATION_FORMAT _format, \
                                   DEBUG_INFORMATION_TO_PRINT, ##__VA_ARGS__ )

#else

#define log$transaction$query _log$transaction$query
#define log$transaction$query$format _log$transaction$query$format

#endif

#define LOG_MAX_TRANSACTION_SIZE_DEFAULT \
    ( ( size_t )( ( ( size_t )1024 * 10 ) * sizeof( char ) ) )

typedef enum { debug, info, warn, error, unknownLogLevel } logLevel_t;

static FORCE_INLINE const char* log$level$convert$toString(
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

bool log$init( const char* _fileName, const char* _fileExtension );
bool log$quit( void );

bool log$level$set( const logLevel_t _logLevel );
bool log$level$set$string( const char* _string );

logLevel_t log$level$get( void );
const char* log$level$get$string( void );

bool _log$transaction$query( const logLevel_t _logLevel, const char* _string );
bool _log$transaction$query$format( const logLevel_t _logLevel,
                                    const char* _format,
                                    ... );
bool log$transaction$commit( void );
