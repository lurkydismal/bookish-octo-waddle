#pragma once

#include <stdint.h>

#define DEFAULT_GLTF_BUFFER_VIEW \
    { .name = NULL,              \
      .buffer = 0,               \
      .bufferOffset = 0,         \
      .byteLength = 0,           \
      .target = 0,               \
      .byteStride = 0 }

struct GLTF_bufferView {
    char* name;
    uint8_t buffer;
    size_t bufferOffset;
    size_t byteLength;
    uint32_t target;
    uint32_t byteStride;
};
