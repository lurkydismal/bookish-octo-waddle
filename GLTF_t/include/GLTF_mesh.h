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

#define DEFAULT_GLTF_MESH_PRIMITIVES_MODE TRIANGLES

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
      .targets = NULL }

#define DEFAULT_GLTF_MESH { .name = NULL, .primitives = NULL, .weights = NULL }

struct GLTF_mesh_primitive_attributes {
    float POSITION[ 3 ];
    float NORMAL[ 3 ];
    float TANGENT[ 4 ];
#if 0
    // TODO: Implement
    float* TEXCOORD_n[ 2 ];
    float* COLOR_n[ 4 ]; // If vec3 then 4th is 1.0
    float* JOINTS_n[ 4 ];
    float* WEIGHTS_n[ 4 ];
#endif
    float** TEXCOORD_n;
    float** COLOR_n; // If vec3 then 4th is 1.0
    float** JOINTS_n;
    float** WEIGHTS_n;
};

enum GLTF_mesh_primitive_mode {
    POINTS = 0,
    LINES = 1,
    LINE_LOOP = 2,
    LINE_STRIP = 3,
    TRIANGLES = 4,
    TRIANGLE_STRIP = 5,
    TRIANGLE_FAN = 6
};

struct GLTF_mesh_primitive_target {
    float POSITION[ 3 ];
    float NORMAL[ 3 ];
    float TANGENT[ 4 ];
};

struct GLTF_mesh_primitive {
    struct GLTF_mesh_primitive_attributes attributes;
    uint8_t indices;
    enum GLTF_mesh_primitive_mode mode;
    uint8_t material;
    struct GLTF_mesh_primitive_target** targets;
};

struct GLTF_mesh {
    char* name;
    struct GLTF_mesh_primitive** primitives;
    float* weights;
};

static FORCE_INLINE const char* GLTF_t$mesh$primitive$mode$toString(
    const enum GLTF_mesh_primitive_mode _mode ) {
    switch ( _mode ) {
        case ( POINTS ): {
            return ( "POINTS" );
        }

        case ( LINES ): {
            return ( "LINES" );
        }

        case ( LINE_LOOP ): {
            return ( "LINE_LOOP" );
        }

        case ( LINE_STRIP ): {
            return ( "LINE_STRIP" );
        }

        case ( TRIANGLES ): {
            return ( "TRIANGLES" );
        }

        case ( TRIANGLE_STRIP ): {
            return ( "TRIANGLE_STRIP" );
        }

        case ( TRIANGLE_FAN ): {
            return ( "TRIANGLE_FAN" );
        }

        default: {
            return ( GLTF_t$mesh$primitive$mode$toString(
                DEFAULT_GLTF_MESH_PRIMITIVES_MODE ) );
        }
    }
}
