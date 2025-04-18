#pragma once

#include <stdint.h>

#define DEFAULT_GLTF_TEXTURE_INFO { .index = 0, .texCoord = 0 }

struct GLTF_texture_info {
    uint8_t index;
    uint32_t texCoord;
};
