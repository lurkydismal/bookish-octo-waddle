#include "log.h"

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#if defined( HOT_RELOAD )

#include "applicationState_t.h"

#endif

#define LOG_LEVEL_DEFAULT ( ( logLevel_t )warn )

static int g_fileDescriptor = -1;
static char* g_transactionString = NULL;
static ssize_t g_transactionSize = 0;
static logLevel_t g_currentLogLevel = LOG_LEVEL_DEFAULT;

static const char* log$level$convert$toColor( const logLevel_t _logLevel ) {
    switch ( _logLevel ) {
        case ( logLevel_t )debug: {
            return ( LOG_COLOR_DEBUG );
        }

        case ( logLevel_t )info: {
            return ( LOG_COLOR_INFO );
        }

        case ( logLevel_t )warn: {
            return ( LOG_COLOR_WARN );
        }

        case ( logLevel_t )error: {
            return ( LOG_COLOR_ERROR );
        }

        default: {
            return ( LOG_COLOR_UNKNOWN );
        }
    }
}

static size_t log$level$prependToString( char* restrict* restrict _string,
                                         const logLevel_t _logLevel ) {
    size_t l_returnValue = 0;

    {
        if ( UNLIKELY( !_string ) || UNLIKELY( !*_string ) ) {
            log$transaction$query( ( logLevel_t )error, "Invalid argument" );

            goto EXIT;
        }

        {
            char* l_logLevelWithBrackets = duplicateString(
                log$level$convert$toStaticString( _logLevel ) );

            concatBeforeAndAfterString( &l_logLevelWithBrackets, "[", "]" );

            // Colored
            concatBeforeAndAfterString( &l_logLevelWithBrackets,
                                        log$level$convert$toColor( _logLevel ),
                                        ASCII_COLOR_RESET_FOREGROUND );

            concatBeforeAndAfterString( _string, " ", NULL );

            // Length of string with log level
            l_returnValue = concatBeforeAndAfterString(
                _string, l_logLevelWithBrackets, NULL );

            free( l_logLevelWithBrackets );
        }
    }

EXIT:
    return ( l_returnValue );
}

bool log$level$set( const logLevel_t _logLevel ) {
    bool l_returnValue = false;

    {
        g_currentLogLevel = _logLevel;

        l_returnValue = true;
    }

    return ( l_returnValue );
}

