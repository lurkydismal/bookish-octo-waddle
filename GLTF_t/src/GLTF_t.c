#include "GLTF_t.h"

#include "stdfunc.h"
#include "yyjson.h"

GLTF_t GLTF_t$create( void ) {
    GLTF_t l_returnValue = DEFAULT_GLTF;

    {
        l_returnValue.scenes =
            ( struct GLTF_scene* )createArray( sizeof( struct GLTF_scene* ) );
        l_returnValue.nodes =
            ( struct GLTF_node* )createArray( sizeof( struct GLTF_node* ) );
        l_returnValue.meshes =
            ( struct GLTF_mesh* )createArray( sizeof( struct GLTF_mesh* ) );
        l_returnValue.accessors = ( struct GLTF_accessor* )createArray(
            sizeof( struct GLTF_accessor* ) );
        l_returnValue.materials = ( struct GLTF_material* )createArray(
            sizeof( struct GLTF_material* ) );
        l_returnValue.bufferViews = ( struct GLTF_bufferView* )createArray(
            sizeof( struct GLTF_bufferView* ) );
        l_returnValue.buffers =
            ( struct GLTF_buffer* )createArray( sizeof( struct GLTF_buffer* ) );
        l_returnValue.samplers = ( struct GLTF_sampler* )createArray(
            sizeof( struct GLTF_sampler* ) );
        l_returnValue.textures = ( struct GLTF_texture* )createArray(
            sizeof( struct GLTF_texture* ) );
        l_returnValue.skins =
            ( struct GLTF_skin* )createArray( sizeof( struct GLTF_skin* ) );
        l_returnValue.images =
            ( struct GLTF_image* )createArray( sizeof( struct GLTF_image* ) );
        l_returnValue.cameras =
            ( struct GLTF_camera* )createArray( sizeof( struct GLTF_camera* ) );
        l_returnValue.animations = ( struct GLTF_animation* )createArray(
            sizeof( struct GLTF_animation* ) );
    }

    return ( l_returnValue );
}

bool GLTF_t$destroy( GLTF_t* restrict _GLTF ) {
    bool l_returnValue = false;

    if ( !_GLTF ) {
        goto EXIT;
    }

    {
        free( _GLTF->scenes );
        free( _GLTF->nodes );
        free( _GLTF->meshes );
        free( _GLTF->accessors );
        free( _GLTF->materials );
        free( _GLTF->bufferViews );
        free( _GLTF->buffers );
        free( _GLTF->samplers );
        free( _GLTF->textures );
        free( _GLTF->skins );
        free( _GLTF->images );
        free( _GLTF->cameras );
        free( _GLTF->animations );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool GLTF_t$load$fromAsset( GLTF_t* restrict _GLTF, asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_GLTF ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool GLTF_t$load$fromPath( GLTF_t* restrict _GLTF,
                           const char* restrict _path ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_GLTF ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_path ) ) {
        goto EXIT;
    }

    {
        asset_t l_GLTFAsset = asset_t$create();

        {
            l_returnValue = asset_t$load( &l_GLTFAsset, _path );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_IMAGE_ASSET;
            }

            l_returnValue = GLTF_t$load$fromAsset( _GLTF, &l_GLTFAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_IMAGE_ASSET;
            }

            l_returnValue = asset_t$unload( &l_GLTFAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_IMAGE_ASSET;
            }
        }

        l_returnValue = true;

    EXIT_IMAGE_ASSET:
        if ( UNLIKELY( !asset_t$destroy( &l_GLTFAsset ) ) ) {
            l_returnValue = false;

            goto EXIT;
        }
    }

EXIT:
    return ( l_returnValue );
}

bool GLTF_t$unload( GLTF_t* restrict _GLTF ) {}
