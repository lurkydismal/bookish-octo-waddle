#include "asset_t.h"

#include <fcntl.h>
#include <snappy-c.h>
#include <stdlib.h>
#include <unistd.h>

#include "log.h"
#include "stdfunc.h"

#if defined( HOT_RELOAD )

#include "applicationState_t.h"

#endif

static char* g_assetsDirectory = NULL;

bool asset_t$loader$init( const char* restrict _assetsDirectory ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Alraedy initialized" );

        goto EXIT;
    }

    {
        char* l_assetsDirectory = duplicateString( _assetsDirectory );

        {
            char* l_directoryPath = getApplicationDirectoryAbsolutePath();

            concatBeforeAndAfterString( &l_assetsDirectory, l_directoryPath,
                                        "/" );

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

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Not initialized" );

        goto EXIT;
    }

    {
        free( g_assetsDirectory );

        g_assetsDirectory = NULL;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

asset_t asset_t$create( void ) {
    asset_t l_returnValue = DEFAULT_ASSET;

    return ( l_returnValue );
}

bool asset_t$destroy( asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

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

bool asset_t$load$fromPath( asset_t* restrict _asset,
                            const char* restrict _path ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_path ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        int l_fileDescriptor = -1;

        {
            char* l_path = duplicateString( _path );

            concatBeforeAndAfterString( &l_path, g_assetsDirectory, NULL );

            l_fileDescriptor = open( l_path, O_RDONLY );

            free( l_path );
        }

        l_returnValue = ( l_fileDescriptor != -1 );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format( ( logLevel_t )error,
                                          "Opening asset: '%s'", _path );

            goto FILE_EXIT;
        }

        {
            // Get file size
            off_t l_fileSize = lseek( l_fileDescriptor, 0, SEEK_END );

            l_returnValue = !!( l_fileSize );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Getting asset size" );

                goto FILE_EXIT;
            }

            lseek( l_fileDescriptor, 0, SEEK_SET );

            _asset->data = ( uint8_t* )malloc( l_fileSize );
            _asset->size = l_fileSize;

            const ssize_t l_readenCount =
                read( l_fileDescriptor, _asset->data, l_fileSize );

            l_returnValue = ( l_readenCount == l_fileSize );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error, "Reading asset" );

                asset_t$unload( _asset );

                goto FILE_EXIT;
            }
        }

    FILE_EXIT:
        if ( UNLIKELY( l_fileDescriptor == -1 ) ) {
            l_returnValue = false;

            goto EXIT;
        }

        close( l_fileDescriptor );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$load$fromGlob( asset_t* restrict _asset,
                            const char* restrict _glob ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_glob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        {
            char** l_paths = getPathsByGlob( _glob, g_assetsDirectory, false );

            l_returnValue = asset_t$load$fromPath( _asset, l_paths[ 0 ] );

            FREE_ARRAY_ELEMENTS( l_paths );
            FREE_ARRAY( l_paths );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query$format( ( logLevel_t )error,
                                              "Loading asset from glob: '%s'",
                                              _glob );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$array$load$fromGlob( asset_t*** restrict _assetArray,
                                  const char* restrict _glob ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_assetArray ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_glob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        char** l_paths = getPathsByGlob( _glob, g_assetsDirectory, false );

        FOR_ARRAY( char* const*, l_paths ) {
            asset_t l_asset = asset_t$create();

            l_returnValue = asset_t$load$fromPath( &l_asset, *_element );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query$format( ( logLevel_t )error,
                                              "Loading asset from path: '%s'",
                                              *_element );

                asset_t$destroy( &l_asset );

                goto EXIT_LOADING;
            }

            insertIntoArray( _assetArray, clone( &l_asset ) );
        }

        l_returnValue = true;

    EXIT_LOADING:
        FREE_ARRAY_ELEMENTS( l_paths );
        FREE_ARRAY( l_paths );
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$load$compressed( asset_t* restrict _asset,
                              const char* restrict _path ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_path ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = asset_t$load$fromPath( _asset, _path );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format(
                ( logLevel_t )error, "Loading asset from path: '%s'", _path );

            goto EXIT;
        }

        l_returnValue = asset_t$compress( _asset );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Compressing asset" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$load$fromGlob$compressed( asset_t* restrict _asset,
                                       const char* restrict _glob ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_glob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = asset_t$load$fromGlob( _asset, _glob );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format(
                ( logLevel_t )error, "Loading asset from glob: '%s'", _glob );

            goto EXIT;
        }

        l_returnValue = asset_t$compress( _asset );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Compressing asset" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$array$load$fromGlob$compressed( asset_t*** restrict _assetArray,
                                             const char* restrict _glob ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_assetArray ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_glob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = asset_t$array$load$fromGlob( _assetArray, _glob );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query$format( ( logLevel_t )error,
                                          "Loading asset array from glob: '%s'",
                                          _glob );

            goto EXIT;
        }

        FOR_ARRAY( asset_t* const*, *_assetArray ) {
            l_returnValue = asset_t$compress( *_element );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Compressing asset" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$unload( asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        if ( LIKELY( _asset->data ) ) {
            free( _asset->data );
            _asset->data = NULL;
        }

        _asset->size = 0;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool asset_t$compress( asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        size_t l_compressedLength =
            snappy_max_compressed_length( _asset->size );

        uint8_t* l_data =
            ( uint8_t* )malloc( l_compressedLength * sizeof( uint8_t ) );

        l_returnValue = ( snappy_compress( ( char* )( _asset->data ),
                                           _asset->size, ( char* )( l_data ),
                                           &l_compressedLength ) == SNAPPY_OK );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Compressing asset" );

            free( l_data );

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
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue =
            ( snappy_validate_compressed_buffer( ( char* )( _asset->data ),
                                                 _asset->size ) != SNAPPY_OK );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Validating compressed asset" );

            goto EXIT;
        }

        size_t l_uncompressedLength = 0;

        l_returnValue = ( snappy_uncompressed_length(
                              ( char* )( _asset->data ), _asset->size,
                              &l_uncompressedLength ) != SNAPPY_OK );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Getting uncompressed asset length" );

            goto EXIT;
        }

        uint8_t* l_data =
            ( uint8_t* )malloc( l_uncompressedLength * sizeof( uint8_t ) );

        l_returnValue =
            ( snappy_uncompress( ( char* )( _asset->data ), _asset->size,
                                 ( char* )( l_data ),
                                 &l_uncompressedLength ) != SNAPPY_OK );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Uncompressing asset" );

            free( l_data );

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

