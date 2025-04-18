#pragma once

#include <stdint.h>

#define DEFAULT_GLTF_ANIMATION_CHANNEL_TARGET_PATH TRANSLATION

#define DEFAULT_GLTF_ANIMATION_CHANNEL_TARGET \
    { .node = 0, .path = DEFAULT_GLTF_ANIMATION_CHANNEL_TARGET_PATH }

#define DEFAULT_GLTF_ANIMATION_CHANNEL \
    { .sampler = 0, .target = DEFAULT_GLTF_ANIMATION_CHANNEL_TARGET }

#define DEFAULT_GLTF_ANIMATION_SAMPLER_INTERPOLATION LINEAR_interpolation

#define DEFAULT_GLTF_ANIMATION_SAMPLER                               \
    { .input = 0,                                                    \
      .interpolation = DEFAULT_GLTF_ANIMATION_SAMPLER_INTERPOLATION, \
      .output = 0 }

#define DEFAULT_GLTF_ANIMATION                                        \
    {                                                                 \
        .name = NULL, .channels = { DEFAULT_GLTF_ANIMATION_CHANNEL }, \
        .samplers = {                                                 \
            DEFAULT_GLTF_ANIMATION_SAMPLER                            \
        }                                                             \
    }

enum GLTF_animation_channel_target_path {
    TRANSLATION,
    ROTATION,
    SCALE,
    WEIGHTS
};

struct GLTF_animation_channel_target {
    uint16_t node;
    enum GLTF_animation_channel_target_path path;
};

struct GLTF_animation_channel {
    uint8_t sampler;
    struct GLTF_animation_channel_target target;
};

enum GLTF_animation_sampler_interpolation {
    LINEAR_interpolation,
    STEP,
    CUBICSPLINE
};

struct GLTF_animation_sampler {
    uint8_t input;
    enum GLTF_animation_sampler_interpolation interpolation;
    uint8_t output;
};

struct GLTF_animation {
    char* name;
    struct GLTF_animation_channel* channels;
    struct GLTF_animation_sampler* sampler;
};
