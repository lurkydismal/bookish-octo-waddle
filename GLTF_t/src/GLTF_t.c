#include "GLTF_t.h"

#include "log.h"
#include "stdfunc.h"
#include "yyjson.h"

#define FOR_JSON_ARRAY( _array ) \
    size_t _index;               \
    size_t _indexMax;            \
    yyjson_val* _element;        \
    yyjson_arr_foreach( _array, _index, _indexMax, _element )

GLTF_t GLTF_t$create( void ) {
    GLTF_t l_returnValue = DEFAULT_GLTF;

    {
        l_returnValue.scenes =
            ( struct GLTF_scene** )createArray( sizeof( struct GLTF_scene* ) );
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
        FREE_ARRAY( _GLTF->scenes );
        FREE_ARRAY( _GLTF->nodes );
        FREE_ARRAY( _GLTF->meshes );
        FREE_ARRAY( _GLTF->accessors );
        FREE_ARRAY( _GLTF->materials );
        FREE_ARRAY( _GLTF->bufferViews );
        FREE_ARRAY( _GLTF->buffers );
        FREE_ARRAY( _GLTF->samplers );
        FREE_ARRAY( _GLTF->textures );
        FREE_ARRAY( _GLTF->skins );
        FREE_ARRAY( _GLTF->images );
        FREE_ARRAY( _GLTF->cameras );
        FREE_ARRAY( _GLTF->animations );

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
        _asset->data = ( uint8_t* )realloc(
            _asset->data, ( _asset->size + YYJSON_PADDING_SIZE ) );

        __builtin_memset( ( _asset->data + _asset->size ), 0,
                          YYJSON_PADDING_SIZE );

        yyjson_doc* l_document = yyjson_read(
            ( char* )( _asset->data ), _asset->size,
            ( YYJSON_READ_STOP_WHEN_DONE | // Stop when done instead of issuing
                                           // an error if there's additional
                                           // content after a JSON document
              YYJSON_READ_INSITU // This option allows the reader to modify and
                                 // use input data to store string values. The
                                 // input data must be padded by at least
                                 // YYJSON_PADDING_SIZE bytes
              ) );

        if ( !l_document ) {
            log$transaction$query( ( logLevel_t )error, "GLTF0\n" );

            goto EXIT;
        }

        // Parse
        {
            yyjson_val* l_root = yyjson_doc_get_root( l_document );

            // Asset
            {
                const char* l_rootFieldName = "asset";

                yyjson_val* l_rootField =
                    yyjson_obj_get( l_root, l_rootFieldName );

                if ( UNLIKELY( !l_rootField ) ) {
                    log$transaction$query$format(
                        ( logLevel_t )error,
                        "Root field '%s' not found in GLTF object\n",
                        l_rootFieldName );

                    goto EXIT_DOCUMENT;
                }

                // Version
                {
                    const char* l_fieldName = "version";

                    const char* l_version = yyjson_get_str(
                        yyjson_obj_get( l_rootField, l_fieldName ) );

                    if ( UNLIKELY( !l_version ) ) {
                        log$transaction$query$format(
                            ( logLevel_t )error,
                            "Field '%s' not found in root field '%s'\n",
                            l_fieldName, l_rootFieldName );

                        goto EXIT_DOCUMENT;
                    }

                    _GLTF->asset.version = strtof( l_version, NULL );
                }

                // Generator
                {
                    const char* l_fieldName = "generator";

                    const char* l_generator = yyjson_get_str(
                        yyjson_obj_get( l_rootField, l_fieldName ) );

                    if ( l_generator ) {
                        l_generator = duplicateString( l_generator );

                    } else {
                        log$transaction$query$format(
                            ( logLevel_t )info,
                            "Field '%s' not found in root field '%s'\n",
                            l_fieldName, l_rootFieldName );
                    }
                }

                // Copyright
                {
                    const char* l_fieldName = "copyright";

                    const char* l_copyright = yyjson_get_str(
                        yyjson_obj_get( l_rootField, l_fieldName ) );

                    if ( l_copyright ) {
                        _GLTF->asset.copyright = duplicateString( l_copyright );

                    } else {
                        log$transaction$query$format(
                            ( logLevel_t )info,
                            "Field '%s' not found in root field '%s'\n",
                            l_fieldName, l_rootFieldName );
                    }
                }
            }

            // Scene
            {
                const char* l_rootFieldName = "scene";

                const uint8_t l_scene = yyjson_get_uint(
                    yyjson_obj_get( l_root, l_rootFieldName ) );

                if ( l_scene ) {
                    _GLTF->scene = l_scene;

                } else {
                    log$transaction$query$format(
                        ( logLevel_t )info,
                        "Root field '%s' not found in GLTF object\n",
                        l_rootFieldName );
                }
            }

            // Scenes
            {
                const char* l_rootFieldName = "scenes";

                yyjson_val* l_rootField =
                    yyjson_obj_get( l_root, l_rootFieldName );

                if ( UNLIKELY( !l_rootField ) ) {
                    log$transaction$query$format(
                        ( logLevel_t )error,
                        "Root field '%s' not found in GLTF object\n",
                        l_rootFieldName );

                    goto EXIT_DOCUMENT;
                }

                FOR_JSON_ARRAY( l_rootField ) {
                    struct GLTF_scene l_scene = DEFAULT_GLTF_SCENE;

                    // Name
                    {
                        const char* l_fieldName = "name";

                        const char* l_name = yyjson_get_str(
                            yyjson_obj_get( _element, l_fieldName ) );

                        if ( l_name ) {
                            l_scene.name = duplicateString( l_name );

                        } else {
                            log$transaction$query$format(
                                ( logLevel_t )info,
                                "Field '%s' not found in root field '%s'\n",
                                l_fieldName, l_rootFieldName );
                        }
                    }

                    // Nodes
                    {
                        const char* l_fieldName = "nodes";

                        yyjson_val* l_nodes =
                            yyjson_obj_get( _element, l_fieldName );

                        if ( l_nodes ) {
                            l_scene.nodes =
                                ( uint16_t* )createArray( sizeof( uint16_t ) );

                            FOR_JSON_ARRAY( l_nodes ) {
                                // TODO: Fix
                                const uint16_t t = yyjson_get_uint( _element );
                                const size_t i =
                                    insertIntoArray( &( l_scene.nodes ), t );
                                log$transaction$query$format(
                                    ( logLevel_t )info, "Field '%u' '%u'\n", t,
                                    l_scene.nodes[ i ] );
                            }

                        } else {
                            log$transaction$query$format(
                                ( logLevel_t )info,
                                "Field '%s' not found in root field '%s'\n",
                                l_fieldName, l_rootFieldName );
                        }
                    }

                    struct GLTF_scene* l_sceneAllocated =
                        ( struct GLTF_scene* )malloc(
                            sizeof( struct GLTF_scene ) );

                    __builtin_memcpy( l_sceneAllocated, &l_scene,
                                      sizeof( struct GLTF_scene ) );

                    insertIntoArray( &( _GLTF->scenes ), l_sceneAllocated );
                }
            }
        }

        // Asset
        log$transaction$query$format(
            ( logLevel_t )info,
            "\033[1;32m=== GLTF Asset Info ===\033[0m\n"
            "  \033[1;34mVersion\033[0m    : \033[1;36m%.2f\033[0m\n"
            "  \033[1;34mGenerator\033[0m  : \033[1;36m'%s'\033[0m\n"
            "  \033[1;34mCopyright\033[0m  : \033[1;36m'%s'\033[0m\n"
            "\033[1;32m========================\033[0m\n",
            ( float )_GLTF->asset.version,
            ( ( _GLTF->asset.generator ) ? ( _GLTF->asset.generator )
                                         : ( "N/A" ) ),
            ( _GLTF->asset.copyright ) ? ( _GLTF->asset.copyright )
                                       : ( "N/A" ) );

        // Scene
        log$transaction$query$format(
            ( logLevel_t )info,
            "\033[1;32m=== GLTF Scene Info ===\033[0m\n"
            "  \033[1;34mScene\033[0m    : \033[1;36m%u\033[0m\n"
            "\033[1;32m========================\033[0m\n",
            _GLTF->scene );

        // Scenes
        {
            log$transaction$query(
                ( logLevel_t )info,
                "\033[1;32m=== GLTF Scenes Info ===\033[0m\n" );

            FOR_ARRAY( void**, ( void** )( _GLTF->scenes ) ) {
                const struct GLTF_scene* l_element =
                    *( ( struct GLTF_scene** )_element );
                log$transaction$query$format(
                    ( logLevel_t )info,
                    "  \033[1;34mName\033[0m  : \033[1;36m'%s'\033[0m\n",
                    ( ( l_element->name ) ? ( l_element->name ) : ( "N/A" ) ) );

                char l_nodesAsString[ 256 ] = "";

                if ( l_element->nodes ) {
                    char* l_buffer = l_nodesAsString;

                    FOR_ARRAY( uint16_t*, l_element->nodes ) {
                        const uint16_t l_node = *_element;

                        l_buffer += sprintf( l_buffer, "%u ", l_node );
                    }

                    // Remove trailing space
                    *( l_buffer - 1 ) = '\0';
                }

                log$transaction$query$format(
                    ( logLevel_t )info,
                    "  \033[1;34mNodes\033[0m  : \033[1;36m'%s'\033[0m\n",
                    ( ( *l_nodesAsString ) ? ( l_nodesAsString )
                                           : ( "N/A" ) ) );
            }

            log$transaction$query(
                ( logLevel_t )info,
                "\033[1;32m========================\033[0m\n" );
        }

    EXIT_DOCUMENT:
        yyjson_doc_free( l_document );

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
                goto EXIT_GLTF_ASSET;
            }

            l_returnValue = GLTF_t$load$fromAsset( _GLTF, &l_GLTFAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_GLTF_ASSET;
            }

            l_returnValue = asset_t$unload( &l_GLTFAsset );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT_GLTF_ASSET;
            }
        }

        l_returnValue = true;

    EXIT_GLTF_ASSET:
        if ( UNLIKELY( !asset_t$destroy( &l_GLTFAsset ) ) ) {
            l_returnValue = false;

            goto EXIT;
        }
    }

