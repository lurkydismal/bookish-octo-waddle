#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "GLTF_accessor.h"
#include "GLTF_animation.h"
#include "GLTF_asset.h"
#include "GLTF_buffer.h"
#include "GLTF_buffer_view.h"
#include "GLTF_camera.h"
#include "GLTF_image.h"
#include "GLTF_material.h"
#include "GLTF_mesh.h"
#include "GLTF_node.h"
#include "GLTF_sampler.h"
#include "GLTF_scene.h"
#include "GLTF_skin.h"
#include "GLTF_texture.h"
#include "asset_t.h"
#include "stdfloat16.h"

#define DEFAULT_GLTF               \
    { .asset = DEFAULT_GLTF_ASSET, \
      .scene = 0,                  \
      .scenes = NULL,              \
      .nodes = NULL,               \
      .meshes = NULL,              \
      .accessors = NULL,           \
      .materials = NULL,           \
      .bufferViews = NULL,         \
      .buffers = NULL,             \
      .samplers = NULL,            \
      .textures = NULL,            \
      .skins = NULL,               \
      .images = NULL,              \
      .cameras = NULL,             \
      .animations = NULL }

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
bool GLTF_t$destroy( GLTF_t* restrict _GLTF );

bool GLTF_t$load$fromAsset( GLTF_t* restrict _GLTF, asset_t* restrict _asset );
bool GLTF_t$load$fromPath( GLTF_t* restrict _GLTF, const char* restrict _path );
bool GLTF_t$unload( GLTF_t* restrict _GLTF );
