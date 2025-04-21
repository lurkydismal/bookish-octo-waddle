#include "asset_t.h"

#include <fcntl.h>
#include <snappy-c.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "log.h"
#include "stdfunc.h"

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
                &l_assetsDirectory, l_directoryPath, "/" ) );

            if ( UNLIKELY( !l_returnValue ) ) {
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
        _asset->data = NULL;
        _asset->size = 0;

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

            l_returnValue = !!( concatBeforeAndAfterString(
                &l_path, g_assetsDirectory, NULL ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_ASSET_PATH_CONCAT;
            }

            l_fileDescriptor = open( l_path, O_RDONLY );

        EXIT_ASSET_PATH_CONCAT:
            free( l_path );
        }

        if ( UNLIKELY( l_fileDescriptor == -1 ) ) {
            l_returnValue = false;

            log$transaction$query$format( ( logLevel_t )error,
                                          "Opening asset: '%s'\n", _path );

            goto EXIT;
        }

        {
            // Get file size
            off_t l_fileSize = lseek( l_fileDescriptor, 0, SEEK_END );

            if ( UNLIKELY( !l_fileSize ) ) {
                l_returnValue = false;

                goto FILE_EXIT;
            }

            lseek( l_fileDescriptor, 0, SEEK_SET );

            _asset->data = ( uint8_t* )malloc( l_fileSize );
            _asset->size = l_fileSize;

            const ssize_t l_readenCount =
                read( l_fileDescriptor, _asset->data, l_fileSize );

            l_returnValue = ( l_readenCount == l_fileSize );

            if ( UNLIKELY( !l_returnValue ) ) {
                asset_t$unload( _asset );

                goto FILE_EXIT;
            }
        }

    FILE_EXIT:
        close( l_fileDescriptor );
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$load$compressed( asset_t* restrict _asset,
                              const char* restrict _path ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
        if ( UNLIKELY( !asset_t$load( _asset, _path ) ) ) {
            l_returnValue = false;

            goto EXIT;
        }

        if ( UNLIKELY( !asset_t$compress( _asset ) ) ) {
            l_returnValue = false;

            goto EXIT;
        }

        l_returnValue = true;
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

bool asset_t$compress( asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
        size_t l_compressedLength =
            snappy_max_compressed_length( _asset->size );

        uint8_t* l_data =
            ( uint8_t* )malloc( l_compressedLength * sizeof( uint8_t ) );

        if ( UNLIKELY( snappy_compress( ( char* )( _asset->data ), _asset->size,
                                        ( char* )( l_data ),
                                        &l_compressedLength ) != SNAPPY_OK ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Compression failed\n" );

            free( l_data );

            l_returnValue = false;

            goto EXIT;
        }

        l_data = ( uint8_t* )realloc(
            l_data, ( l_compressedLength * sizeof( uint8_t ) ) );

        free( _asset->data );

        _asset->data = l_data;
        _asset->size = ( l_compressedLength * sizeof( uint8_t ) );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$uncompress( asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
        if ( UNLIKELY( snappy_validate_compressed_buffer(
                           ( char* )( _asset->data ), _asset->size ) !=
                       SNAPPY_OK ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Decompression failed: data\n" );

            l_returnValue = false;

            goto EXIT;
        }

        size_t l_uncompressedLength = 0;

        if ( UNLIKELY( snappy_uncompressed_length(
                           ( char* )( _asset->data ), _asset->size,
                           &l_uncompressedLength ) != SNAPPY_OK ) ) {
            log$transaction$query(
                ( logLevel_t )error,
                "Decompression failed: uncompressed length\n" );

            l_returnValue = false;

            goto EXIT;
        }

        uint8_t* l_data =
            ( uint8_t* )malloc( l_uncompressedLength * sizeof( uint8_t ) );

        if ( UNLIKELY( snappy_uncompress( ( char* )( _asset->data ),
                                          _asset->size, ( char* )( l_data ),
                                          &l_uncompressedLength ) !=
                       SNAPPY_OK ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Decompression failed: uncompress\n" );

            free( l_data );

            l_returnValue = false;

            goto EXIT;
        }

        l_data = ( uint8_t* )realloc(
            l_data, ( l_uncompressedLength * sizeof( uint8_t ) ) );

        free( _asset->data );

        _asset->data = l_data;
        _asset->size = ( l_uncompressedLength * sizeof( uint8_t ) );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
