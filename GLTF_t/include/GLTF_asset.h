#pragma once

#include <stdint.h>

#include "stdfloat16.h"

#define DEFAULT_GLTF_ASSET \
    { .version = 0, .generator = NULL, .copyright = NULL }

struct GLTF_asset {
    float16_t version;
    char* generator;
    char* copyright;
};
