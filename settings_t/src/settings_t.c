#include "settings_t.h"

#include "asset_t.h"
#include "log.h"
#include "stdfunc.h"

settings_t settings_t$create( void ) {
    settings_t l_returnValue = DEFAULT_SETTINGS;

    return ( l_returnValue );
}

bool settings_t$destroy( settings_t* _settings ) {
    bool l_returnValue = false;

    if ( !_settings ) {
        goto EXIT;
    }

    {
        l_returnValue = window_t$destroy( &( _settings->window ) );

        if ( !l_returnValue ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool settings_t$load( settings_t* _settings,
                      const char* _fileName,
                      const char* _fileExtension ) {
    bool l_returnValue = false;

    if ( !_settings ) {
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

                concatBeforeAndAfterString( &l_filePath, _fileName,
                                            _fileExtension );

                l_returnValue = asset_t$load( &l_settingsAsset, l_filePath );

                mi_free( l_filePath );
            }

            if ( !l_returnValue ) {
                goto EXIT;
            }

            log$transaction$query$format(
                ( logLevel_t )debug, "Settings:\nData: {%s}\nSize: %lu\n",
                l_settingsAsset.data, l_settingsAsset.size );

            // TODO: Parse settings
            {
            }

            l_returnValue = asset_t$unload( &l_settingsAsset );

            if ( !l_returnValue ) {
                goto EXIT;
            }

            l_returnValue = asset_t$destroy( &l_settingsAsset );

            if ( !l_returnValue ) {
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

    if ( !_settings ) {
        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