bool asset_t$save$sync$toPath( asset_t* restrict _asset,
                               const char* restrict _path,
                               const bool _needTruncate ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_path ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        int l_fileDescriptor = -1;

        {
            {
                char* l_path = duplicateString( _path );

                concatBeforeAndAfterString( &l_path, g_assetsDirectory, NULL );

                // Open file descriptor
                {
                    size_t l_openFlags = ( O_WRONLY | O_CREAT );

                    if ( _needTruncate ) {
                        l_openFlags |= O_TRUNC;
                    }

                    // 0 - No special bits
                    // 6 - Read & Write for owner
                    // 4 - Read for group members
                    // 4 - Read for others
                    l_fileDescriptor = open( l_path, l_openFlags, 0644 );
                }

                free( l_path );
            }

            l_returnValue = ( l_fileDescriptor != -1 );

            if ( !l_returnValue ) {
                log$transaction$query$format( ( logLevel_t )error,
                                              "Opening file for saving: '%s'",
                                              _path );

                goto EXIT;
            }

            const ssize_t l_writtenCount =
                write( l_fileDescriptor, _asset->data, _asset->size );

            l_returnValue = ( l_writtenCount == ( ssize_t )( _asset->size ) );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error, "Writing to file" );

                goto EXIT_SAVE;
            }
        }

        l_returnValue = true;

    EXIT_SAVE:
        close( l_fileDescriptor );
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Implement
bool asset_t$save$async$toPath( asset_t* restrict _asset,
                                const char* restrict _path,
                                const bool _needTruncate ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_path ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue =
            asset_t$save$sync$toPath( _asset, _path, _needTruncate );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Saving asset to path" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

const char* asset_t$loader$assetsDirectory$get( void ) {
    if ( UNLIKELY( !g_assetsDirectory ) ) {
        log$transaction$query( ( logLevel_t )error,
                               "Assets loader not initialized" );

        trap();

        return ( "" );
    }

    return ( g_assetsDirectory );
}

#if defined( HOT_RELOAD )

bool hotReload$unload( void** restrict _state,
                       size_t* restrict _stateSize,
                       applicationState_t* restrict _applicationState ) {
    UNUSED( _applicationState );

    *_stateSize = ( sizeof( g_assetsDirectory ) );
    *_state = malloc( *_stateSize );

    void* l_pointer = *_state;

#define APPEND_TO_STATE( _variable )                                   \
    do {                                                               \
        const size_t l_variableSize = sizeof( _variable );             \
        __builtin_memcpy( l_pointer, &( _variable ), l_variableSize ); \
        l_pointer += l_variableSize;                                   \
    } while ( 0 )

    APPEND_TO_STATE( g_assetsDirectory );

#undef APPEND_TO_STATE

    return ( true );
}

bool hotReload$load( void* restrict _state,
                     size_t _stateSize,
                     applicationState_t* restrict _applicationState ) {
    UNUSED( _applicationState );

    bool l_returnValue = false;

    {
        const size_t l_stateSize = ( sizeof( g_assetsDirectory ) );

        if ( UNLIKELY( _stateSize != l_stateSize ) ) {
            trap( "Corrupted state" );

            goto EXIT;
        }

        void* l_pointer = _state;

#define DESERIALIZE_NEXT( _variable )                       \
    do {                                                    \
        const size_t l_variableSize = sizeof( _variable );  \
        _variable = *( ( typeof( _variable )* )l_pointer ); \
        l_pointer += l_variableSize;                        \
    } while ( 0 )

        DESERIALIZE_NEXT( g_assetsDirectory );

#undef DESERIALIZE_NEXT

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

#endif
