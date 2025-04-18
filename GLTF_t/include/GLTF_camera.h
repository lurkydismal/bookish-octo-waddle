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
    float16_t xmag;
    float16_t ymag;
    float16_t zfar;
    float16_t znear;
};

struct GLTF_camera_perspective {
    float16_t aspectRatio;
    float16_t yfov;
    float16_t zfar;
    float16_t znear;
};

struct GLTF_camera {
    char* name;
    enum GLTF_camera_type type;
    struct GLTF_camera_orthographic orthographic;
    struct GLTF_camera_perspective perspective;
};
