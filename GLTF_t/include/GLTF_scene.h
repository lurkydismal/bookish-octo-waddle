#pragma once

#include <stdint.h>

#define DEFAULT_GLTF_SCENE { .name = NULL, .nodes = NULL }

struct GLTF_scene {
    char* name;
    uint16_t* nodes;
};
