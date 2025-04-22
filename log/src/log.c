#include "log.h"

#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// TODO: Make each thread have its own transaction string and implement mutex
// for commit

static int g_fileDescriptor = -1;
static char* g_transactionString;
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
            goto EXIT;
        }

        {
            char* l_logLevelWithBrackets =
                duplicateString( log$level$convert$toString( _logLevel ) );

            l_returnValue =
                concatBeforeAndAfterString( &l_logLevelWithBrackets, "[", "]" );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_PREPENDING;
            }

            // Colored
            l_returnValue = concatBeforeAndAfterString(
                &l_logLevelWithBrackets, log$level$convert$toColor( _logLevel ),
                COLOR_RESET );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_PREPENDING;
            }

            l_returnValue = concatBeforeAndAfterString( _string, " ", "" );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_PREPENDING;
            }

            l_returnValue = concatBeforeAndAfterString(
                _string, l_logLevelWithBrackets, "" );

            if ( UNLIKELY( !l_returnValue ) ) {
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
               const char* restrict _fileExtension ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_fileName ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_fileExtension ) ) {
        goto EXIT;
    }

    {
        // Open file descriptor
        {
            char* l_filePath = duplicateString( "." );

            l_returnValue = !!( concatBeforeAndAfterString(
                &l_filePath, _fileName, _fileExtension ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_FILE_PATH_CONCAT;
            }

            // Prepend absolute path to executable directory
            {
                char* l_directoryPath = getApplicationDirectoryAbsolutePath();

                // Construct full file path
                l_returnValue = !!( concatBeforeAndAfterString(
                    &l_filePath, l_directoryPath, "" ) );

                free( l_directoryPath );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT_FILE_PATH_CONCAT;
                }
            }

            // 0 - No special bits
            // 6 - Read & Write for owner
            // 4 - Read for group members
            // 4 - Read for others
            // NOLINTBEGIN
            g_fileDescriptor =
                open( l_filePath, O_WRONLY | O_TRUNC | O_CREAT, 0644 );
            // NOLINTEND

        EXIT_FILE_PATH_CONCAT:
            free( l_filePath );

            if ( UNLIKELY( g_fileDescriptor == -1 ) ) {
                goto EXIT;
            }
        }

        // Allocate transaction string
        {
            g_transactionString =
                ( char* )malloc( LOG_MAX_TRANSACTION_SIZE_DEFAULT );
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

// TODO: Decide on how this function works in DEBUG
bool _log$transaction$query( const logLevel_t _logLevel,
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
                   LOG_MAX_TRANSACTION_SIZE_DEFAULT ) ) {
        l_stringLength =
            ( LOG_MAX_TRANSACTION_SIZE_DEFAULT - g_transactionSize );
    }

    {
        {
            char* l_string = duplicateString( _string );

            l_stringLength = log$level$prependToString( &l_string, _logLevel );

            if ( UNLIKELY( ( g_transactionSize + l_stringLength ) >
                           LOG_MAX_TRANSACTION_SIZE_DEFAULT ) ) {
                l_stringLength =
                    ( LOG_MAX_TRANSACTION_SIZE_DEFAULT - g_transactionSize );
            }

            __builtin_memcpy( ( g_transactionString + g_transactionSize ),
                              l_string, l_stringLength );

            g_transactionSize += l_stringLength;

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

    if ( UNLIKELY( !_format ) ) {
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

        if ( UNLIKELY( !l_writtenCount ) ) {
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
        }

#if defined( DEBUG )

        log$transaction$commit();

#else

        if ( UNLIKELY( _logLevel == ( logLevel_t )error ) ) {
            log$transaction$commit();
        }

#endif

        l_returnValue = true;

    EXIT_BUFFER_APPENDING:
        free( l_buffer );
    }

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
            g_transactionString[ g_transactionSize ] = '\0';

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
