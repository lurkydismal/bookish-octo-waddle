#include "log.h"

#include <fcntl.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "stdfunc.h"

// TODO: Store debug level for each query and log it

static pthread_mutex_t g_transactionMutex = PTHREAD_MUTEX_INITIALIZER;
static int g_fileDescriptor;
static char* g_transactionString;
static ssize_t g_transactionSize = 0;
static size_t g_maxTransactionSize;
static logLevel_t g_currentLogLevel = LOG_LEVEL_DEFAULT;

static const char* log$level$convert$toString( const logLevel_t _logLevel ) {
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

static logLevel_t log$level$convert$fromString( const char* _string ) {
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

bool log$init( const char* _fileName,
               const char* _fileExtension,
               const size_t _maxTransactionSize ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_fileName ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_fileExtension ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_maxTransactionSize ) ) {
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
                concatBeforeAndAfterString( &l_filePath, l_directoryPath, "" );

                free( l_directoryPath );
            }

            // 0 - No special bits
            // 6 - Read & Write for owner
            // 4 - Read for group members
            // 4 - Read for others
            g_fileDescriptor =
                open( l_filePath, O_WRONLY | O_TRUNC | O_CREAT, 0644 );

            free( l_filePath );

            if ( UNLIKELY( g_fileDescriptor == -1 ) ) {
                goto EXIT;
            }
        }

        // Set max transaction size
        {
            g_maxTransactionSize = _maxTransactionSize;

            if ( UNLIKELY( !g_maxTransactionSize ) ) {
                goto EXIT;
            }
        }

        // Allocate transaction string
        {
            g_transactionString =
                ( char* )malloc( g_maxTransactionSize * sizeof( char ) );
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool log$quit( void ) {
    bool l_returnValue = false;

    {
        free( g_transactionString );

        pthread_mutex_destroy( &g_transactionMutex );

        if ( UNLIKELY( close( g_fileDescriptor ) == -1 ) ) {
            goto EXIT;
        }

        l_returnValue = true;
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

bool log$level$set$string( const char* _string ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_string ) ) {
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
    return ( log$level$convert$toString( g_currentLogLevel ) );
}

bool log$transaction$query( const logLevel_t _logLevel, const char* _string ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_string ) ) {
        goto EXIT;
    }

    if ( _logLevel < g_currentLogLevel ) {
        goto EXIT;
    }

    size_t l_stringLength = __builtin_strlen( _string );

    if ( UNLIKELY( ( g_transactionSize + l_stringLength ) >
                   g_maxTransactionSize ) ) {
        l_stringLength = ( g_maxTransactionSize - g_transactionSize );

#if defined( DEBUG )

        l_stringLength--;

#endif
    }

    {
        __builtin_memcpy( ( g_transactionString + g_transactionSize ), _string,
                          l_stringLength );

        g_transactionSize += l_stringLength;

#if defined( DEBUG )

        g_transactionString[ g_transactionSize ] = '\0';

#endif

        l_returnValue = true;
    }

#if defined( DEBUG )

    log$transaction$commit();

#else

    if ( _logLevel == ( logLevel_t )error ) {
        log$transaction$commit();
    }

#endif

EXIT:
    return ( l_returnValue );
}

bool _log$transaction$query$format( const logLevel_t _logLevel,
                                    const char* _format,
                                    ... ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_format ) ) {
        goto EXIT;
    }

    if ( _logLevel < g_currentLogLevel ) {
        goto EXIT;
    }

    {
        va_list l_arguments;

        va_start( l_arguments, _format );

        const ssize_t l_writtenCount =
            vsnprintf( ( g_transactionString + g_transactionSize ),
                       ( g_maxTransactionSize - g_transactionSize ), _format,
                       l_arguments );

        va_end( l_arguments );

        if ( UNLIKELY( !l_writtenCount ) ) {
            goto EXIT;
        }

        g_transactionSize += l_writtenCount;

        l_returnValue = true;
    }

#if defined( DEBUG )

    log$transaction$commit();

#else

    if ( _logLevel == ( logLevel_t )error ) {
        log$transaction$commit();
    }

#endif

EXIT:
    return ( l_returnValue );
}

bool log$transaction$commit( void ) {
    bool l_returnValue = false;

    if ( g_transactionSize ) {
        // Log to file descriptor
        {
            const ssize_t l_writtenCount = write(
                g_fileDescriptor, g_transactionString, g_transactionSize );

            l_returnValue = ( l_writtenCount == g_transactionSize );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }

            fdatasync( g_fileDescriptor );
        }

        // Log to stdout
#if defined( DEBUG )
        {
            static const char l_logSignature[] = { 'L', 'O', 'G', ':' };

            // 1 for [
            // 1 for ]
            // 1 for space
            // 1 for new line
            const ssize_t l_bufferLength =
                ( LOG_COLOR_MAX_LENGTH + 1 + LOG_LEVEL_AS_STRING_MAX_LENGTH +
                  1 + __builtin_strlen( COLOR_RESET ) * sizeof( char ) + 1 +
                  arrayLengthNative( l_logSignature ) + g_transactionSize + 1 );

            // TODO: Reduce free() count to 1 for this variable
            char* l_buffer = ( char* )malloc( l_bufferLength * sizeof( char ) );

            {
                const char* l_logLevelColor =
                    log$level$convert$toColor( g_currentLogLevel );
                const char* l_logLevelAsString =
                    log$level$convert$toString( g_currentLogLevel );

                __builtin_snprintf( l_buffer, l_bufferLength, "%s[%s]%s %s",
                                    l_logLevelColor, l_logLevelAsString,
                                    COLOR_RESET, g_transactionString );
            }

            const ssize_t l_writtenCount =
                write( STDOUT_FILENO, l_buffer, __builtin_strlen( l_buffer ) );

            if ( UNLIKELY( !l_writtenCount ) ) {
                free( l_buffer );

                l_returnValue = false;

                goto EXIT;
            }

            l_returnValue =
                ( ( size_t )l_writtenCount == __builtin_strlen( l_buffer ) );

            free( l_buffer );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }

            fdatasync( STDOUT_FILENO );
        }
#endif
    }

    g_transactionSize = 0;

EXIT:
    return ( l_returnValue );
}
