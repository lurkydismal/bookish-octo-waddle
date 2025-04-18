#pragma once

#include <stdint.h>

#define DEFAULT_GLTF_TEXTURE { .name = NULL, .sampler = 0, .source = 0 }

struct GLTF_texture {
    char* name;
    uint8_t sampler;
    uint8_t source;
};
