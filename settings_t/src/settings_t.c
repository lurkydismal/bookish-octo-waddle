#include "settings_t.h"

#include "asset_t.h"

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
        window_t$destroy( &( _settings->window ) );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool settings_t$load( settings_t* _settings ) {
    bool l_returnValue = false;

    if ( !_settings ) {
        goto EXIT;
    }

    {
        settings_t l_settings = settings_t$create();

        asset_t l_settingsAsset = asset_t$create();

        // TODO: soft code file name
        asset_t$load( &l_settingsAsset, "settings.ini" );

        asset_t$unload( &l_settingsAsset );

        asset_t$destroy( &l_settingsAsset );

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
        settings_t$destroy( _settings );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
