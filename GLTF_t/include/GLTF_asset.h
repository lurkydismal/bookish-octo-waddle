#pragma once

#include <stdint.h>

#define DEFAULT_GLTF_ASSET \
    { .version = 0, .generator = NULL, .copyright = NULL }

struct GLTF_asset {
    uint8_t version;
    char* generator;
    char* copyright;
};
