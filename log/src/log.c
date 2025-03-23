#include "log.h"

#include <fcntl.h>
#include <libgen.h>
#include <mimalloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#include "stdfunc.h"

static int g_fileDescriptor;
static char* g_transactionString;
static ssize_t g_transactionSize = 0;
static size_t g_maxTransactionSize;
static logLevel_t g_currentLogLevel = LOG_LEVEL_DEFAULT;

static const char* log$level$convert$toString( logLevel_t _logLevel ) {
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
    if ( !_string ) {
        return ( logLevel_t )unknown;
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
        return ( ( logLevel_t )unknown );
    }
}

static const char* log$level$convert$toColor( logLevel_t _logLevel ) {
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

    if ( !_fileName ) {
        goto EXIT;
    }

    if ( !_fileExtension ) {
        goto EXIT;
    }

    if ( !_maxTransactionSize ) {
        goto EXIT;
    }

    {
        // Open file descriptor
        {
            const size_t l_fileNameLength = __builtin_strlen( _fileName );
            const size_t l_fileExtensionLength =
                __builtin_strlen( _fileExtension );
            const size_t l_filePathLength =
                ( l_fileNameLength + 1 + l_fileExtensionLength );

            {
                // +1 for file name and extension separator
                char* l_filePath = ( char* )mi_malloc(
                    ( l_filePathLength + 1 ) * sizeof( char ) );

                // Name + dot + extension
                {
                    size_t l_index = 0;

                    __builtin_memcpy( ( l_filePath + l_index ), _fileName,
                                      l_fileNameLength );
                    l_index += l_fileNameLength;

                    // Separator
                    l_filePath[ l_index ] = '.';
                    l_index += 1;

                    __builtin_memcpy( ( l_filePath + l_index ), _fileExtension,
                                      l_fileExtensionLength );
                    l_index += l_fileExtensionLength;

                    l_filePath[ l_filePathLength ] = '\0';
                }

                // Prepend absolute path to executable directory
                {
                    char* l_executablePath =
                        ( char* )mi_malloc( 250 * sizeof( char ) );

                    // Get executable path
                    {
                        ssize_t l_executablePathLength =
                            readlink( "/proc/self/exe", l_executablePath, 249 );

                        if ( l_executablePathLength == -1 ) {
                            log$transaction$query( ( logLevel_t )error,
                                                   "readlink failed\n" );

                            goto EXIT2;
                        }

                        l_executablePath[ l_executablePathLength ] = '\0';
                    }

                    char* l_directoryPath;

                    // Get directory path
                    {
                        char* l_lastSlash =
                            __builtin_strrchr( l_executablePath, '/' );

                        if ( !l_lastSlash ) {
                            log$transaction$query$format(
                                ( logLevel_t )error,
                                "Extracting directory failed: '%s'\n",
                                l_executablePath );

                            goto EXIT2;
                        }

                        const ssize_t l_lastSlashIndex =
                            ( l_lastSlash - l_executablePath );

                        l_directoryPath = l_executablePath;

                        // Do not move the beginning
                        trim( &l_directoryPath, -1, l_lastSlashIndex );

                        concatBeforeAndAfterString( &l_directoryPath, NULL,
                                                    "/" );
                    }

                    // Construct full file path
                    concatBeforeAndAfterString( &l_filePath, l_directoryPath,
                                                "" );

                    printf( "%s\n", l_filePath );

                EXIT2:
                    // TODO: Fix
                    mi_free( l_directoryPath );
                }

                g_fileDescriptor = open( l_filePath, O_WRONLY | O_CREAT, 0644 );

                mi_free( l_filePath );
            }

            if ( g_fileDescriptor == -1 ) {
                goto EXIT;
            }
        }

        // Set max transaction size
        {
            g_maxTransactionSize = _maxTransactionSize;

            if ( !g_maxTransactionSize ) {
                goto EXIT;
            }
        }

        // Allocate transaction string
        {
            g_transactionString =
                ( char* )mi_malloc( g_maxTransactionSize * sizeof( char ) );
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool log$quit( void ) {
    bool l_returnValue = false;

    {
        if ( close( g_fileDescriptor ) == -1 ) {
            goto EXIT;
        }

        mi_free( g_transactionString );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool log$level$set( logLevel_t _logLevel ) {
    bool l_returnValue = false;

    {
        g_currentLogLevel = _logLevel;

        l_returnValue = true;
    }

    return ( l_returnValue );
}

bool log$level$set$string( const char* _string ) {
    bool l_returnValue = false;

    if ( !_string ) {
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

    if ( !_string ) {
        goto EXIT;
    }

    if ( _logLevel < g_currentLogLevel ) {
        goto EXIT;
    }

    const size_t l_stringLength = __builtin_strlen( _string );

    if ( UNLIKELY( ( g_transactionSize + l_stringLength ) >
                   g_maxTransactionSize ) ) {
        goto EXIT;
    }

    {
        __builtin_memcpy( ( g_transactionString + g_transactionSize ), _string,
                          l_stringLength );

        g_transactionSize += l_stringLength;

        l_returnValue = true;
    }

#if defined( DEBUG )

    log$transaction$commit();

#endif

EXIT:
    return ( l_returnValue );
}

bool log$transaction$query$format( const logLevel_t _logLevel,
                                   const char* _format,
                                   ... ) {
    bool l_returnValue = false;

    if ( !_format ) {
        goto EXIT;
    }

    if ( _logLevel < g_currentLogLevel ) {
        goto EXIT;
    }

    {
        va_list l_arguments;

        va_start( l_arguments, _format );

        const ssize_t l_writtenCount = vsprintf(
            ( g_transactionString + g_transactionSize ), _format, l_arguments );

        va_end( l_arguments );

        if ( !l_writtenCount ) {
            goto EXIT;
        }

        g_transactionSize += l_writtenCount;

        l_returnValue = true;
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

            if ( !l_returnValue ) {
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

            // TODO: Limit scope for allocated variable
            char* l_buffer =
                ( char* )mi_malloc( l_bufferLength * sizeof( char ) );

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

            if ( !l_writtenCount ) {
                mi_free( l_buffer );

                l_returnValue = false;

                goto EXIT;
            }

            l_returnValue =
                ( ( size_t )l_writtenCount == __builtin_strlen( l_buffer ) );

            mi_free( l_buffer );

            if ( !l_returnValue ) {
                goto EXIT;
            }

            fdatasync( STDOUT_FILENO );
        }
#endif
    }

EXIT:
    g_transactionSize = 0;

    return ( l_returnValue );
}
