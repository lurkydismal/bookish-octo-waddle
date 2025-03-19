#pragma once

#include <stdbool.h>
#include <stddef.h>

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

#define LOG_LEVEL_AS_STRING_DEFAULT LOG_LEVEL_AS_STRING_INFO
#define LOG_LEVEL_DEFAULT ( ( logLevel_t )info )

typedef enum { debug, info, warn, error, unknown } logLevel_t;

bool log$init( const char* _fileName,
               const char* _fileExtension,
               const size_t _maxTransactionSize );
bool log$quit( void );

bool log$level$set( const logLevel_t _logLevel );
bool log$level$set$string( const char* _string );

logLevel_t log$level$get( void );
const char* log$level$get$string( void );

bool log$transaction$query( const logLevel_t _logLevel, const char* _string );
bool log$transaction$commit( void );
