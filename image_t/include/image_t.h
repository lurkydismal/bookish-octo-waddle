#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "asset_t.h"

#define DEFAULT_IMAGE { .width = 0, .height = 0, .data = NULL, .size = 0 }

typedef struct {
    size_t width;
    size_t height;
    uint8_t* data;
    size_t size;
} image_t;

image_t image_t$create( void );
bool image_t$destroy( image_t* restrict _image );

bool image_t$load$fromAsset( image_t* restrict _image,
                             asset_t* restrict _asset );
bool image_t$load$fromAsset$compressed( image_t* restrict _image,
                                        asset_t* restrict _asset );
bool image_t$load$fromPath( image_t* restrict _image,
                            const char* restrict _path );
bool image_t$load$fromPath$compressed( image_t* restrict _image,
                                       const char* restrict _path );
bool image_t$unload( image_t* restrict _image );

bool image_t$compress( image_t* restrict _image );
bool image_t$uncompress( image_t* restrict _image );
