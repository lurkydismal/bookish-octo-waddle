#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "stdfloat16.h"
#include "stdfunc.h"

#define GLTF_ACCESSOR_MAX_BUFFER_SIZE 16
#define GLTF_ACCESSOR_MIN_BUFFER_SIZE 16

#define DEFAULT_GLTF_ACCESSOR_COMPONENT_TYPE INT8

#define DEFAULT_GLTF_ACCESSOR_MAX \
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

#define DEFAULT_GLTF_ACCESSOR_MIN \
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

#define DEFAULT_GLTF_ACCESSOR_TYPE SCALAR

#define DEFAULT_GLTF_ACCESSOR_SPARSE_INDICES \
    { .bufferView = 0,                       \
      .byteOffset = 0,                       \
      .componentType = DEFAULT_GLTF_ACCESSOR_COMPONENT_TYPE }

#define DEFAULT_GLTF_ACCESSOR_SPARSE_VALUES { .bufferView = 0, .byteOffset = 0 }

#define DEFAULT_GLTF_ACCESSOR_SPARSE                   \
    { .count = 0,                                      \
      .indices = DEFAULT_GLTF_ACCESSOR_SPARSE_INDICES, \
      .values = DEFAULT_GLTF_ACCESSOR_SPARSE_VALUES }

#define DEFAULT_GLTF_ACCESSOR                                \
    { .name = NULL,                                          \
      .bufferView = 0,                                       \
      .byteOffset = 0,                                       \
      .componentType = DEFAULT_GLTF_ACCESSOR_COMPONENT_TYPE, \
      .normalized = false,                                   \
      .count = 0,                                            \
      .type = DEFAULT_GLTF_ACCESSOR_TYPE,                    \
      .max = DEFAULT_GLTF_ACCESSOR_MAX,                      \
      .min = DEFAULT_GLTF_ACCESSOR_MIN,                      \
      .sparse = DEFAULT_GLTF_ACCESSOR_SPARSE }

enum GLTF_accessor_componentType {
    INT8 = 5120,
    UINT8 = 5121,
    INT16 = 5122,
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

struct GLTF_accessor_sparse_indices {
    uint8_t bufferView;
    size_t byteOffset;
    enum GLTF_accessor_componentType componentType;
};

struct GLTF_accessor_sparse_values {
    uint8_t bufferView;
    size_t byteOffset;
};

struct GLTF_accessor_sparse {
    uint8_t count;
    struct GLTF_accessor_sparse_indices indices;
    struct GLTF_accessor_sparse_values values;
};

struct GLTF_accessor {
    char* name;
    uint8_t bufferView;
    size_t byteOffset;
    enum GLTF_accessor_componentType componentType;
    bool normalized;
    uint32_t count;
    enum GLTF_accessor_type type;
    float max[ GLTF_ACCESSOR_MAX_BUFFER_SIZE ];
    float min[ GLTF_ACCESSOR_MIN_BUFFER_SIZE ];
    struct GLTF_accessor_sparse sparse;
};

static FORCE_INLINE enum GLTF_accessor_type GLTF_t$accessor$type$fromString(
    const char* _string ) {
    if ( UNLIKELY( !_string ) ) {
        return ( DEFAULT_GLTF_ACCESSOR_TYPE );
    }

    {
        if ( __builtin_strcmp( _string, "SCALAR" ) == 0 ) {
            return ( SCALAR );

        } else if ( __builtin_strcmp( _string, "VEC2" ) == 0 ) {
            return ( VEC2 );

        } else if ( __builtin_strcmp( _string, "VEC3" ) == 0 ) {
            return ( VEC3 );

        } else if ( __builtin_strcmp( _string, "VEC4" ) == 0 ) {
            return ( VEC4 );

        } else if ( __builtin_strcmp( _string, "MAT2" ) == 0 ) {
            return ( MAT2 );

        } else if ( __builtin_strcmp( _string, "MAT3" ) == 0 ) {
            return ( MAT3 );

        } else if ( __builtin_strcmp( _string, "MAT4" ) == 0 ) {
            return ( MAT4 );

        } else {
            return ( DEFAULT_GLTF_ACCESSOR_TYPE );
        }
    }
}

static FORCE_INLINE const char* GLTF_t$accessor$componentType$toString(
    const enum GLTF_accessor_componentType _componentType ) {
    switch ( _componentType ) {
        case ( INT8 ): {
            return ( "INT8" );
        }

        case ( UINT8 ): {
            return ( "UINT8" );
        }

        case ( INT16 ): {
            return ( "INT16" );
        }

        case ( UINT16 ): {
            return ( "UINT16" );
        }

        case ( UINT32 ): {
            return ( "UINT32" );
        }

        case ( FLOAT ): {
            return ( "FLOAT" );
        }

        default: {
            return ( GLTF_t$accessor$componentType$toString(
                DEFAULT_GLTF_ACCESSOR_COMPONENT_TYPE ) );
        }
    }
}

static FORCE_INLINE const char* GLTF_t$accessor$type$toString(
    const enum GLTF_accessor_type _type ) {
    switch ( _type ) {
        case ( SCALAR ): {
            return ( "SCALAR" );
        }

        case ( VEC2 ): {
            return ( "VEC2" );
        }

        case ( VEC3 ): {
            return ( "VEC3" );
        }

        case ( VEC4 ): {
            return ( "VEC4" );
        }

#if 0
       // The same as VEC4
        case (MAT2): {
            return ("MAT2");
        }
#endif

        case ( MAT3 ): {
            return ( "MAT3" );
        }

        case ( MAT4 ): {
            return ( "MAT4" );
        }

        default: {
            return (
                GLTF_t$accessor$type$toString( DEFAULT_GLTF_ACCESSOR_TYPE ) );
        }
    }
}
