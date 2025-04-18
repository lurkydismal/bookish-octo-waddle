#pragma once

#include <stdint.h>

#include "GLTF_texture_info.h"
#include "stdfloat16.h"

#define DEFAULT_GLTF_MATERIAL_PBR_METALLIC_ROUGHNESS_BASE_COLOR_FACTOR \
    { 0, 0, 0, 0 }

#define DEFAULT_GLTF_MATERIAL_PBR_METALLIC_ROUGHNESS_BASE_COLOR_TEXTURE \
    DEFAULT_GLTF_TEXTURE_INFO

#define DEFAULT_GLTF_MATERIAL_PBR_METALLIC_ROUGHNESS_METALLIC_ROUGHNESS_TEXTURE \
    DEFAULT_GLTF_TEXTURE_INFO

#define DEFAULT_GLTF_MATERIAL_PBR_METALLIC_ROUGHNESS                       \
    { .baseColorFactor =                                                   \
          DEFAULT_GLTF_MATERIAL_PBR_METALLIC_ROUGHNESS_BASE_COLOR_FACTOR,  \
      .metallicFactor = 0,                                                 \
      .roughnessFactor = 0,                                                \
      .baseColorTexture =                                                  \
          DEFAULT_GLTF_MATERIAL_PBR_METALLIC_ROUGHNESS_BASE_COLOR_TEXTURE, \
      .metallicRoughnessTexture =                                          \
          DEFAULT_GLTF_MATERIAL_PBR_METALLIC_ROUGHNESS_METALLIC_ROUGHNESS_TEXTURE }

#define DEFAULT_GLTF_MATERIAL_NORMAL_TEXTURE_INFO \
    { .index = 0, .texCoord = 0, .scale = 0 }

#define DEFAULT_GLTF_MATERIAL_OCCLUSION_TEXTURE_INFO \
    { .index = 0, .texCoord = 0, .strength = 0 }

#define DEFAULT_GLTF_MATERIAL_EMISSIVE_TEXTURE DEFAULT_GLTF_TEXTURE_INFO

#define DEFAULT_GLTF_MATERIAL_EMISSIVE_FACTOR { 0, 0, 0 }

#define DEFAULT_GLTF_MATERIAL_ALPHA_MODE OPAQUE

#define DEFAULT_GLTF_MATERIAL                                               \
    { .name = NULL,                                                         \
      .pbrMetallicRoughness = DEFAULT_GLTF_MATERIAL_PBR_METALLIC_ROUGHNESS, \
      .normalTexture = DEFAULT_GLTF_MATERIAL_NORMAL_TEXTURE_INFO,           \
      .occlusionTexture = DEFAULT_GLTF_MATERIAL_OCCLUSION_TEXTURE_INFO,     \
      .emissiveTexture = DEFAULT_GLTF_MATERIAL_EMISSIVE_TEXTURE,            \
      .emissiveFactor = DEFAULT_GLTF_MATERIAL_EMISSIVE_FACTOR,              \
      .alphaMode = DEFAULT_GLTF_MATERIAL_ALPHA_MODE,                        \
      .alphaCutoff = 0,                                                     \
      .doubleSided = false }

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

enum GLTF_material_alphaMode { OPAQUE, MASK, BLEND };

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
