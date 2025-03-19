#pragma once

#include <stddef.h>
#include <stdint.h>

#define MAX_ASSETS 128

typedef struct {
    uint8_t* data;
    size_t size;
} asset_t;

bool asset_t$loader$init( const char* _assetsDirectory );
bool asset_t$loader$quit( void );

asset_t asset_t$create( void );
void asset_t$destroy( asset_t* _asset );

void asset_t$load( asset_t* _asset, const char* _path );
void asset_t$unload( asset_t* _asset );
