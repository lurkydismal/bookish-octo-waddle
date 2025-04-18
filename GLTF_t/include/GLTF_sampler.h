#pragma once

#include <stdint.h>

#define DEFAULT_GLTF_SAMPLER_MAG_FILER NEAREST

#define DEFAULT_GLTF_SAMPLER_MIN_FILER NEAREST

#define DEFAULT_GLTF_SAMPLER_WRAP_S CLAMP_TO_EDGE

#define DEFAULT_GLTF_SAMPLER_WRAP_T CLAMP_TO_EDGE

#define DEFAULT_GLTF_SAMPLER                         \
    {                                                \
        .name = NULL,                                \
        .magFilter = DEFAULT_GLTF_SAMPLER_MAG_FILER, \
        .minFilter = DEFAULT_GLTF_SAMPLER_MIN_FILER, \
        .wrapS = DEFAULT_GLTF_SAMPLER_WRAP_S,        \
        .wrapT = DEFAULT_GLTF_SAMPLER_WRAP_T,        \
    }

enum GLTF_sampler_filter {
    NEAREST = 9728,
    LINEAR_filter = 9729,
    NEAREST_MIPMAP_NEAREST = 9984,
    LINEAR_MIPMAP_NEAREST = 9985,
    NEAREST_MIPMAP_LINEAR = 9986,
    LINEAR_MIPMAP_LINEAR = 9987
};

enum GLTF_sampler_wrapping {
    CLAMP_TO_EDGE = 33071,
    MIRRORED_REPEAT = 33648,
    REPEAT = 10497
};

struct GLTF_sampler {
    char* name;
    enum GLTF_sampler_filter magFilter;
    enum GLTF_sampler_filter minFilter;
    enum GLTF_sampler_wrapping wrapS;
    enum GLTF_sampler_wrapping wrapT;
};
