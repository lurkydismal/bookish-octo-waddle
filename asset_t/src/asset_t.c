#include "asset_t.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "log.h"
#include "stdfunc.h"

// Max number of concurrent I/O operations
#define QUEUE_DEPTH 8

static char* g_assetsDirectory;

bool asset_t$loader$init( const char* _assetsDirectory ) {
    bool l_returnValue = false;

    if ( !_assetsDirectory ) {
        goto EXIT;
    }

    {
        const size_t l_assetsDirectoryLength =
            __builtin_strlen( _assetsDirectory );

        // 1 for /
        // 1 for NUL
        char* l_assetsDirectory = ( char* )mi_malloc(
            ( l_assetsDirectoryLength + 1 + 1 ) * sizeof( char ) );

        __builtin_memcpy( l_assetsDirectory, _assetsDirectory,
                          l_assetsDirectoryLength );
        l_assetsDirectory[ l_assetsDirectoryLength ] = '/';
        l_assetsDirectory[ l_assetsDirectoryLength + 1 ] = '\0';

        g_assetsDirectory = l_assetsDirectory;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$loader$quit( void ) {
    bool l_returnValue = false;

    {
        mi_free( g_assetsDirectory );

        l_returnValue = true;
    }

    return ( l_returnValue );
}

asset_t asset_t$create( void ) {
    asset_t l_returnValue = DEFAULT_ASSET;

    return ( l_returnValue );
}

bool asset_t$destroy( asset_t* _asset ) {
    bool l_returnValue = false;

    if ( !_asset ) {
        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$load( asset_t* _asset, const char* _path ) {
    bool l_returnValue = false;

    if ( !_asset ) {
        goto EXIT;
    }

    if ( !_path ) {
        goto EXIT;
    }

    {
        const size_t l_assetsDirectoryLength =
            __builtin_strlen( g_assetsDirectory );
        const size_t l_pathLength = __builtin_strlen( _path );

        // 1 for /
        // 1 for /
        // 1 for NUL
        char* l_path = ( char* )mi_malloc(
            ( l_assetsDirectoryLength + 1 + l_pathLength + 1 + 1 ) *
            sizeof( char ) );

        {
            size_t l_index = 0;

            __builtin_memcpy( ( l_path + l_index ), g_assetsDirectory,
                              l_assetsDirectoryLength );
            l_index += l_assetsDirectoryLength;

            l_path[ l_index ] = '/';
            l_index += 1;

            __builtin_memcpy( ( l_path + l_index ), _path, l_pathLength );
            l_index += l_pathLength;

            l_path[ l_index ] = '/';
            l_index += 1;

            l_path[ l_index ] = '\0';
        }

        int l_fileDescriptor = open( l_path, O_RDONLY );

        mi_free( l_path );

        if ( l_fileDescriptor == -1 ) {
            char l_string[ 256 ];

            snprintf( l_string, sizeof( l_string ), "Opening asset: %s\n",
                      l_path );

            log$transaction$query( ( logLevel_t )error, l_string );

            goto EXIT;
        }

        {
            // Get file size
            off_t l_fileSize = lseek( l_fileDescriptor, 0, SEEK_END );
            lseek( l_fileDescriptor, 0, SEEK_SET );

            _asset->data = ( uint8_t* )mi_malloc( l_fileSize );
            _asset->size = l_fileSize;

            const ssize_t l_readenCount =
                read( l_fileDescriptor, _asset->data, l_fileSize );

            // TODO: Move mi_free to a single place
            if ( !l_readenCount ) {
                mi_free( _asset->data );

                l_returnValue = false;

                goto FILE_EXIT;
            }

            l_returnValue = ( l_readenCount == l_fileSize );

            if ( !l_returnValue ) {
                mi_free( _asset->data );

                goto FILE_EXIT;
            }
        }

    FILE_EXIT:
        close( l_fileDescriptor );
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$unload( asset_t* _asset ) {
    bool l_returnValue = false;

    if ( !_asset ) {
        goto EXIT;
    }

    {
        mi_free( _asset->data );

        _asset->size = 0;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
