#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "stdfloat16.h"

#define DEFAULT_GLTF       \
    { .asset = NULL,       \
      .scene = 0,          \
      .scenes = NULL,      \
      .nodes = NULL,       \
      .meshes = NULL,      \
      .accessors = NULL,   \
      .materials = NULL,   \
      .bufferViews = NULL, \
      .buffers = NULL,     \
      .samplers = NULL,    \
      .textures = NULL,    \
      .skins = NULL,       \
      .images = NULL,      \
      .cameras = NULL,     \
      .animations = NULL }

enum GLTF_accessor_componentType {
    INT8 = 5120,
    UINT8 = 5121,
    INT16 = 5121,
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

struct GLTF_asset {
    uint8_t version;
    char* generator;
    char* copyright;
};

struct GLTF_scene {
    char* name;
    uint16_t* nodes;
};

struct GLTF_node {
    char* name;
    uint8_t* children;
    float matrix[ 16 ];
    uint8_t mesh;
    float16_t* weights;
    float16_t translation[ 3 ];
    float16_t scale[ 3 ];
    float16_t rotation[ 4 ];
    uint8_t skin;
    uint8_t camera;
};

struct GLTF_attribute {
    float16_t POSITION[ 3 ];
    float16_t NORMAL[ 3 ];
    float16_t TANGENT[ 4 ];
    float16_t TEXCOORD_n[ 2 ];
    float16_t COLOR_n[ ? ];
    float16_t JOINTS_n[ 4 ];
    float16_t WEIGHTS_n[ 4 ];
};

struct GLTF_primitive {
    // boject attributes;
    // struct GLTF_attribute* attributes;
    uint8_t indices;
    uint8_t mode;
    uint8_t material;
    // object* targets
};

struct GLTF_mesh {
    char* name;
    struct GLTF_primitive* primitives;
    float16_t* weights;
};

struct GLTF_accessor {
    uint8_t bufferView;
    uint8_t bufferOffset;
    enum GLTF_accessor_componentType componentType;
    uint32_t count;
    float max[ 16 ];
    float min[ 16 ];
    enum GLTF_accessor_type type;
    bool normalized;
};

struct GLTF_texture_info {
    uint8_t index;
    uint32_t texCoord;
};

struct GLTF_material_pbrMetallicRoughness {
    float16_t baseColorFactor[ 4 ];
    float16_t metallicFactor;
    float16_t roughnessFactor;
    struct GLTF_texture_info baseColorTexture;
    struct GLTF_texture_info metallicRoughnessTexture;
};

struct GLTF_material_normal_texture_info {
    uint8_t index;
    uint8_t texCoord;
    float16_t scale;
};

struct GLTF_material_occlusion_texture_info {
    uint8_t index;
    uint8_t texCoord;
    float16_t strength;
};

enum GLTF_material_alphaMode {
    OPAQUE,
    MASK,
    BLEND
};

struct GLTF_material {
    char* name;
    struct GLTF_material_pbrMetallicRoughness pbrMetallicRoughness;
    struct GLTF_material_normal_texture_info normalTexture;
    struct GLTF_material_occlusion_texture_info occlusionTexture;
    struct GLTF_texture_info emissiveTexture;
    float16_t emissiveFactor[ 3 ];
    enum GLTF_material_alphaMode alphaMode;
    float16_t alphaCutoff;
    bool doubleSided;
};

struct GLTF_bufferView {
    char* name;
    uint8_t buffer;
    size_t bufferOffset;
    size_t byteLength;
    uint32_t target;
    uint32_t byteStride;
};

struct GLTF_buffer {
    char* name;
    size_t byteLength;
    char* uri;
};

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

struct GLTF_texture {
    char* name;
    uint8_t sampler;
    uint8_t source;
};

struct GLTF_skin {
    char* name;
    uint8_t inverseBindMatrices;
    uint16_t skeleton;
    uint16_t* joints;
};

// Ignoring mime type
struct GLTF_image {
    char* name;
    char* uri;
    uint8_t bufferView;
};

enum GLTF_camera_type { PERSPECTIVE, ORTHOGRAPHIC };

struct GLTF_camera_orthographic {
    float16_t xmag;
    float16_t ymag;
    float16_t zfar;
    float16_t znear;
};

struct GLTF_camera_perspective {
    float16_t aspectRatio;
    float16_t yfov;
    float16_t zfar;
    float16_t znear;
};

struct GLTF_camera {
    char* name;
    enum GLTF_camera_type type;
    struct GLTF_camera_orthographic orthographic;
    struct GLTF_camera_perspective perspective;
};

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

typedef struct {
    struct GLTF_asset asset;
    uint8_t scene;
    struct GLTF_scene* scenes;
    struct GLTF_node* nodes;
    struct GLTF_mesh* meshes;
    struct GLTF_accessor* accessors;
    struct GLTF_material* materials;
    struct GLTF_bufferView* bufferViews;
    struct GLTF_buffer* buffers;
    struct GLTF_sampler* samplers;
    struct GLTF_texture* textures;
    struct GLTF_skin* skins;
    struct GLTF_image* images;
    struct GLTF_camera* cameras;
    struct GLTF_animation* animations;
} GLTF_t;

GLTF_t GLTF_t$create( void );
bool GLTF_t$destroy( GLTF_t* _gltf );

bool GLTF_t$load( GLTF_t* _gltf );
bool GLTF_t$unload( GLTF_t* _gltf );
