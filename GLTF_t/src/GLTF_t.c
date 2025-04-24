#include "GLTF_t.h"

#include "log.h"
#include "stdfunc.h"
#include "yyjson.h"

typedef yyjson_val document_t;
typedef yyjson_val rootField_t;
typedef yyjson_val field_t;

#define joinArrayNative( _array, _storage, _elementType, _elementFormat,   \
                         _delimeter )                                      \
    do {                                                                   \
        char* l_buffer = ( _storage );                                     \
        FOR( _elementType*, ( _array ) ) {                                 \
            l_buffer +=                                                    \
                sprintf( l_buffer, _elementFormat _delimeter, *_element ); \
        }                                                                  \
        /* Remove trailing space */                                        \
        *( l_buffer - 1 ) = '\0';                                          \
    } while ( 0 )

#define joinArray( _array, _storage, _elementType, _elementFormat,             \
                   _delimeter )                                                \
    do {                                                                       \
        if ( _array ) {                                                        \
            char* l_buffer = ( _storage );                                     \
            FOR_ARRAY( _elementType*, ( _array ) ) {                           \
                l_buffer +=                                                    \
                    sprintf( l_buffer, _elementFormat _delimeter, *_element ); \
            }                                                                  \
            /* Remove trailing space */                                        \
            *( l_buffer - 1 ) = '\0';                                          \
        }                                                                      \
    } while ( 0 )

#define FOR_JSON_ARRAY( _array ) \
    size_t _index;               \
    size_t _indexMax;            \
    yyjson_val* _element;        \
    yyjson_arr_foreach( ( _array ), _index, _indexMax, _element )

#define GLTF_t$get$rootField( _root, _rootFieldName )         \
    ( {                                                       \
        field_t* l_returnValue =                              \
            yyjson_obj_get( ( _root ), ( _rootFieldName ) );  \
        if ( UNLIKELY( !l_returnValue ) ) {                   \
            log$transaction$query$format(                     \
                ( logLevel_t )warn,                           \
                "Root field '%s' not found in GLTF object\n", \
                ( _rootFieldName ) );                         \
            l_returnValue = NULL;                             \
        }                                                     \
        ( l_returnValue );                                    \
    } )

#define GLTF_t$bind$value( _rootField, _rootFieldName, _fieldName,           \
                           _JSONParserType, _fieldType, _storage,            \
                           _convertFunction, ... )                           \
    do {                                                                     \
        const _fieldType l_fieldValue = yyjson_get_##_JSONParserType(        \
            yyjson_obj_get( ( _rootField ), ( _fieldName ) ) );              \
        if ( UNLIKELY( !l_fieldValue ) ) {                                   \
            log$transaction$query$format(                                    \
                ( logLevel_t )info,                                          \
                "Field '%s' not found in root field '%s'\n", ( _fieldName ), \
                ( _rootFieldName ) );                                        \
        } else {                                                             \
            *( _storage ) = _convertFunction( l_fieldValue, ##__VA_ARGS__ ); \
        }                                                                    \
    } while ( 0 )

