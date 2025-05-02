#pragma once

#include <cglm/cglm.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "stdfloat16.h"
#include "stdfunc.h"

typedef struct {
    vec3 position;
    vec3 normal;
    vec2 texcoord;
    vec4 tangent;
    uvec4 joints;
    vec4 weights;
#if 0
    vec4 color;
#endif
} vertex_t;

typedef struct {
    vertex_t* vertexes;
    uint32_t *indices;
    size_t vertexCount;
    size_t indexCount;
    int materialIndex;
} mesh_t;

typedef struct {
    char* name;
    float baseColorFactor[4];    // RGBA multiplier
    int baseColorTexture;        // Index into texture array (or -1 if none)
    float metallicFactor;        // Scalar [0..1]
    float roughnessFactor;       // Scalar [0..1]
    int metallicRoughnessTexture;// Combined M (B) + R (G) texture
    int normalTexture;
    float normalScale;           // Strength of normal map
    int occlusionTexture;
    float occlusionStrength;
    int emissiveTexture;         // Emissive color map
    float emissiveFactor[3];     // RGB multiplier for emissive
    int doubleSided;
    enum alphaMode alphaMode;
    float alphaCutoff;
} material_t;

typedef struct {
    char* uri;
    sampler_t sampler;      // Texture sampling parameters
    unsigned int texId;   // GPU texture handle (e.g. OpenGL GLuint)
    int width;
    int height;    // Image dimensions
} texture_t;

typedef struct {
    int magFilter;  // e.g. GL_LINEAR, GL_NEAREST
    int minFilter;  // e.g. GL_LINEAR_MIPMAP_LINEAR
    int wrapS;      // e.g. GL_REPEAT
    int wrapT;      // e.g. GL_CLAMP_TO_EDGE
} sampler_t;

typedef struct {
    char* name;
    int parentIndex;       // Index of parent node (-1 if root)
    int *children;         // Array of child node indices
    int childCount;
    float localMatrix[16];// Local transform (col-major 4x4 matrix)
    float worldMatrix[16];// World transform (computed = parent * local)
    int meshIndex;         // -1 if no mesh
    int skinIndex;         // -1 if no skin (skeleton)
} node_t;

typedef struct {
    int jointCount;
    int *jointNodeIndices;   // Array of node indices for each joint
    float *inverseBindMatrices; // Flat array [jointCount][16] (each mat4)
    int skeletonRoot;        // Optionally the index of the root joint
} skin_t;

typedef struct {
    float time;
    float translation[3];
    float rotation[4];   // quaternion (x,y,z,w)
    float scale[3];
} keyframe_t;

typedef struct {
    int nodeIndex;         // target node/joint index
    int keyframeCount;
    keyframe_t *keyframes;   // Sorted by time
    enum interpolation_t interp;  // glTF sampler interpolation
} animationChannel_t;

typedef struct {
    char* name;           // Animation name (if any)
    int channelCount;
    animationChannel_t *channels;
    float startTime, endTime; // Overall animation duration
} animation_t;
