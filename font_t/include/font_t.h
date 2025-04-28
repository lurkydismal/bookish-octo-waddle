#pragma once

#include <cglm/cglm.h>
#include <glad/gl.h>
#include <stb/stb_rect_pack.h>
#include <stb/stb_truetype.h>
#include <stdbool.h>

#include "asset_t.h"

#define FONT_ATLAS_WIDTH( _font )    \
    ( ( size_t )( ( _font ).height * \
                  CHARACTERS_COUNT( ASCII_START, ASCII_END ) ) )
#define FONT_ATLAS_HEIGHT( _font ) ( ( size_t )( ( _font ).height ) )

#define ASCII_START ( 32 )
#define ASCII_END ( 126 )
#define CHARACTERS_COUNT( _start, _end ) ( ( _end ) - ( ( _start ) - 1 ) )

#define DEFAULT_FONT_COLOR { 1, 1, 1 }

#define DEFAULT_FONT \
    { .texture = 0,  \
      .height = 0.0, \
      .ascent = 0,   \
      .descent = 0,  \
      .lineGap = 0,  \
      .color = DEFAULT_FONT_COLOR }

typedef stbtt_fontinfo fontInfo_t;
typedef stbtt_packedchar glyph_t;

typedef struct {
    GLuint texture;
    float height;
    size_t ascent;
    size_t atlasHeight;
    size_t atlasWidth;
    size_t descent;
    size_t lineGap;
    fontInfo_t info;
    glyph_t glyphs[ CHARACTERS_COUNT( ASCII_START, ASCII_END ) ];
    vec3 color;
} font_t;

font_t font_t$create( void );
bool font_t$destroy( font_t* restrict _font );

bool font_t$load$fromAsset( font_t* restrict _font, asset_t* restrict _asset );
bool font_t$load$fromPath( font_t* restrict _font, const char* restrict _path );
bool font_t$unload( font_t* restrict _font );