bool log$level$set$string( const char* restrict _string ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_string ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        g_currentLogLevel = log$level$convert$fromString( _string );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

logLevel_t log$level$get( void ) {
    return ( g_currentLogLevel );
}

const char* log$level$get$string( void ) {
    return ( log$level$convert$toStaticString( g_currentLogLevel ) );
}

bool log$init( const char* restrict _fileName,
               const char* restrict _fileExtension ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_fileName ) ) {
        trap( "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_fileExtension ) ) {
        trap( "Invalid argument" );

        goto EXIT;
    }

    {
        // Open file descriptor
        {
            char* l_filePath = duplicateString( "." );

            concatBeforeAndAfterString( &l_filePath, _fileName,
                                        _fileExtension );

            // Prepend absolute path to executable directory
            {
                char* l_directoryPath = getApplicationDirectoryAbsolutePath();

                // Construct full file path
                concatBeforeAndAfterString( &l_filePath, l_directoryPath,
                                            NULL );

                free( l_directoryPath );
            }

            // 0 - No special bits
            // 6 - Read & Write for owner
            // 4 - Read for group members
            // 4 - Read for others
            g_fileDescriptor =
                open( l_filePath, ( O_WRONLY | O_TRUNC | O_CREAT ), 0644 );

            free( l_filePath );

            l_returnValue = ( g_fileDescriptor != -1 );

            if ( UNLIKELY( !l_returnValue ) ) {
                trap( "Corrupted file descritor" );
            }
        }

        // Allocate transaction string
        g_transactionString =
            ( char* )malloc( LOG_MAX_TRANSACTION_SIZE_DEFAULT );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool log$quit( void ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !g_transactionString ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( g_fileDescriptor == -1 ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        free( g_transactionString );

        g_transactionString = NULL;

        g_transactionSize = 0;

        g_currentLogLevel = ( logLevel_t )unknownLogLevel;

        l_returnValue = ( close( g_fileDescriptor ) != -1 );

        if ( UNLIKELY( !l_returnValue ) ) {
            trap( "Corrupted file descriptor" );
        }

        g_fileDescriptor = -1;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE void appendColorReset( void ) {
    const size_t l_colorResetLength = __builtin_strlen( ASCII_COLOR_RESET );

    __builtin_memcpy( ( g_transactionString + g_transactionSize ),
                      ASCII_COLOR_RESET, l_colorResetLength );

    g_transactionSize += l_colorResetLength;
}

bool _log$transaction$query( const logLevel_t _logLevel,
                             const char* restrict _string ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !g_transactionString ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_string ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( _logLevel < g_currentLogLevel ) {
        goto EXIT;
    }

    {
        {
            char* l_string = duplicateString( _string );

            size_t l_stringLength =
                log$level$prependToString( &l_string, _logLevel );
            const size_t l_colorResetLength =
                __builtin_strlen( ASCII_COLOR_RESET );

            if ( UNLIKELY( ( g_transactionSize + l_stringLength +
                             l_colorResetLength ) >
                           LOG_MAX_TRANSACTION_SIZE_DEFAULT ) ) {
                l_stringLength =
                    ( LOG_MAX_TRANSACTION_SIZE_DEFAULT - g_transactionSize );
            }

            __builtin_memcpy( ( g_transactionString + g_transactionSize ),
                              l_string, l_stringLength );

            g_transactionSize += l_stringLength;

            appendColorReset();

            g_transactionString[ g_transactionSize ] = '\n';

            g_transactionSize++;

            free( l_string );
        }

        if ( UNLIKELY( _logLevel == ( logLevel_t )error ) ) {
            log$transaction$commit();
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool _log$transaction$query$format( const logLevel_t _logLevel,
                                    const char* restrict _format,
                                    ... ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !g_transactionString ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_format ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( _logLevel < g_currentLogLevel ) {
        goto EXIT;
    }

    {
        size_t l_bufferSize =
            ( LOG_MAX_TRANSACTION_SIZE_DEFAULT - g_transactionSize );
        char* l_buffer = ( char* )malloc( l_bufferSize * sizeof( char ) );

        va_list l_arguments;

        va_start( l_arguments, _format );

        const ssize_t l_writtenCount =
            vsnprintf( l_buffer, l_bufferSize, _format, l_arguments );

        va_end( l_arguments );

        l_returnValue = !!( l_writtenCount );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Formatting buffer" );

            goto EXIT_BUFFER_APPENDING;
        }

        {
            l_bufferSize = log$level$prependToString( &l_buffer, _logLevel );

            if ( UNLIKELY( ( g_transactionSize + l_bufferSize ) >
                           LOG_MAX_TRANSACTION_SIZE_DEFAULT ) ) {
                l_bufferSize =
                    ( LOG_MAX_TRANSACTION_SIZE_DEFAULT - g_transactionSize );
            }

            __builtin_memcpy( ( g_transactionString + g_transactionSize ),
                              l_buffer, l_bufferSize );

            g_transactionSize += l_bufferSize;

            appendColorReset();

            g_transactionString[ g_transactionSize ] = '\n';

            g_transactionSize++;
        }

        if ( UNLIKELY( _logLevel == ( logLevel_t )error ) ) {
            log$transaction$commit();
        }

        l_returnValue = true;

    EXIT_BUFFER_APPENDING:
        free( l_buffer );
    }

EXIT:
    return ( l_returnValue );
}

bool log$transaction$commit( void ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !g_transactionString ) ) {
        trap( "Log was not initialized" );

        goto EXIT;
    }

    if ( g_transactionSize ) {
        // Log to file descriptor
        {
            const ssize_t l_writtenCount = write(
                g_fileDescriptor, g_transactionString, g_transactionSize );

            l_returnValue = ( l_writtenCount == g_transactionSize );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error, "Writing to file" );

                goto EXIT;
            }

#if defined( LOG_IMMEDIATE_SYNC )

            fdatasync( g_fileDescriptor );

#endif
        }

#if defined( DEBUG )
        // Log to stdout
        {
            g_transactionString[ g_transactionSize ] = '\0';

            const ssize_t l_writtenCount =
                write( STDOUT_FILENO, g_transactionString, g_transactionSize );

            l_returnValue = ( l_writtenCount == g_transactionSize );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Writing to standard output" );

                goto EXIT;
            }

#if defined( LOG_IMMEDIATE_SYNC )

            fdatasync( STDOUT_FILENO );

#endif
        }
#endif

        g_transactionSize = 0;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

#if defined( HOT_RELOAD )

struct state {
    int g_fileDescriptor;
    char* g_transactionString;
    ssize_t g_transactionSize;
    logLevel_t g_currentLogLevel;
};

EXPORT bool hotReload$unload( void** restrict _state,
                              size_t* restrict _stateSize,
                              applicationState_t* restrict _applicationState ) {
    UNUSED( _applicationState );

    *_stateSize = sizeof( struct state );
    *_state = malloc( *_stateSize );

    struct state l_state = {
        .g_fileDescriptor = g_fileDescriptor,
        .g_transactionString = g_transactionString,
        .g_transactionSize = g_transactionSize,
        .g_currentLogLevel = g_currentLogLevel,
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

        g_fileDescriptor = l_state->g_fileDescriptor;
        g_transactionString = l_state->g_transactionString;
        g_transactionSize = l_state->g_transactionSize;
        g_currentLogLevel = l_state->g_currentLogLevel;
    }

EXIT:
    return ( l_returnValue );
}

#endif