#define GLTF_t$bind$value$array( _rootField, _rootFieldName, _fieldName,       \
                                 _JSONParserType, _fieldType, _storage,        \
                                 _convertFunction, ... )                       \
    do {                                                                       \
        field_t* l_fieldValue =                                                \
            yyjson_obj_get( ( _rootField ), ( _fieldName ) );                  \
        if ( !l_fieldValue ) {                                                 \
            log$transaction$query$format(                                      \
                ( logLevel_t )info,                                            \
                "Field array '%s' not found in root field '%s'\n",             \
                ( _fieldName ), ( _rootFieldName ) );                          \
        } else {                                                               \
            *_storage = ( _fieldType* )createArray( sizeof( _fieldType ) );    \
            FOR_JSON_ARRAY( l_fieldValue ) {                                   \
                preallocateArray( ( _storage ), 1 );                           \
                *arrayLastElementPointer( *_storage ) = _convertFunction(      \
                    yyjson_get_##_JSONParserType( _element ), ##__VA_ARGS__ ); \
            }                                                                  \
        }                                                                      \
    } while ( 0 )

#define GLTF_t$bind$value$array$range(                                         \
    _range, _rootField, _rootFieldName, _fieldName, _JSONParserType,           \
    _fieldType, _storage, _convertFunction, ... )                              \
    do {                                                                       \
        field_t* l_fieldValue =                                                \
            yyjson_obj_get( ( _rootField ), ( _fieldName ) );                  \
        if ( !l_fieldValue ) {                                                 \
            log$transaction$query$format(                                      \
                ( logLevel_t )info,                                            \
                "Field array '%s' with range '%zu' "                           \
                "not found in root field '%s'\n",                              \
                ( _fieldName ), ( size_t )( _range ), ( _rootFieldName ) );    \
        } else {                                                               \
            FOR_JSON_ARRAY( l_fieldValue ) {                                   \
                if ( _index == ( _range ) ) {                                  \
                    break;                                                     \
                }                                                              \
                ( *_storage )[ ( _index ) ] = _convertFunction(                \
                    yyjson_get_##_JSONParserType( _element ), ##__VA_ARGS__ ); \
            }                                                                  \
        }                                                                      \
    } while ( 0 )

GLTF_t GLTF_t$create( void ) {
    GLTF_t l_returnValue = DEFAULT_GLTF;

    {
        l_returnValue.scenes =
            ( struct GLTF_scene** )createArray( sizeof( struct GLTF_scene* ) );
        l_returnValue.nodes =
            ( struct GLTF_node** )createArray( sizeof( struct GLTF_node* ) );
        l_returnValue.meshes =
            ( struct GLTF_mesh** )createArray( sizeof( struct GLTF_mesh* ) );
        l_returnValue.accessors = ( struct GLTF_accessor** )createArray(
            sizeof( struct GLTF_accessor* ) );
        l_returnValue.materials = ( struct GLTF_material** )createArray(
            sizeof( struct GLTF_material* ) );
        l_returnValue.bufferViews = ( struct GLTF_bufferView** )createArray(
            sizeof( struct GLTF_bufferView* ) );
        l_returnValue.buffers = ( struct GLTF_buffer** )createArray(
            sizeof( struct GLTF_buffer* ) );
        l_returnValue.samplers = ( struct GLTF_sampler** )createArray(
            sizeof( struct GLTF_sampler* ) );
        l_returnValue.textures = ( struct GLTF_texture** )createArray(
            sizeof( struct GLTF_texture* ) );
        l_returnValue.skins =
            ( struct GLTF_skin** )createArray( sizeof( struct GLTF_skin* ) );
        l_returnValue.images =
            ( struct GLTF_image** )createArray( sizeof( struct GLTF_image* ) );
        l_returnValue.cameras = ( struct GLTF_camera** )createArray(
            sizeof( struct GLTF_camera* ) );
        l_returnValue.animations = ( struct GLTF_animation** )createArray(
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
        _GLTF->asset.version = 0;

        _GLTF->scene = 0;

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
            document_t* l_root = yyjson_doc_get_root( l_document );

            // Asset
            {
                const char* l_rootFieldName = "asset";

                rootField_t* l_rootField =
                    GLTF_t$get$rootField( l_root, l_rootFieldName );

                if ( !l_rootField ) {
                    goto EXIT_DOCUMENT;
                }

                // Version
                GLTF_t$bind$value( l_rootField, l_rootFieldName, "version", str,
                                   char*, &( _GLTF->asset.version ), strtof,
                                   NULL );

                // Generator
                GLTF_t$bind$value( l_rootField, l_rootFieldName, "generator",
                                   str, char*, &( _GLTF->asset.generator ),
                                   duplicateString );

                // Copyright
                GLTF_t$bind$value( l_rootField, l_rootFieldName, "copyright",
                                   str, char*, &( _GLTF->asset.copyright ),
                                   duplicateString );
            }

            // Scene
            GLTF_t$bind$value( l_root, "GLTF", "scene", uint, uint8_t,
                               &( _GLTF->scene ), ( uint8_t ));

            // Scenes
            {
                const char* l_rootFieldName = "scenes";

                rootField_t* l_rootField =
                    GLTF_t$get$rootField( l_root, l_rootFieldName );

                FOR_JSON_ARRAY( l_rootField ) {
                    struct GLTF_scene l_scene = DEFAULT_GLTF_SCENE;

                    // Name
                    GLTF_t$bind$value( _element, l_rootFieldName, "name", str,
                                       char*, &( l_scene.name ),
                                       duplicateString );

                    // Nodes
                    GLTF_t$bind$value$array( _element, l_rootFieldName, "nodes",
                                             uint, uint16_t, &( l_scene.nodes ),
                                             ( uint16_t ));

                    struct GLTF_scene* l_sceneAllocated =
                        ( struct GLTF_scene* )malloc(
                            sizeof( struct GLTF_scene ) );

                    __builtin_memcpy( l_sceneAllocated, &l_scene,
                                      sizeof( struct GLTF_scene ) );

                    insertIntoArray( &( _GLTF->scenes ), l_sceneAllocated );
                }
            }

            // Nodes
            {
                const char* l_rootFieldName = "nodes";

                rootField_t* l_rootField =
                    GLTF_t$get$rootField( l_root, l_rootFieldName );

                FOR_JSON_ARRAY( l_rootField ) {
                    struct GLTF_node l_node = DEFAULT_GLTF_NODE;

                    // Name
                    GLTF_t$bind$value( _element, l_rootFieldName, "name", str,
                                       char*, &( l_node.name ),
                                       duplicateString );

                    // Children
                    GLTF_t$bind$value$array( _element, l_rootFieldName,
                                             "children", uint, uint8_t,
                                             &( l_node.children ), ( uint8_t ));

                    // Matrix
                    GLTF_t$bind$value$array$range(
                        GLTF_NODE_MATRIX_BUFFER_SIZE, _element, l_rootFieldName,
                        "matrix", real, float, &( l_node.matrix ), ( float ));

                    // Mesh
                    GLTF_t$bind$value( l_root, l_rootFieldName, "mesh", uint,
                                       uint8_t, &( l_node.mesh ), ( uint8_t ));

                    // Weights
                    GLTF_t$bind$value$array(
                        _element, l_rootFieldName, "weights", real, float16_t,
                        &( l_node.weights ), ( float16_t ) );

                    // Translation
                    GLTF_t$bind$value$array$range(
                        3, _element, l_rootFieldName, "translation", real,
                        float16_t, &( l_node.translation ), ( float16_t ) );

                    // Scale
                    GLTF_t$bind$value$array$range(
                        3, _element, l_rootFieldName, "scale", real, float16_t,
                        &( l_node.scale ), ( float16_t ) );

                    // Rotation
                    GLTF_t$bind$value$array$range(
                        3, _element, l_rootFieldName, "rotation", real,
                        float16_t, &( l_node.rotation ), ( float16_t ) );

                    // Skin
                    GLTF_t$bind$value( l_root, l_rootFieldName, "skin", uint,
                                       uint8_t, &( l_node.skin ), ( uint8_t ));

                    // Camera
                    GLTF_t$bind$value( l_root, l_rootFieldName, "camera", uint,
                                       uint8_t, &( l_node.camera ),
                                       ( uint8_t ));

                    struct GLTF_node* l_nodeAllocated =
                        ( struct GLTF_node* )malloc(
                            sizeof( struct GLTF_node ) );

                    __builtin_memcpy( l_nodeAllocated, &l_node,
                                      sizeof( struct GLTF_node ) );

                    insertIntoArray( &( _GLTF->nodes ), l_nodeAllocated );
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

                // Nodes
                {
                    char l_nodesAsString[ 256 ];

                    joinArray( l_element->nodes, l_nodesAsString, uint16_t,
                               "%u", " " );

                    log$transaction$query$format(
                        ( logLevel_t )info,
                        "  \033[1;34mNodes\033[0m  : \033[1;36m'%s'\033[0m\n",
                        ( ( *l_nodesAsString ) ? ( l_nodesAsString )
                                               : ( "N/A" ) ) );
                }
            }

            log$transaction$query(
                ( logLevel_t )info,
                "\033[1;32m========================\033[0m\n" );
        }

        // Nodes
        {
            log$transaction$query(
                ( logLevel_t )info,
                "\033[1;32m=== GLTF Nodes Info ===\033[0m\n" );

            FOR_ARRAY( void**, ( void** )( _GLTF->nodes ) ) {
                log$transaction$query(
                    ( logLevel_t )info,
                    "\033[1;32m=== GLTF Node Info ===\033[0m\n" );

                const struct GLTF_node* l_element =
                    *( ( struct GLTF_node** )_element );
                log$transaction$query$format(
                    ( logLevel_t )info,
                    "  \033[1;34mName\033[0m  : \033[1;36m'%s'\033[0m\n",
                    ( ( l_element->name ) ? ( l_element->name ) : ( "N/A" ) ) );

                // Children
                {
                    char l_childrenAsString[ 256 ];

                    joinArray( l_element->children, l_childrenAsString, uint8_t,
                               "%u", " " );

                    log$transaction$query$format(
                        ( logLevel_t )info,
                        "  \033[1;34mChildren\033[0m  : "
                        "\033[1;36m'%s'\033[0m\n",
                        ( ( *l_childrenAsString ) ? ( l_childrenAsString )
                                                  : ( "N/A" ) ) );
                }

                // Matrix
                {
                    bool l_isNotDefault = false;
                    const float16_t
                        l_matrixDefault[ GLTF_NODE_MATRIX_BUFFER_SIZE ] =
                            DEFAULT_GLTF_NODE_MATRIX;

                    FOR_RANGE( uint8_t, 0, GLTF_NODE_MATRIX_BUFFER_SIZE ) {
                        if ( ( l_element->matrix )[ _index ] !=
                             l_matrixDefault[ _index ] ) {
                            l_isNotDefault = true;

                            break;
                        }
                    }

                    if ( l_isNotDefault ) {
                        char l_matrixAsString[ 256 ];

                        joinArrayNative( l_element->matrix, l_matrixAsString,
                                         float, "%f", " " );

                        log$transaction$query$format(
                            ( logLevel_t )info,
                            "  \033[1;34mMatrix\033[0m  : "
                            "\033[1;36m'%s'\033[0m\n",
                            l_matrixAsString );

                    } else {
                        log$transaction$query( ( logLevel_t )info,
                                               "  \033[1;34mMatrix\033[0m  : "
                                               "\033[1;36m'N/A'\033[0m\n" );
                    }
                }

                log$transaction$query$format(
                    ( logLevel_t )info,
                    "  \033[1;34mMesh\033[0m  : \033[1;36m'%u'\033[0m\n",
                    ( ( l_element->mesh ) ? ( l_element->mesh ) : ( 0 ) ) );

                // Weights
                {
                    char l_weightsAsString[ 256 ];

                    joinArray( ( float* )( l_element->weights ),
                               l_weightsAsString, float, "%f", " " );

                    log$transaction$query$format(
                        ( logLevel_t )info,
                        "  \033[1;34mWeights\033[0m  : "
                        "\033[1;36m'%s'\033[0m\n",
                        ( ( *l_weightsAsString ) ? ( l_weightsAsString )
                                                 : ( "N/A" ) ) );
                }

#if 0
                // Translation
                {
                    bool l_isNotDefault = false;
                    const float16_t l_translationDefault[ 3 ] =
                        DEFAULT_GLTF_NODE_TRANSLATION;

                    FOR_RANGE( uint8_t, 0, 3 ) {
                        if ( ( l_element->translation )[ _index ] !=
                             l_translationDefault[ _index ] ) {
                            l_isNotDefault = true;

                            break;
                        }
                    }

                    if ( l_isNotDefault ) {
                        char l_translationAsString[ 256 ];

                        joinArrayNative( l_element->matrix,
                                         l_translationAsString, float, "%f",
                                         " " );

                        log$transaction$query$format(
                            ( logLevel_t )info,
                            "  \033[1;34mTranslation\033[0m  : "
                            "\033[1;36m'%s'\033[0m\n",
                            l_translationAsString );

                    } else {
                        log$transaction$query(
                            ( logLevel_t )info,
                            "  \033[1;34mTranslation\033[0m  : "
                            "\033[1;36m'N/A'\033[0m\n" );
                    }
                }
#endif

                // Scale
                {
                }

                // Rotation
                {
                }

                // Skin
                {
                    log$transaction$query$format(
                        ( logLevel_t )info,
                        "  \033[1;34mSkin\033[0m  : \033[1;36m'%u'\033[0m\n",
                        ( ( l_element->skin ) ? ( l_element->skin ) : ( 0 ) ) );
                }

                // Camera
                {
                    log$transaction$query$format(
                        ( logLevel_t )info,
                        "  \033[1;34mCamera\033[0m  : \033[1;36m'%u'\033[0m\n",
                        ( ( l_element->camera ) ? ( l_element->camera )
                                                : ( 0 ) ) );
                }

                log$transaction$query(
                    ( logLevel_t )info,
                    "\033[1;32m========================\033[0m\n" );
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
        // Asset
        {
            free( _GLTF->asset.generator );
            free( _GLTF->asset.copyright );
        }

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

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

#if defined( __clang__ )

#pragma clang diagnostic pop

#endif
