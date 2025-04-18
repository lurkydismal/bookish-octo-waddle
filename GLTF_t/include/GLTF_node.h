#pragma once

#include <stdint.h>

#include "stdfloat16.h"

#define DEFAULT_GLTF_NODE_MATRIX \
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

#define DEFAULT_GLTF_NODE_TRANSLATION { 0, 0, 0 }

#define DEFAULT_GLTF_NODE_SCALE { 0, 0, 0 }

#define DEFAULT_GLTF_NODE_ROTATION { 0, 0, 0, 0 }

#define DEFAULT_GLTF_NODE                           \
    { .name = NULL,                                 \
      .children = NULL,                             \
      .matrix = DEFAULT_GLTF_NODE_MATRIX.mesh = 0,  \
      .weights = NULL,                              \
      .translation = DEFAULT_GLTF_NODE_TRANSLATION, \
      .scale = DEFAULT_GLTF_NODE_SCALE,             \
      .rotation = DEFAULT_GLTF_NODE_ROTATION,       \
      .skin = 0,                                    \
      .camera = 0 }

struct GLTF_node {
    char* name;
    uint8_t* children;
    float matrix[ 16 ];
    uint8_t mesh;
    float16_t* weights;
    float16_t translation[ 3 ];
    float16_t scale[ 3 ];
    float16_t rotation[ 4 ];
    uint8_t skin;
    uint8_t camera;
};
