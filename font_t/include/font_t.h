#pragma once

#include <stdbool.h>

#include "asset_t.h"

#define DEFAULT_FONT \
    {                \
    }

typedef struct {
} font_t;

font_t font_t$create( void );
bool font_t$destroy( font_t* _font );

bool font_t$load$fromAsset( font_t* _font, asset_t* _asset );
bool font_t$load$fromPath( font_t* _font, const char* _path );
bool font_t$unload( font_t* _font );
