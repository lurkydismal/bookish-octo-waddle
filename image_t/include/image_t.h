#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "asset_t.h"

#define DEFAULT_IMAGE \
    { .id = NULL, .width = 0, .height = 0, .data = NULL, .size = 0 }

typedef struct {
    void* id;
    size_t width;
    size_t height;
    uint8_t* data;
    size_t size;
} image_t;

bool image_t$loader$init( const char* _applicationName );
bool image_t$loader$quit( void );

image_t image_t$create( void );
bool image_t$destroy( image_t* _image );

bool image_t$load$fromAsset( image_t* _image, asset_t* _asset );
bool image_t$load$fromPath( image_t* _image, const char* _path );
bool image_t$unload( image_t* _image );
