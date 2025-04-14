#pragma once

#include <cglm/cglm.h>
#include <glad/gl.h>
#include <stb/stb_truetype.h>
#include <stdbool.h>

#include "asset_t.h"

#define FONT_ATLAS_W 512
#define FONT_ATLAS_H 512

#define ASCII_START ( 32 )
#define ASCII_END ( 126 )
#define CHARACTERS_COUNT ( ASCII_END - ( ASCII_START - 1 ) )

#define DEFAULT_FONT { .texture = 0, .height = 0.0 }

typedef struct {
    GLuint texture;
    stbtt_bakedchar glyphs[ CHARACTERS_COUNT ];
    stbtt_fontinfo info;
    float height;
    vec3 color;
} font_t;

font_t font_t$create( void );
bool font_t$destroy( font_t* restrict _font );

bool font_t$load$fromAsset( font_t* restrict _font, asset_t* restrict _asset );
bool font_t$load$fromPath( font_t* restrict _font, const char* restrict _path );
bool font_t$unload( font_t* restrict _font );
