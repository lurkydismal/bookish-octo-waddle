#pragma once

#include <stdint.h>

#include "stdfloat16.h"

#define DEFAULT_GLTF_MESH_PRIMITIVES_ATTRIBUTES_POSITION { 0, 0, 0 }

#define DEFAULT_GLTF_MESH_PRIMITIVES_ATTRIBUTES_NORMAL { 0, 0, 0 }

#define DEFAULT_GLTF_MESH_PRIMITIVES_ATTRIBUTES_TANGENT { 0, 0, 0, 0 }

#define DEFAULT_GLTF_MESH_PRIMITIVES_ATTRIBUTES_TEXCOORD { 0, 0 }

#define DEFAULT_GLTF_MESH_PRIMITIVES_ATTRIBUTES_COLOR { 0, 0, 0, 1 }

#define DEFAULT_GLTF_MESH_PRIMITIVES_ATTRIBUTES_JOINT { 0, 0, 0, 0 }

#define DEFAULT_GLTF_MESH_PRIMITIVES_ATTRIBUTES_WEIGHT { 0, 0, 0, 0 }

#define DEFAULT_GLTF_MESH_PRIMITIVES_ATTRIBUTES                     \
    { .POSITION = DEFAULT_GLTF_MESH_PRIMITIVES_ATTRIBUTES_POSITION, \
      .NORMAL = DEFAULT_GLTF_MESH_PRIMITIVES_ATTRIBUTES_NORMAL,     \
      .TANGENT = DEFAULT_GLTF_MESH_PRIMITIVES_ATTRIBUTES_TANGENT,   \
      .TEXCOORD_n = NULL,                                           \
      .COLOR_n = NULL,                                              \
      .JOINTS_n = NULL,                                             \
      .WEIGHTS_n = NULL }

#define DEFAULT_GLTF_MESH_PRIMITIVES_MODE POINTS

#define DEFAULT_GLTF_MESH_PRIMITIVES_TARGETS_POSITION { 0, 0, 0 }

#define DEFAULT_GLTF_MESH_PRIMITIVES_TARGETS_NORMAL { 0, 0, 0 }

#define DEFAULT_GLTF_MESH_PRIMITIVES_TARGETS_TANGENT { 0, 0, 0, 0 }

#define DEFAULT_GLTF_MESH_PRIMITIVES_TARGETS                     \
    { .POSITION = DEFAULT_GLTF_MESH_PRIMITIVES_TARGETS_POSITION, \
      .NORMAL = DEFAULT_GLTF_MESH_PRIMITIVES_TARGETS_NORMAL,     \
      .TANGENT = DEFAULT_GLTF_MESH_PRIMITIVES_TARGETS_TANGENT }

#define DEFAULT_GLTF_MESH_PRIMITIVES                         \
    { .attributes = DEFAULT_GLTF_MESH_PRIMITIVES_ATTRIBUTES, \
      .indices = 0,                                          \
      .mode = DEFAULT_GLTF_MESH_PRIMITIVES_MODE,             \
      .material = 0,                                         \
      .targets = DEFAULT_GLTF_MESH_PRIMITIVES_TARGETS }

#define DEFAULT_GLTF_MESH                         \
    { .name = NULL,                               \
      .primitives = DEFAULT_GLTF_MESH_PRIMITIVES, \
      .weights = NULL }

struct GLTF_attribute {
    float16_t POSITION[ 3 ];
    float16_t NORMAL[ 3 ];
    float16_t TANGENT[ 4 ];
    float16_t* TEXCOORD_n[ 2 ];
    float16_t* COLOR_n[ 4 ]; // If vec3 then 4th is 1.0
    float16_t* JOINTS_n[ 4 ];
    float16_t* WEIGHTS_n[ 4 ];
};

enum GLTF_primitive_mode {
    POINTS = 0,
    LINES,
    LINE_LOOP,
    LINE_STRIP,
    TRIANGLES,
    TRIANGLE_STRIP,
    TRIANGLE_FAN
};

struct GLTF_target {
    float16_t POSITION[ 3 ];
    float16_t NORMAL[ 3 ];
    float16_t TANGENT[ 4 ];
};

struct GLTF_primitive {
    struct GLTF_attribute* attributes;
    uint8_t indices;
    enum GLTF_primitive_mode mode;
    uint8_t material;
    struct GLTF_target* targets;
};

struct GLTF_mesh {
    char* name;
    struct GLTF_primitive* primitives;
    float16_t* weights;
};
