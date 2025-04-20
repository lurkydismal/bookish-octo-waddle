#include "settings_t.h"

#include <stdlib.h>

#include "asset_t.h"
#include "log.h"
#include "stdfunc.h"

settings_t settings_t$create( void ) {
    settings_t l_returnValue = DEFAULT_SETTINGS;

    {
        l_returnValue.window = window_t$create();
    }

    return ( l_returnValue );
}

bool settings_t$destroy( settings_t* _settings ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_settings ) ) {
        goto EXIT;
    }

    {
        l_returnValue = window_t$destroy( &( _settings->window ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool settings_t$load( settings_t* restrict _settings,
                      const char* restrict _fileName,
                      const char* restrict _fileExtension ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_settings ) ) {
        goto EXIT;
    }

    {
        settings_t l_settings = settings_t$create();

        // Parse settings file
        {
            asset_t l_settingsAsset = asset_t$create();

            // Load settings file
            {
                char* l_filePath = duplicateString( "." );

                l_returnValue = !!( concatBeforeAndAfterString(
                    &l_filePath, _fileName, _fileExtension ) );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT_FILE_PATH_CONCAT;
                }

                l_returnValue = asset_t$load( &l_settingsAsset, l_filePath );

                if ( UNLIKELY( !l_returnValue ) ) {
                    goto EXIT_FILE_PATH_CONCAT;
                }

            EXIT_FILE_PATH_CONCAT:
                free( l_filePath );
            }

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }

            log$transaction$query$format(
                ( logLevel_t )debug, "Settings:\nData: {%s}\nSize: %lu\n",
                l_settingsAsset.data, l_settingsAsset.size );

            {
                char** l_lines = splitStringIntoArrayBySymbol(
                    ( char* )( l_settingsAsset.data ), '\n' );

                FOR_ARRAY( char* const*, l_lines ) {
                    const char* l_line = sanitizeString( *_element );

                    if ( l_line ) {
                        log$transaction$query$format( ( logLevel_t )info,
                                                      "%s\n", l_line );

                        char** l_keyAndValue =
                            splitStringIntoArrayBySymbol( l_line, '=' );

                        if ( arrayLength( l_keyAndValue ) != 2 ) {
                            log$transaction$query$format(
                                ( logLevel_t )error, "Settings line '%s'\n",
                                l_line );

                            goto LOOP_CONTINUE;
                        }

                        const char* l_key = l_keyAndValue[ 1 ];
                        const char* l_value = l_keyAndValue[ 2 ];

                        // TODO: Implement something like settingsOption_t
                        if ( __builtin_strcmp( l_key, "window_name" ) == 0 ) {
                            free( l_settings.window.name );

                            l_settings.window.name = duplicateString( l_value );

                        } else if ( __builtin_strcmp( l_key, "window_width" ) ==
                                    0 ) {
                            l_settings.window.width = atoi( l_value );

                        } else if ( __builtin_strcmp( l_key,
                                                      "window_height" ) == 0 ) {
                            l_settings.window.height = atoi( l_value );

                        } else if ( __builtin_strcmp(
                                        l_key, "window_desired_FPS" ) == 0 ) {
                            l_settings.window.desiredFPS = atoi( l_value );

                        } else if ( __builtin_strcmp( l_key, "window_vsync" ) ==
                                    0 ) {
                            l_settings.window.vsync =
                                vsync_t$fromString( l_value );

                        } else if ( __builtin_strcmp(
                                        l_key, "limited_loop_desired_FPS" ) ==
                                    0 ) {
                            l_settings.limitedLoopDesiredFPS = atoi( l_value );
                        }

                    LOOP_CONTINUE:
                        FREE_ARRAY_ELEMENTS( l_keyAndValue );
                        FREE_ARRAY( l_keyAndValue );
                    }
                }

                FREE_ARRAY_ELEMENTS( l_lines );
                FREE_ARRAY( l_lines );
            }

            l_returnValue = asset_t$unload( &l_settingsAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }

            l_returnValue = asset_t$destroy( &l_settingsAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
        }

        *_settings = l_settings;

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool settings_t$unload( settings_t* _settings ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_settings ) ) {
        goto EXIT;
    }

    {
        if ( UNLIKELY( !window_t$destroy( &( _settings->window ) ) ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
