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

#define DEFAULT_GLTF_ANIMATION \
    { .name = NULL, .channels = NULL, .samplers = NULL }

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
    struct GLTF_animation_channel** channels;
    struct GLTF_animation_sampler** samplers;
};

static FORCE_INLINE enum GLTF_animation_channel_target_path
GLTF_t$animation$channel$target$path$fromString( const char* _string ) {
    if ( !_string ) {
        return ( DEFAULT_GLTF_ANIMATION_CHANNEL_TARGET_PATH );
    }

    {
        if ( __builtin_strcmp( _string, "translation" ) == 0 ) {
            return ( TRANSLATION );

        } else if ( __builtin_strcmp( _string, "rotation" ) == 0 ) {
            return ( ROTATION );

        } else if ( __builtin_strcmp( _string, "scale" ) == 0 ) {
            return ( SCALE );

        } else if ( __builtin_strcmp( _string, "weights" ) == 0 ) {
            return ( WEIGHTS );

        } else {
            return ( DEFAULT_GLTF_ANIMATION_CHANNEL_TARGET_PATH );
        }
    }
}

static FORCE_INLINE enum GLTF_animation_sampler_interpolation
GLTF_t$animation$sampler$interpolation$fromString( const char* _string ) {
    if ( !_string ) {
        return ( DEFAULT_GLTF_ANIMATION_SAMPLER_INTERPOLATION );
    }

    {
        if ( __builtin_strcmp( _string, "LINEAR" ) == 0 ) {
            return ( LINEAR_interpolation );

        } else if ( __builtin_strcmp( _string, "STEP" ) == 0 ) {
            return ( STEP );

        } else if ( __builtin_strcmp( _string, "CUBICSPLINE" ) == 0 ) {
            return ( CUBICSPLINE );

        } else {
            return ( DEFAULT_GLTF_ANIMATION_SAMPLER_INTERPOLATION );
        }
    }
}

static FORCE_INLINE const char* GLTF_t$animation$channel$target$path$toString(
    const enum GLTF_animation_channel_target_path _type ) {
    switch ( _type ) {
        case ( TRANSLATION ): {
            return ( "translation" );
        }

        case ( ROTATION ): {
            return ( "rotation" );
        }

        case ( SCALE ): {
            return ( "scale" );
        }

        case ( WEIGHTS ): {
            return ( "weights" );
        }

        default: {
            return ( GLTF_t$animation$channel$target$path$toString(
                DEFAULT_GLTF_ANIMATION_CHANNEL_TARGET_PATH ) );
        }
    }
}

static FORCE_INLINE const char* GLTF_t$animation$sampler$interpolation$toString(
    const enum GLTF_animation_sampler_interpolation _type ) {
    switch ( _type ) {
        case ( LINEAR_interpolation ): {
            return ( "LINEAR" );
        }

        case ( STEP ): {
            return ( "STEP" );
        }

        case ( CUBICSPLINE ): {
            return ( "CUBICSPLINE" );
        }

        default: {
            return ( GLTF_t$animation$sampler$interpolation$toString(
                DEFAULT_GLTF_ANIMATION_SAMPLER_INTERPOLATION ) );
        }
    }
}
