#include "log.h"

#include <mimalloc.h>
#include <stddef.h>
#include <unistd.h>

#include "stdfunc.h"

#define LOG_FILE_NAME "log"
#define LOG_FILE_EXTENSION "txt"
#define MAX_TRANSACITON_STRING_SIZE ( 1024 * 2 )

static int g_fileDescriptor = 1;
static char g_transactionString[ MAX_TRANSACITON_STRING_SIZE ];
static ssize_t g_transactionStringLength = 0;

bool log$transaction$query( const char* _string ) {
    bool l_returnValue = false;

    const ssize_t l_stringLength = __builtin_strlen( _string );

    if ( ( g_transactionStringLength + l_stringLength ) >
         MAX_TRANSACITON_STRING_SIZE ) {
        goto EXIT;
    }

    {
        __builtin_memcpy( ( g_transactionString + g_transactionStringLength ),
                          _string, l_stringLength );

        g_transactionStringLength += l_stringLength;

        l_returnValue = true;
    }

#if defined( DEBUG )

    log$transaction$commit();

#endif

EXIT:
    return ( l_returnValue );
}

bool log$transaction$commit( void ) {
    bool l_returnValue = false;

    if ( g_transactionStringLength ) {
        // Log to file descriptor
        {
            const ssize_t l_writtenCount =
                write( g_fileDescriptor, g_transactionString,
                       g_transactionStringLength );

            l_returnValue = ( l_writtenCount == g_transactionStringLength );

            if ( !l_returnValue ) {
                goto EXIT;
            }

            fdatasync( g_fileDescriptor );
        }

        // Log to stdout
#if defined( DEBUG )
        {
            const char l_logSignature[] = { 'L', 'O', 'G', ':' };

            const ssize_t l_bufferLength =
                ( arrayLengthNative( l_logSignature ) +
                  g_transactionStringLength );

            char* l_buffer =
                ( char* )mi_malloc( l_bufferLength * sizeof( char ) );

            __builtin_memcpy( l_buffer, l_logSignature,
                              arrayLengthNative( l_logSignature ) );
            __builtin_memcpy(
                ( l_buffer + arrayLengthNative( l_logSignature ) ),
                g_transactionString, g_transactionStringLength );

            const ssize_t l_writtenCount =
                write( STDOUT, l_buffer, l_bufferLength );

            mi_free( l_buffer );

            l_returnValue = ( l_writtenCount == g_transactionStringLength );

            if ( !l_returnValue ) {
                goto EXIT;
            }

            fdatasync( STDOUT );
        }
#endif
    }

EXIT:
    g_transactionStringLength = 0;

    return ( l_returnValue );
}
