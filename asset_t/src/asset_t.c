#include "asset_t.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "log.h"
#include "stdfunc.h"

// Max number of concurrent I/O operations
#define QUEUE_DEPTH 8

static char* g_assetsDirectory;

bool asset_t$loader$init( const char* restrict _assetsDirectory ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_assetsDirectory ) ) {
        goto EXIT;
    }

    {
        char* l_assetsDirectory = duplicateString( _assetsDirectory );

        {
            char* l_directoryPath = getApplicationDirectoryAbsolutePath();

            l_returnValue = !!( concatBeforeAndAfterString(
                l_assetsDirectory, l_directoryPath, "/" ) );

            if ( !l_returnValue ) {
                goto EXIT_DIRECTORY_PATH_CONCAT;
            }

        EXIT_DIRECTORY_PATH_CONCAT:
            free( l_directoryPath );
        }

        g_assetsDirectory = l_assetsDirectory;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$loader$quit( void ) {
    bool l_returnValue = false;

    {
        free( g_assetsDirectory );

        l_returnValue = true;
    }

    return ( l_returnValue );
}

asset_t asset_t$create( void ) {
    asset_t l_returnValue = DEFAULT_ASSET;

    return ( l_returnValue );
}

bool asset_t$destroy( asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$load( asset_t* restrict _asset, const char* restrict _path ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_path ) ) {
        goto EXIT;
    }

    {
        int l_fileDescriptor = -1;

        {
            char* l_path = duplicateString( _path );

            l_returnValue = !!(
                concatBeforeAndAfterString( l_path, g_assetsDirectory, NULL ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_ASSET_PATH_CONCAT;
            }

            l_fileDescriptor = open( l_path, O_RDONLY );

        EXIT_ASSET_PATH_CONCAT:
            free( l_path );
        }

        if ( UNLIKELY( l_fileDescriptor == -1 ) ) {
            log$transaction$query$format( ( logLevel_t )error,
                                          "Opening asset: '%s'\n", _path );

            goto EXIT;
        }

        {
            // Get file size
            off_t l_fileSize = lseek( l_fileDescriptor, 0, SEEK_END );

            lseek( l_fileDescriptor, 0, SEEK_SET );

            _asset->data = ( uint8_t* )malloc( l_fileSize );
            _asset->size = l_fileSize;

            const ssize_t l_readenCount =
                read( l_fileDescriptor, _asset->data, l_fileSize );

            l_returnValue = ( l_readenCount == l_fileSize );

            if ( UNLIKELY( !l_returnValue ) ) {
                free( _asset->data );

                goto FILE_EXIT;
            }
        }

    FILE_EXIT:
        close( l_fileDescriptor );
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$unload( asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
        free( _asset->data );

        _asset->size = 0;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