EXIT:
    return ( l_returnValue );
}

bool GLTF_t$unload( GLTF_t* restrict _GLTF ) {
    bool l_returnValue = false;

    if ( !_GLTF ) {
        goto EXIT;
    }

    {
#if 0
        FREE_ARRAY_ELEMENTS( _GLTF->scenes );
        FREE_ARRAY_ELEMENTS( _GLTF->nodes );
        FREE_ARRAY_ELEMENTS( _GLTF->meshes );
        FREE_ARRAY_ELEMENTS( _GLTF->accessors );
        FREE_ARRAY_ELEMENTS( _GLTF->materials );
        FREE_ARRAY_ELEMENTS( _GLTF->bufferViews );
        FREE_ARRAY_ELEMENTS( _GLTF->buffers );
        FREE_ARRAY_ELEMENTS( _GLTF->samplers );
        FREE_ARRAY_ELEMENTS( _GLTF->textures );
        FREE_ARRAY_ELEMENTS( _GLTF->skins );
        FREE_ARRAY_ELEMENTS( _GLTF->images );
        FREE_ARRAY_ELEMENTS( _GLTF->cameras );
        FREE_ARRAY_ELEMENTS( _GLTF->animations );

#endif
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

#if defined( __clang__ )

#pragma clang diagnostic pop

#endif
