#pragma once

#include <stdint.h>

#include "stdfunc.h"

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

static FORCE_INLINE const char* GLTF_t$sampler$filter$toString(
    const enum GLTF_sampler_filter _filter ) {
    switch ( _filter ) {
        case ( NEAREST ): {
            return ( "NEAREST" );
        }

        case ( LINEAR_filter ): {
            return ( "LINEAR" );
        }

        case ( NEAREST_MIPMAP_NEAREST ): {
            return ( "NEAREST_MIPMAP_NEAREST" );
        }

        case ( LINEAR_MIPMAP_NEAREST ): {
            return ( "LINEAR_MIPMAP_NEAREST" );
        }

        case ( NEAREST_MIPMAP_LINEAR ): {
            return ( "NEAREST_MIPMAP_LINEAR" );
        }

        case ( LINEAR_MIPMAP_LINEAR ): {
            return ( "LINEAR_MIPMAP_LINEAR" );
        }

        default: {
            return ( GLTF_t$sampler$filter$toString(
                DEFAULT_GLTF_SAMPLER_MAG_FILER ) );
        }
    }
}

static FORCE_INLINE const char* GLTF_t$sampler$wrapping$toString(
    const enum GLTF_sampler_wrapping _wrapping ) {
    switch ( _wrapping ) {
        case ( CLAMP_TO_EDGE ): {
            return ( "CLAMP_TO_EDGE" );
        }

        case ( MIRRORED_REPEAT ): {
            return ( "MIRRORED_REPEAT" );
        }

        case ( REPEAT ): {
            return ( "REPEAT" );
        }

        default: {
            return ( GLTF_t$sampler$wrapping$toString(
                DEFAULT_GLTF_SAMPLER_WRAP_T ) );
        }
    }
}
