#if defined( __SANITIZE_LEAK__ )

#include <sanitizer/lsan_interface.h>

#endif

#if defined( HOT_RELOAD )

#include <dlfcn.h>
#include <link.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/stat.h>

#include "plthook.h"

#endif

#include "applicationState_t.h"
#include "event.h"
#include "init.h"
#include "iterate.h"
#include "quit.h"
#include "stdfunc.h"

applicationState_t g_applicationState;

#if defined( HOT_RELOAD )

#define HOT_RELOAD_ROOT_SHARED_OBJECT_FILE_NAME "root"
#define HOT_RELOAD_CHECK_DELAY_FRAMES 2

#define HOT_RELOAD_UNLOAD_FUNCTION_SIGNATURE "hotReload$unload"
#define HOT_RELOAD_LOAD_FUNCTION_SIGNATURE "hotReload$load"

typedef bool ( *hotReload$unload_t )(
    void** restrict _state,
    size_t* restrict _stateSize,
    applicationState_t* restrict _applicationState );
typedef bool ( *hotReload$load_t )(
    void* restrict _state,
    size_t _stateSize,
    applicationState_t* restrict _applicationState );

struct state {
    void* data;
    size_t size;
};

char* g_rootSharedObjectPath = NULL;
sigjmp_buf g_jumpBuffer;

static void crashHandler( int _signal ) {
    UNUSED( _signal );

    siglongjmp( g_jumpBuffer, 1 );
}

