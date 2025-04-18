#pragma once

#include <stdint.h>

#include "stdfloat16.h"

#define DEFAULT_GLTF_ACCESSOR_COMPONENT_TYPE INT8

#define DEFAULT_GLTF_ACCESSOR_MAX \
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

#define DEFAULT_GLTF_ACCESSOR_MIN \
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

#define DEFAULT_GLTF_ACCESSOR_TYPE SCALAR

#define DEFAULT_GLTF_ACCESSOR                                \
    { .bufferView = 0,                                       \
      .bufferOffset = 0,                                     \
      .componentType = DEFAULT_GLTF_ACCESSOR_COMPONENT_TYPE, \
      .count = 0,                                            \
      .max = DEFAULT_GLTF_ACCESSOR_MAX,                      \
      .min = DEFAULT_GLTF_ACCESSOR_MIN,                      \
      .type = DEFAULT_GLTF_ACCESSOR_TYPE,                    \
      .normalized = false }

enum GLTF_accessor_componentType {
    INT8 = 5120,
    UINT8 = 5121,
    INT16 = 5121,
    UINT16 = 5123,
    UINT32 = 5125,
    FLOAT = 5126
};

enum GLTF_accessor_type {
    SCALAR = 1,
    VEC2 = 2,
    VEC3 = 3,
    VEC4 = 4,
    MAT2 = 4,
    MAT3 = 9,
    MAT4 = 16
};

struct GLTF_accessor {
    uint8_t bufferView;
    uint8_t bufferOffset;
    enum GLTF_accessor_componentType componentType;
    uint32_t count;
    float16_t max[ 16 ];
    float16_t min[ 16 ];
    enum GLTF_accessor_type type;
    bool normalized;
};
