#pragma once

#include <stddef.h>
#include <stdint.h>

#define DEFAULT_GLTF_BUFFER { .name = NULL, .byteLength = 0, .uri = NULL }

struct GLTF_buffer {
    char* name;
    size_t byteLength;
    char* uri;
};
