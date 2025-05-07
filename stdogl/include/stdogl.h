#pragma once

#include <cglm/cglm.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "GLTF_t.h"
#include "log.h"
#include "stdfloat16.h"

#define readAccessor( _out, _fieldName, _accessor, _GLTF )                   \
    do {                                                                     \
        struct GLTF_bufferView* bufferView =                                 \
            ( _GLTF )->bufferViews[ ( _accessor )->bufferView ];             \
        uint8_t* buffer =                                                    \
            ( _GLTF )->buffers[ bufferView->buffer ]->asset->data;           \
        size_t byteOffset =                                                  \
            ( bufferView->byteOffset + ( _accessor )->byteOffset );          \
        uint8_t* dataStart = ( buffer + byteOffset );                        \
        size_t l_componentSize;                                              \
        switch ( ( _accessor )->componentType ) {                            \
            case ( BYTE ): {                                                 \
                l_componentSize = sizeof( int8_t );                          \
                break;                                                       \
            }                                                                \
            case ( UNSIGNED_BYTE ): {                                        \
                l_componentSize = sizeof( uint8_t );                         \
                break;                                                       \
            }                                                                \
            case ( SHORT ): {                                                \
                l_componentSize = sizeof( int16_t );                         \
                break;                                                       \
            }                                                                \
            case ( UNSIGNED_SHORT ): {                                       \
                l_componentSize = sizeof( uint16_t );                        \
                break;                                                       \
            }                                                                \
            case ( UNSIGNED_INT ): {                                         \
                l_componentSize = sizeof( uint32_t );                        \
                break;                                                       \
            }                                                                \
            case ( FLOAT ): {                                                \
                l_componentSize = sizeof( float );                           \
                break;                                                       \
            }                                                                \
            default: {                                                       \
                l_componentSize = sizeof( int8_t );                          \
            }                                                                \
        }                                                                    \
        size_t stride = ( ( bufferView->byteStride )                         \
                              ? ( bufferView->byteStride )                   \
                              : ( ( _accessor )->type * l_componentSize ) ); \
        FOR_RANGE( size_t, 0, ( _accessor )->count ) {                       \
            typeof( ( _out )[ 0 ]._fieldName[ 0 ] )* data =                  \
                ( typeof( ( _out )[ 0 ]._fieldName[ 0 ] )* )( dataStart +    \
                                                              ( _index *     \
                                                                stride ) );  \
            memcpy( &( ( _out )[ _index ]._fieldName ), data,                \
                    ( _accessor )->type* l_componentSize );                  \
        }                                                                    \
    } while ( 0 )

typedef struct {
    vec3 position;
    vec3 normal;
    vec4 tangent;
    vec2 texcoord_0;
    vec2 texcoord_1;
    vec4 joints;
    vec4 weights;
} vertex_t;

static FORCE_INLINE void mapAttributesToVertex( vertex_t* _vertex,
                            struct GLTF_mesh_primitive_attributes* _attributes,
                            GLTF_t* _GLTF ) {
    readAccessor( _vertex, position, _GLTF->accessors[ _attributes->POSITION ],
                  _GLTF );
    readAccessor( _vertex, normal, _GLTF->accessors[ _attributes->NORMAL ],
                  _GLTF );
    readAccessor( _vertex, tangent, _GLTF->accessors[ _attributes->TANGENT ],
                  _GLTF );
    readAccessor( _vertex, texcoord_0,
                  _GLTF->accessors[ _attributes->TEXCOORD_0 ], _GLTF );
    readAccessor( _vertex, texcoord_1,
                  _GLTF->accessors[ _attributes->TEXCOORD_1 ], _GLTF );
    readAccessor( _vertex, joints, _GLTF->accessors[ _attributes->JOINTS_0 ],
                  _GLTF );
    readAccessor( _vertex, weights, _GLTF->accessors[ _attributes->WEIGHTS_0 ],
                  _GLTF );
}

// TODO
typedef struct {
    vertex_t* vertexes;
    uint32_t* indices;
    size_t vertexCount;
    size_t indexCount;
    int materialIndex;
} mesh_t;

typedef struct {
    char* name;
    vec4 baseColorFactor;         // RGBA multiplier
    int baseColorTexture;         // Index into texture array (or -1 if none)
    float metallicFactor;         // Scalar [0..1]
    float roughnessFactor;        // Scalar [0..1]
    int metallicRoughnessTexture; // Combined M (B) + R (G) texture
    int normalTexture;
    float normalScale; // Strength of normal map
    int occlusionTexture;
    float occlusionStrength;
    int emissiveTexture; // Emissive color map
    vec3 emissiveFactor; // RGB multiplier for emissive
    int doubleSided;
    enum GLTF_material_alphaMode alphaMode;
    float alphaCutoff;
} material_t;

void mapMaterial( material_t* _material,
                  struct GLTF_material* _GLTF_material,
                  GLTF_t* _GLTF ) {}

typedef struct {
    int magFilter; // e.g. GL_LINEAR, GL_NEAREST
    int minFilter; // e.g. GL_LINEAR_MIPMAP_LINEAR
    int wrapS;     // e.g. GL_REPEAT
    int wrapT;     // e.g. GL_CLAMP_TO_EDGE
} sampler_t;

typedef struct {
    char* uri;
    sampler_t sampler;  // Texture sampling parameters
    unsigned int texId; // GPU texture handle (e.g. OpenGL GLuint)
    int width;
    int height; // Image dimensions
} texture_t;

typedef struct {
    char* name;
    int parentIndex; // Index of parent node (-1 if root)
    int* children;   // Array of child node indices
    int childCount;
    mat4 localMatrix; // Local transform (col-major 4x4 matrix)
    mat4 worldMatrix; // World transform (computed = parent * local)
    int meshIndex;    // -1 if no mesh
    int skinIndex;    // -1 if no skin (skeleton)
} node_t;

typedef struct {
    int jointCount;
    int* jointNodeIndices;      // Array of node indices for each joint
    float* inverseBindMatrices; // Flat array [jointCount][16] (each mat4)
    int skeletonRoot;           // Optionally the index of the root joint
} skin_t;

// TODO: keyframes
typedef struct {
    float time;
    vec3 translation;
    vec4 rotation;
    vec3 scale;
} keyframe_t;

typedef struct {
    struct GLTF_animation_channel** channels;
    // TODO: keyframes
    keyframe_t** channelsKeyframes;
} animation_channel_t;

typedef struct {
    char* name;
    animation_channel_t** channels;
    float startTime;
    float endTime;
} animation_t;

static FORCE_INLINE void mapAnimation( animation_t* _animation,
                                       struct GLTF_animation* _GLTF_animation,
                                       struct GLTF_accessor** _accessors ) {
    _animation->name = _GLTF_animation->name;
    // _animation->channels = _GLTF_animation->channels;

    _animation->startTime = FLT_MAX;
    _animation->endTime = -FLT_MAX;

    FOR_ARRAY( struct GLTF_animation_sampler* const*,
               _GLTF_animation->samplers ) {
        uint8_t inputIndex = ( *_element )->input;
        struct GLTF_accessor* l_accessor = _accessors[ inputIndex ];

        if ( l_accessor->componentType != FLOAT ||
             l_accessor->type != SCALAR ) {
            log$transaction$query( ( logLevel_t )error, "\n" );

            continue;
        }

        float l_min = l_accessor->min[ 0 ];
        float l_max = l_accessor->max[ 0 ];

        if ( l_min < _animation->startTime )
            _animation->startTime = l_min;
        if ( l_max > _animation->endTime )
            _animation->endTime = l_max;
    }
}

#undef readAccessor