static FORCE_INLINE bool hasPathChanged( const char* restrict _path ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_path ) ) {
        goto EXIT;
    }

    {
        static long l_lastModificationTime = 0;

        struct stat l_statisticss;

        assert( ( stat( _path, &l_statisticss ) == 0 ), "stat failed" );

        if ( l_statisticss.st_mtime <= l_lastModificationTime ) {
            goto EXIT;
        }

        l_lastModificationTime = l_statisticss.st_mtime;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE Lmid_t getNamespaceId( void* restrict _handle ) {
    Lmid_t l_returnValue = LONG_MAX;

    if ( UNLIKELY( !_handle ) ) {
        goto EXIT;
    }

    {
        const int l_result = dlinfo( _handle, RTLD_DI_LMID, &l_returnValue );

        assert( l_result >= 0 );
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool collectStatesFromHandle(
    void* restrict _handle,
    char** restrict* restrict _stateNames,
    struct state** restrict* restrict _states ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_handle ) ) {
        goto EXIT;
    }

    {
        const Lmid_t l_namespaceId = getNamespaceId( _handle );

        assert( l_namespaceId != LONG_MAX );

        struct link_map* l_linkMap;

        const int l_result = dlinfo( _handle, RTLD_DI_LINKMAP, &l_linkMap );

        assert( l_result >= 0 );

        // Call each module state saving function
        for ( struct link_map* _linkMap = l_linkMap; _linkMap;
              _linkMap = _linkMap->l_next ) {
            if ( UNLIKELY( !( _linkMap->l_name ) ||
                           !( *( _linkMap->l_name ) ) ) ) {
                continue;
            }

            if ( UNLIKELY( __builtin_strcmp( _linkMap->l_name,
                                             g_rootSharedObjectPath ) == 0 ) ) {
                continue;
            }

            if ( !comparePathsDirectories( _linkMap->l_name,
                                           g_rootSharedObjectPath ) ) {
                continue;
            }

            void* l_handle = dlmopen( l_namespaceId, _linkMap->l_name,
                                      ( RTLD_LAZY | RTLD_NOLOAD ) );

            assert( l_handle != NULL );

            hotReload$unload_t l_unloadCallback =
                dlsym( l_handle, HOT_RELOAD_UNLOAD_FUNCTION_SIGNATURE );

            if ( l_unloadCallback ) {
                struct state l_state;

                l_returnValue = l_unloadCallback(
                    &( l_state.data ), &( l_state.size ), &g_applicationState );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT;
                }

                insertIntoArray( _stateNames,
                                 duplicateString( _linkMap->l_name ) );
                insertIntoArray( _states, clone( &l_state ) );
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Implement
static FORCE_INLINE char** getUndefinedFunctionsFromSoPath(
    const char* restrict _soPath ) {
    char** l_returnValue = NULL;

    if ( UNLIKELY( !_soPath ) ) {
        goto EXIT;
    }

    {
        l_returnValue = createArray( char* );

        const char* names[] = {
            "__extendhfsf2",
            "__truncsfhf2",
            "snappy_max_compressed_length",
            "snappy_compress",
            "snappy_validate_compressed_buffer",
            "snappy_uncompressed_length",
            "snappy_uncompress",
            "ini_parse_string_length",
            "SDL_SetRenderVSync",
            "SDL_GetError",
            "SDL_GetRenderDrawColor",
            "SDL_SetRenderDrawColor",
            "SDL_RenderRect",
            "SDL_RenderFillRect",
            "SDL_GetError",
            "SDL_IOFromConstMem",
            "IMG_LoadTexture_IO",
            "SDL_CloseIO",
            "SDL_DestroyTexture",
            "SDL_RenderTexture",
            "SDL_RenderTextureRotated",
            "SDL_GetError",
            "SDL_GetKeyboardState",
            "SDL_SetRenderScale",
            "SDL_GetError",
            "SDL_SetAppMetadata",
            "SDL_Init",
            "SDL_CreateWindowAndRenderer",
            "SDL_SetDefaultTextureScaleMode",
            "SDL_HasGamepad",
            "SDL_RenderClear",
            "SDL_RenderPresent",
            "SDL_DestroyRenderer",
            "SDL_DestroyWindow",
            "SDL_Quit",
        };

        FOR( const char* const*, names ) {
            insertIntoArray( &l_returnValue, duplicateString( *_element ) );
        }
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Implement
static FORCE_INLINE char** getMainExecutableFunctionNamesToPatch( void ) {
    char** l_returnValue = NULL;

    {
        l_returnValue = createArray( char* );

        const char* names[] = {
            // Main loop
            "init",
            "event",
            "iterate",
            "quit",

            // Other
            "concatBeforeAndAfterString",
        };

        FOR( const char* const*, names ) {
            insertIntoArray( &l_returnValue, duplicateString( *_element ) );
        }
    }

    return ( l_returnValue );
}

static bool hotReloadSo( const char* restrict _soPath ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_soPath ) ) {
        goto EXIT;
    }

    {
        if ( hasPathChanged( _soPath ) ) {
            static void* l_managerHandle = NULL;

            // If first hot reload
            if ( UNLIKELY( !l_managerHandle ) ) {
                l_managerHandle = dlopen( NULL, RTLD_NOW );

                assert( l_managerHandle != NULL );
            }

            {
                char** l_stateNames = createArray( char* );
                struct state** l_states = createArray( struct state* );

                l_returnValue = collectStatesFromHandle(
                    l_managerHandle, &l_stateNames, &l_states );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT2;
                }

                static void* l_previouManagerHandle = NULL;

                // Can not be NULL
                l_previouManagerHandle = l_managerHandle;

                // Load new
                l_managerHandle = dlmopen( LM_ID_NEWLM, _soPath,
                                           ( RTLD_LAZY | RTLD_DEEPBIND ) );

                if ( sigsetjmp( g_jumpBuffer, 0 ) != 0 ) {
                    l_managerHandle = l_previouManagerHandle;
                }

                assert( l_managerHandle != NULL );

                char** l_functionNames =
                    getMainExecutableFunctionNamesToPatch();

                assert( l_functionNames != NULL );

                void** l_functionAddresses = createArray( void* );

                preallocateArray( &l_functionAddresses,
                                  arrayLength( l_functionNames ) );

                __builtin_memset(
                    l_functionAddresses, 0,
                    ( arrayLength( l_functionAddresses ) * sizeof( void* ) ) );

                {
                    char** l_undefinedFunctions =
                        getUndefinedFunctionsFromSoPath( _soPath );

                    assert( l_undefinedFunctions != NULL );

                    const Lmid_t l_namespaceId =
                        getNamespaceId( l_managerHandle );

                    assert( l_namespaceId != LONG_MAX );

                    struct link_map* l_linkMap;
                    const int l_result =
                        dlinfo( l_managerHandle, RTLD_DI_LINKMAP, &l_linkMap );

                    assert( l_result >= 0 );

                    for ( struct link_map* _linkMap = l_linkMap; _linkMap;
                          _linkMap = _linkMap->l_next ) {
                        if ( UNLIKELY( !( _linkMap->l_name ) ||
                                       !( *( _linkMap->l_name ) ) ) ) {
                            continue;
                        }

                        if ( UNLIKELY( __builtin_strcmp(
                                           _linkMap->l_name,
                                           g_rootSharedObjectPath ) == 0 ) ) {
                            continue;
                        }

                        if ( !comparePathsDirectories(
                                 _linkMap->l_name, g_rootSharedObjectPath ) ) {
                            continue;
                        }

                        void* l_handle =
                            dlmopen( l_namespaceId, _linkMap->l_name,
                                     ( RTLD_LAZY | RTLD_NOLOAD ) );

                        assert( l_handle != NULL );

                        {
                            hotReload$load_t l_loadCallback = dlsym(
                                l_handle, HOT_RELOAD_LOAD_FUNCTION_SIGNATURE );

                            if ( l_loadCallback ) {
                                FOR_RANGE( arrayLength_t, 0,
                                           arrayLength( l_stateNames ) ) {
                                    const char* l_name = l_stateNames[ _index ];

                                    if ( __builtin_strcmp(
                                             l_name, _linkMap->l_name ) == 0 ) {
                                        struct state* l_state =
                                            l_states[ _index ];

                                        void* l_stateData = l_state->data;
                                        size_t l_stateDataSize = l_state->size;

                                        l_returnValue = l_loadCallback(
                                            l_stateData, l_stateDataSize,
                                            &g_applicationState );

                                        if ( UNLIKELY( !l_returnValue ) ) {
                                            continue;
                                        }
                                    }
                                }
                            }
                        }

                        // Patch undefined functions with base namespace
                        {
                            plthook_t* l_plthookHandle;

                            const int l_result = plthook_open_by_handle(
                                &l_plthookHandle, l_handle );

                            if ( l_result != 0 ) {
                                continue;
                            }

                            FOR_ARRAY( char* const*, l_undefinedFunctions ) {
                                plthook_replace(
                                    l_plthookHandle, *_element,
                                    dlsym( RTLD_DEFAULT, *_element ), NULL );
                            }

                            plthook_close( l_plthookHandle );
                        }

                        // Locate functions used in main executable for patching
                        {
                            FOR_RANGE( arrayLength_t, 0,
                                       arrayLength( l_functionNames ) ) {
                                const char* l_name = l_functionNames[ _index ];

                                void* l_address = dlsym( l_handle, l_name );

                                if ( l_address ) {
                                    l_functionAddresses[ _index ] = l_address;
                                }
                            }
                        }
                    }

                    FREE_ARRAY_ELEMENTS( l_undefinedFunctions );
                    FREE_ARRAY( l_undefinedFunctions );
                }

                // Patch functions in main executable
                {
                    void* l_baseHandle = dlopen( NULL, RTLD_NOW );

                    plthook_t* l_plthookHandle;

                    const int l_result = plthook_open_by_handle(
                        &l_plthookHandle, l_baseHandle );

                    assert( l_result == 0 );

                    FOR_RANGE( arrayLength_t, 0,
                               arrayLength( l_functionNames ) ) {
                        const char* l_name = l_functionNames[ _index ];
                        void* l_address = l_functionAddresses[ _index ];

                        assert( l_address,
                                "Function for main executable was not found" );

                        const int l_result = plthook_replace(
                            l_plthookHandle, l_name, l_address, NULL );

                        assert( l_result == 0 );
                    }

                    plthook_close( l_plthookHandle );
                }

                FREE_ARRAY_ELEMENTS( l_functionNames );
                FREE_ARRAY( l_functionNames );
                FREE_ARRAY( l_functionAddresses );

            EXIT2:
                FREE_ARRAY_ELEMENTS( l_stateNames );
                FREE_ARRAY( l_stateNames );
                FREE_ARRAY_ELEMENTS( l_states );
                FREE_ARRAY( l_states );
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

#endif

int main( int _argumentCount, char** _argumentVector ) {
    bool l_returnValue = false;

#if defined( HOT_RELOAD )

    g_rootSharedObjectPath =
        duplicateString( HOT_RELOAD_ROOT_SHARED_OBJECT_FILE_NAME ".so" );

    {
        char* l_directoryPath = getApplicationDirectoryAbsolutePath();

        concatBeforeAndAfterString( &g_rootSharedObjectPath, l_directoryPath,
                                    NULL );

        free( l_directoryPath );
    }

    signal( SIGSEGV, crashHandler );
    signal( SIGILL, crashHandler );
    signal( SIGBUS, crashHandler );

    l_returnValue = hotReloadSo( g_rootSharedObjectPath );

    if ( UNLIKELY( !l_returnValue ) ) {
        goto EXIT;
    }

#endif

    l_returnValue =
        init( &g_applicationState, _argumentCount, _argumentVector );

    if ( UNLIKELY( !l_returnValue ) ) {
        goto EXIT;
    }

    {
#if defined( HOT_RELOAD )

        size_t l_iterationCount = 0;

#endif

        while ( true ) {
            SDL_PumpEvents();

            event_t l_event;

            while ( SDL_PollEvent( &l_event ) ) {
                l_returnValue = event( &g_applicationState, &l_event );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT;
                }
            }

            // TODO: Improve
            l_returnValue = event( &g_applicationState, NULL );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }

            l_returnValue = iterate( &g_applicationState );

            if ( UNLIKELY( !l_returnValue ) ) {
                break;
            }

#if defined( HOT_RELOAD )

            l_iterationCount++;

            if ( ( l_iterationCount % HOT_RELOAD_CHECK_DELAY_FRAMES ) == 0 ) {
                l_returnValue = hotReloadSo( g_rootSharedObjectPath );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT;
                }
            }
#endif
        }
    }

EXIT:
    quit( &g_applicationState, l_returnValue );

#if defined( HOT_RELOAD )

    free( g_rootSharedObjectPath );

#endif

#if defined( __SANITIZE_LEAK__ )

    __lsan_do_leak_check();

#endif

    return ( !l_returnValue );
}
