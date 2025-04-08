#include "log.h"

#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// TODO: Store debug level for each query and log it

static int g_fileDescriptor;
static char* g_transactionString;
static ssize_t g_transactionSize = 0;
static size_t g_maxTransactionSize;
static logLevel_t g_currentLogLevel = LOG_LEVEL_DEFAULT;

static logLevel_t log$level$convert$fromString( const char* restrict _string ) {
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

static size_t log$level$prependToString( char* restrict _string,
                                         const logLevel_t _logLevel ) {
    size_t l_returnValue = 0;

    {
        // TODO: Discover if _string check is reduntant by *_string
        if ( UNLIKELY( !_string ) ) {
            goto EXIT;
        }

        {
            char* l_logLevelWithBrackets =
                duplicateString( log$level$convert$toString( _logLevel ) );

            printf( "TEST1:%lu\n", __builtin_strlen( l_logLevelWithBrackets ) );
            l_returnValue =
                concatBeforeAndAfterString( &l_logLevelWithBrackets, "[", "]" );
            printf( "TEST2:%lu %lu\n", l_returnValue,
                    __builtin_strlen( l_logLevelWithBrackets ) );

            if ( !l_returnValue ) {
                goto EXIT_PREPENDING;
            }

            // Colored
            l_returnValue = concatBeforeAndAfterString(
                &l_logLevelWithBrackets,
                log$level$convert$toColor( g_currentLogLevel ), COLOR_RESET );
            printf( "TEST3:%lu %lu\n", l_returnValue,
                    __builtin_strlen( l_logLevelWithBrackets ) );

            if ( !l_returnValue ) {
                goto EXIT_PREPENDING;
            }

            l_returnValue = concatBeforeAndAfterString( &_string, " ", "" );
            printf( "TEST4:%lu %lu\n", l_returnValue,
                    __builtin_strlen( _string ) );

            if ( !l_returnValue ) {
                goto EXIT_PREPENDING;
            }

            l_returnValue = concatBeforeAndAfterString(
                &_string, l_logLevelWithBrackets, "" );
            printf( "TEST5:%lu %lu\n", l_returnValue,
                    __builtin_strlen( _string ) );

            if ( !l_returnValue ) {
                goto EXIT_PREPENDING;
            }

        EXIT_PREPENDING:
            free( l_logLevelWithBrackets );
        }
    }

EXIT:
    return ( l_returnValue );
}

bool log$init( const char* restrict _fileName,
               const char* restrict _fileExtension,
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

bool log$level$set$string( const char* restrict _string ) {
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

bool log$transaction$query( const logLevel_t _logLevel,
                            const char* restrict _string ) {
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
                                    const char* restrict _format,
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

        g_transactionSize += log$level$prependToString(
            ( g_transactionString + g_transactionSize ), _logLevel );

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
            const ssize_t l_writtenCount =
                write( STDOUT_FILENO, g_transactionString, g_transactionSize );

            l_returnValue = ( l_writtenCount == g_transactionSize );

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
