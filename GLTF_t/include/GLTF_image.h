#pragma once

#include <stdint.h>

#include "asset_t.h"

#define DEFAULT_GLTF_IMAGE { .name = NULL, .uri = NULL, .bufferView = 0 }

// Ignoring mime type
struct GLTF_image {
    char* name;
    char* uri;
    uint8_t bufferView;
    asset_t* asset;
};
