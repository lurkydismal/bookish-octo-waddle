#pragma once

#include <stdbool.h>
#include <stdint.h>

#define DEFAULT_IMAGE \
    {                 \
    }

typedef struct {
} image_t;

bool image_t$loader$init( const char* _applicationName );
bool image_t$loader$quit( void );

image_t image_t$create( void );
bool image_t$destroy( void );

bool image_t$load( image_t* _image, uint8_t* _dataArray );
bool image_t$unload( image_t* _image );
