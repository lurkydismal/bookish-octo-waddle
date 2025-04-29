#pragma once

#include <stdint.h>

#include "stdfloat16.h"

#define DEFAULT_GLTF_CAMERA_TYPE PERSPECTIVE

#define DEFAULT_GLTF_CAMERA_ORTHOGRAPHIC \
    { .xmag = 0, .ymag = 0, .zfar = 0, .znear = 0 }

#define DEFAULT_GLTF_CAMERA_PERSPECTIVE \
    { .aspectRatio = 0, .yfov = 0, .zfar = 0, .znear = 0 }

#define DEFAULT_GLTF_CAMERA                             \
    { .name = NULL,                                     \
      .type = DEFAULT_GLTF_CAMERA_TYPE,                 \
      .orthographic = DEFAULT_GLTF_CAMERA_ORTHOGRAPHIC, \
      .perspective = DEFAULT_GLTF_CAMERA_PERSPECTIVE }

enum GLTF_camera_type { PERSPECTIVE, ORTHOGRAPHIC };

struct GLTF_camera_orthographic {
    float xmag;
    float ymag;
    float zfar;
    float znear;
};

struct GLTF_camera_perspective {
    float aspectRatio;
    float yfov;
    float zfar;
    float znear;
};

struct GLTF_camera {
    char* name;
    enum GLTF_camera_type type;
    struct GLTF_camera_orthographic orthographic;
    struct GLTF_camera_perspective perspective;
};

static FORCE_INLINE enum GLTF_camera_type GLTF_t$camera$type$fromString(
    const char* _string ) {
    if ( !_string ) {
        return ( DEFAULT_GLTF_CAMERA_TYPE );
    }

    {
        if ( __builtin_strcmp( _string, "perspective" ) == 0 ) {
            return ( PERSPECTIVE );

        } else if ( __builtin_strcmp( _string, "orthographic" ) == 0 ) {
            return ( ORTHOGRAPHIC );

        } else {
            return ( DEFAULT_GLTF_CAMERA_TYPE );
        }
    }
}

static FORCE_INLINE const char* GLTF_t$camera$type$toString(
    const enum GLTF_camera_type _type ) {
    switch ( _type ) {
        case ( PERSPECTIVE ): {
            return ( "perspective" );
        }

        case ( ORTHOGRAPHIC ): {
            return ( "orthographic" );
        }

        default: {
            return ( GLTF_t$camera$type$toString( DEFAULT_GLTF_CAMERA_TYPE ) );
        }
    }
}
