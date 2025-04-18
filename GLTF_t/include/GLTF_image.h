#pragma once

#include <stdint.h>

#define DEFAULT_GLTF_IMAGE { .name = NULL, .uri = NULL, .bufferView = 0 }

// Ignoring mime type
struct GLTF_image {
    char* name;
    char* uri;
    uint8_t bufferView;
};
