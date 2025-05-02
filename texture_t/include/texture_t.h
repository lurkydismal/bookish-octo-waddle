#pragma once

#include <glad/gl.h>
#include <ktx.h>
#include <stdbool.h>

#include "asset_t.h"

#define DEFAULT_TEXTURE { .texture = 0, .type = 0 .data = NULL }

// TOOD: Offset list for each mipmap
typedef struct {
    GLuint texture;
    GLenum type;
    ktxTexture2* data;
} texture_t;

texture_t texture_t$create( void );
bool texture_t$destroy( texture_t* _texture );

bool texture_t$load$fromAsset( texture_t* _texture, asset_t* _asset );
bool texture_t$unload( texture_t* _texture );
