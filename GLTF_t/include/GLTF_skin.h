#pragma once

#include <stdint.h>

#define DEFAULT_GLTF_SKIN \
    { .name = NULL, .inverseBindMatrices = 0, .skeleton = 0, .joints = NULL }

struct GLTF_skin {
    char* name;
    uint8_t inverseBindMatrices;
    uint16_t skeleton;
    uint16_t* joints;
};
