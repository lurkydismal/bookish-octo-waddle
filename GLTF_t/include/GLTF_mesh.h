#pragma once

#include <stdint.h>

#include "stdfloat16.h"
#include "stdfunc.h"

#define DEFAULT_GLTF_MESH_PRIMITIVES_ATTRIBUTES \
    { .POSITION = 0,                            \
      .NORMAL = 0,                              \
      .TANGENT = 0,                             \
      .TEXCOORD_0 = 0,                          \
      .TEXCOORD_1 = 0,                          \
      .JOINTS_0 = 0,                            \
      .WEIGHTS_0 = 0 }

#define DEFAULT_GLTF_MESH_PRIMITIVES_MODE TRIANGLES

#define DEFAULT_GLTF_MESH_PRIMITIVES_TARGETS \
    { .POSITION = 0, .NORMAL = 0, .TANGENT = 0 }

#define DEFAULT_GLTF_MESH_PRIMITIVES                         \
    { .attributes = DEFAULT_GLTF_MESH_PRIMITIVES_ATTRIBUTES, \
      .indices = 0,                                          \
      .mode = DEFAULT_GLTF_MESH_PRIMITIVES_MODE,             \
      .material = 0,                                         \
      .targets = NULL }

#define DEFAULT_GLTF_MESH { .name = NULL, .primitives = NULL, .weights = NULL }

struct GLTF_mesh_primitive_attributes {
    uint16_t POSITION;
    uint16_t NORMAL;
    uint16_t TANGENT;
    uint16_t TEXCOORD_0;
    uint16_t TEXCOORD_1;
    uint16_t JOINTS_0;
    uint16_t WEIGHTS_0;
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
    uint16_t POSITION;
    uint16_t NORMAL;
    uint16_t TANGENT;
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
