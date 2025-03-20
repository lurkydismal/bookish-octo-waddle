#include "asset_t.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "log.h"
#include "stdfunc.h"

// Max number of concurrent I/O operations
#define QUEUE_DEPTH 8

static const char* g_assetsDirectory;

bool asset_t$loader$init( const char* _assetsDirectory ) {
    bool l_returnValue = false;

    if ( !_assetsDirectory ) {
        goto EXIT;
    }

    {
        g_assetsDirectory = _assetsDirectory;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$loader$quit( void ) {
    bool l_returnValue = false;

    {
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
        int l_fileDescriptor = open( _path, O_RDONLY );

        if ( l_fileDescriptor == -1 ) {
            char l_string[ 256 ];

            snprintf( l_string, sizeof( l_string ), "Opening asset: %s\n",
                      _path );

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
