#pragma once

#include <stdint.h>
#include <cglm.h>

#include "stdfloat16.h"

#define GLTF_NODE_MATRIX_BUFFER_SIZE 16

#define DEFAULT_GLTF_NODE_MATRIX \
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

#define DEFAULT_GLTF_NODE_TRANSLATION { 0, 0, 0 }

#define DEFAULT_GLTF_NODE_SCALE { 1, 1, 1 }

#define DEFAULT_GLTF_NODE_ROTATION { 0, 0, 0, 1 }

#define DEFAULT_GLTF_NODE                           \
    { .name = NULL,                                 \
      .children = NULL,                             \
      .matrix = DEFAULT_GLTF_NODE_MATRIX,           \
      .mesh = 0,                                    \
      .weights = NULL,                              \
      .translation = DEFAULT_GLTF_NODE_TRANSLATION, \
      .scale = DEFAULT_GLTF_NODE_SCALE,             \
      .rotation = DEFAULT_GLTF_NODE_ROTATION,       \
      .skin = 0,                                    \
      .camera = 0 }

struct GLTF_node {
    char* name;
    uint8_t* children;
    float matrix[ GLTF_NODE_MATRIX_BUFFER_SIZE ];
    uint8_t mesh;
    float* weights;
    vec3 translation;
    vec3 scale;
    vec4 rotation;
    uint8_t skin;
    uint8_t camera;
};
