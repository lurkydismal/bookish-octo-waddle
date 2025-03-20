#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DEFAULT_ASSET { NULL, 0 }

typedef struct {
    uint8_t* data;
    size_t size;
} asset_t;

bool asset_t$loader$init( const char* _assetsDirectory );
bool asset_t$loader$quit( void );

asset_t asset_t$create( void );
bool asset_t$destroy( asset_t* _asset );

bool asset_t$load( asset_t* _asset, const char* _path );
bool asset_t$unload( asset_t* _asset );
