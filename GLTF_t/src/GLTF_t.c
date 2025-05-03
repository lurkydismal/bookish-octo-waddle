#include "GLTF_t.h"

#include "log.h"
#include "stdfunc.h"
#include "yyjson.h"

#define LOG_PARSED_GLTF

typedef yyjson_doc* document_t;
typedef yyjson_val* rootField_t;
typedef yyjson_val* field_t;

#define joinArrayNative( _array, _storage, _elementFormat, _delimeter )    \
    do {                                                                   \
        char* l_buffer = ( _storage );                                     \
        FOR( const typeof( *( _array ) )*, ( _array ) ) {                  \
            l_buffer +=                                                    \
                sprintf( l_buffer, _elementFormat _delimeter, *_element ); \
        }                                                                  \
        /* Remove trailing space */                                        \
        *( l_buffer - 1 ) = '\0';                                          \
    } while ( 0 )

#define joinArrayRange( _range, _array, _storage, _elementFormat, _delimeter ) \
    do {                                                                       \
        char* l_buffer = ( _storage );                                         \
        FOR_RANGE( size_t, 0, ( _range ) ) {                                   \
            l_buffer += sprintf( l_buffer, _elementFormat _delimeter,          \
                                 ( _array )[ _index ] );                       \
        }                                                                      \
        /* Remove trailing space */                                            \
        *( l_buffer - 1 ) = '\0';                                              \
    } while ( 0 )

#define joinArray( _array, _storage, _elementFormat, _delimeter )              \
    do {                                                                       \
        if ( _array ) {                                                        \
            char* l_buffer = ( _storage );                                     \
            FOR_ARRAY( const typeof( *( _array ) )*, ( _array ) ) {            \
                l_buffer +=                                                    \
                    sprintf( l_buffer, _elementFormat _delimeter, *_element ); \
            }                                                                  \
            /* Remove trailing space */                                        \
            *( l_buffer - 1 ) = '\0';                                          \
        }                                                                      \
    } while ( 0 )

#define FOR_JSON_ARRAY( _array )                                 \
    size_t _index;                                               \
    field_t _element;                                            \
    for ( _index = 0, _element = yyjson_arr_get_first( _array ); \
          _index < yyjson_arr_size( _array );                    \
          _index++, _element = unsafe_yyjson_get_next( _element ) )

#define FOR_JSON_ARRAY_RANGE( _range, _array )                               \
    size_t _index;                                                           \
    field_t _element;                                                        \
    for ( _index = 0, _element = yyjson_arr_get_first( _array );             \
          ( ( _index < _range ) && ( _index < yyjson_arr_size( _array ) ) ); \
          _index++, _element = unsafe_yyjson_get_next( _element ) )

#define FOR_JSON_FIELD( _field )                                             \
    size_t _index;                                                           \
    field_t _elementKey, _elementValue;                                      \
    for ( _index = 0,                                                        \
          _elementKey = ( ( _field ) ? ( unsafe_yyjson_get_first( _field ) ) \
                                     : ( NULL ) ),                           \
          _elementValue = ( _elementKey + 1 );                               \
          _index < yyjson_obj_size( _field );                                \
          _index++, _elementKey = unsafe_yyjson_get_next( _elementValue ),   \
          _elementValue = ( _elementKey + 1 ) )

#if defined( DEBUG_LOG_ROOT_FIELD_NOT_FOUND )

#define LOG_ROOT_FIELD_NOT_FOUND( _rootFieldName )                             \
    do {                                                                       \
        log$transaction$query$format(                                          \
            ( logLevel_t )debug, "Root field '%s' not found in GLTF object\n", \
            ( _rootFieldName ) );                                              \
    } while ( 0 )

#else

#define LOG_ROOT_FIELD_NOT_FOUND( _rootFieldName ) \
    do {                                           \
        ( void )( sizeof( _rootFieldName ) );      \
    } while ( 0 )

#endif

#if defined( DEBUG_LOG_FIELD_NOT_FOUND )

#define LOG_FIELD_NOT_FOUND( _fieldName, _rootFieldName )                     \
    do {                                                                      \
        log$transaction$query$format(                                         \
            ( logLevel_t )debug, "Field '%s' not found in root field '%s'\n", \
            ( _fieldName ), ( _rootFieldName ) );                             \
    } while ( 0 )

#else

#define LOG_FIELD_NOT_FOUND( _fieldName, _rootFieldName ) \
    do {                                                  \
        ( void )( sizeof( _fieldName ) );                 \
        ( void )( sizeof( _rootFieldName ) );             \
    } while ( 0 )

#endif

#if defined( DEBUG_LOG_FIELD_ARRAY_RANGE_NOT_FOUND )

#define LOG_FIELD_ARRAY_RANGE_NOT_FOUND( _fieldName, _range, _rootFieldName ) \
    do {                                                                      \
        log$transaction$query$format( ( logLevel_t )debug,                    \
                                      "Field array '%s' with range '%zu' "    \
                                      "not found in root field '%s'\n",       \
                                      ( _fieldName ), ( size_t )( _range ),   \
                                      ( _rootFieldName ) );                   \
    } while ( 0 )

#else

#define LOG_FIELD_ARRAY_RANGE_NOT_FOUND( _fieldName, _range, _rootFieldName ) \
    do {                                                                      \
        ( void )( sizeof( _fieldName ) );                                     \
        ( void )( sizeof( _range ) );                                         \
        ( void )( sizeof( _rootFieldName ) );                                 \
    } while ( 0 )

#endif

#define GLTF_t$get$rootField( _root, _rootFieldName )               \
    ( {                                                             \
        rootField_t l_returnValue =                                 \
            GLTF_t$rootField$get( &( _root ), ( _rootFieldName ) ); \
        if ( UNLIKELY( !l_returnValue ) ) {                         \
            LOG_ROOT_FIELD_NOT_FOUND( _rootFieldName );             \
            l_returnValue = NULL;                                   \
        }                                                           \
        ( l_returnValue );                                          \
    } )

#define GLTF_t$get$field( _rootField, _rootFieldName, _fieldName )   \
    ( {                                                              \
        field_t l_returnValue =                                      \
            GLTF_t$field$get( &( _rootField ), ( _rootFieldName ) ); \
        if ( UNLIKELY( !l_returnValue ) ) {                          \
            LOG_FIELD_NOT_FOUND( _fieldName, _rootFieldName );       \
            l_returnValue = NULL;                                    \
        }                                                            \
        ( l_returnValue );                                           \
    } )

#define GLTF_t$bind$value( _rootField, _rootFieldName, _fieldName,           \
                           _JSONParserType, _fieldType, _storage,            \
                           _convertFunction, ... )                           \
    do {                                                                     \
        const _fieldType l_fieldValue = yyjson_get_##_JSONParserType(        \
            GLTF_t$field$get( &( _rootField ), ( _fieldName ) ) );           \
        if ( UNLIKELY( !l_fieldValue ) ) {                                   \
            LOG_FIELD_NOT_FOUND( _fieldName, _rootFieldName );               \
        } else {                                                             \
            *( _storage ) = _convertFunction( l_fieldValue, ##__VA_ARGS__ ); \
        }                                                                    \
    } while ( 0 )

#define GLTF_t$bind$value$array( _rootField, _rootFieldName, _fieldName,       \
                                 _JSONParserType, _fieldType, _storage,        \
                                 _convertFunction, ... )                       \
    do {                                                                       \
        field_t l_fieldValue =                                                 \
            GLTF_t$field$get( &( _rootField ), ( _fieldName ) );               \
        if ( !l_fieldValue ) {                                                 \
            LOG_FIELD_NOT_FOUND( _fieldName, _rootFieldName );                 \
        } else {                                                               \
            *_storage = createArray( _fieldType );                             \
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
        field_t l_fieldValue =                                                 \
            GLTF_t$field$get( &( _rootField ), ( _fieldName ) );               \
        if ( !l_fieldValue ) {                                                 \
            LOG_FIELD_ARRAY_RANGE_NOT_FOUND( _fieldName, _range,               \
                                             _rootFieldName );                 \
        } else {                                                               \
            FOR_JSON_ARRAY_RANGE( ( _range ), l_fieldValue ) {                 \
                ( *_storage )[ ( _index ) ] = _convertFunction(                \
                    yyjson_get_##_JSONParserType( _element ), ##__VA_ARGS__ ); \
            }                                                                  \
        }                                                                      \
    } while ( 0 )

static FORCE_INLINE bool GLTF_t$document$load$fromAsset(
    document_t* restrict _document,
    asset_t* restrict _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
        *_document = yyjson_read(
            ( char* )( _asset->data ), _asset->size,
            ( YYJSON_READ_STOP_WHEN_DONE | // Stop when done instead of issuing
                                           // an error if there's additional
                                           // content after a JSON document
              YYJSON_READ_INSITU // This option allows the reader to modify and
                                 // use input data to store string values. The
                                 // input data must be padded by at least
                                 // YYJSON_PADDING_SIZE bytes
              ) );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool GLTF_t$document$unload(
    document_t* restrict _document ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_document ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !*_document ) ) {
        goto EXIT;
    }

    {
        yyjson_doc_free( *_document );

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE field_t
GLTF_t$rootField$get( rootField_t* restrict _rootField,
                      const char* restrict _rootFieldName ) {
    return ( yyjson_obj_get( *_rootField, _rootFieldName ) );
}

static FORCE_INLINE field_t
GLTF_t$field$get( rootField_t* restrict _rootField,
                  const char* restrict _fieldName ) {
    return ( yyjson_obj_get( *_rootField, _fieldName ) );
}

static FORCE_INLINE field_t
GLTF_t$field$get2( rootField_t* restrict _rootField,
                   const char* _rootFieldName,
                   const char* restrict _fieldName ) {
    field_t l_returnValue = yyjson_obj_get( *_rootField, _fieldName );

    if ( !l_returnValue ) {
        FOR_JSON_FIELD( *_rootField ) {
            if ( __builtin_strcmp( yyjson_get_str( _elementKey ),
                                   _fieldName ) == 0 ) {
                l_returnValue = _elementValue;
            }
        }
    }

    if ( !l_returnValue ) {
        LOG_FIELD_NOT_FOUND( _fieldName, _rootFieldName );
    }

    return ( l_returnValue );
}

GLTF_t GLTF_t$create( void ) {
    GLTF_t l_returnValue = DEFAULT_GLTF;

    {
        l_returnValue.scenes = createArray( struct GLTF_scene* );
        l_returnValue.nodes = createArray( struct GLTF_node* );
        l_returnValue.meshes = createArray( struct GLTF_mesh* );
        l_returnValue.accessors = createArray( struct GLTF_accessor* );
        l_returnValue.materials = createArray( struct GLTF_material* );
        l_returnValue.bufferViews = createArray( struct GLTF_bufferView* );
        l_returnValue.buffers = createArray( struct GLTF_buffer* );
        l_returnValue.samplers = createArray( struct GLTF_sampler* );
        l_returnValue.textures = createArray( struct GLTF_texture* );
        l_returnValue.skins = createArray( struct GLTF_skin* );
        l_returnValue.images = createArray( struct GLTF_image* );
        l_returnValue.cameras = createArray( struct GLTF_camera* );
        l_returnValue.animations = createArray( struct GLTF_animation* );
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

        // TODO: Decide if this needs create and destroy
        document_t l_document;

        if ( UNLIKELY(
                 !GLTF_t$document$load$fromAsset( &l_document, _asset ) ) ) {
            log$transaction$query$format( ( logLevel_t )error,
                                          "Reading JSON document from the "
                                          "provided asset data of size %zu\n",
                                          _asset->size );

            goto EXIT;
        }

        // Parse
        {
            // TODO: Abstract all yyjson calls
            rootField_t l_root = yyjson_doc_get_root( l_document );

            // Asset
            {
                const char* l_rootFieldName = "asset";

                rootField_t l_rootField =
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

                rootField_t l_rootField =
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

                rootField_t l_rootField =
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
                    GLTF_t$bind$value( _element, l_rootFieldName, "mesh", uint,
                                       uint8_t, &( l_node.mesh ), ( uint8_t ));

                    // Weights
                    GLTF_t$bind$value$array( _element, l_rootFieldName,
                                             "weights", real, float,
                                             &( l_node.weights ), ( float ));

                    // Translation
                    GLTF_t$bind$value$array$range(
                        3, _element, l_rootFieldName, "translation", real,
                        float, &( l_node.translation ), ( float ));

                    // Scale
                    GLTF_t$bind$value$array$range(
                        3, _element, l_rootFieldName, "scale", real, float,
                        &( l_node.scale ), ( float ));

                    // Rotation
                    GLTF_t$bind$value$array$range(
                        3, _element, l_rootFieldName, "rotation", real, float,
                        &( l_node.rotation ), ( float ));

                    // Skin
                    GLTF_t$bind$value( _element, l_rootFieldName, "skin", uint,
                                       uint8_t, &( l_node.skin ), ( uint8_t ));

                    // Camera
                    GLTF_t$bind$value( _element, l_rootFieldName, "camera",
                                       uint, uint8_t, &( l_node.camera ),
                                       ( uint8_t ));

                    struct GLTF_node* l_nodeAllocated =
                        ( struct GLTF_node* )malloc(
                            sizeof( struct GLTF_node ) );

                    __builtin_memcpy( l_nodeAllocated, &l_node,
                                      sizeof( struct GLTF_node ) );

                    insertIntoArray( &( _GLTF->nodes ), l_nodeAllocated );
                }
            }

            // Meshes
            {
                const char* l_rootFieldName = "meshes";

                rootField_t l_rootField =
                    GLTF_t$get$rootField( l_root, l_rootFieldName );

                FOR_JSON_ARRAY( l_rootField ) {
                    struct GLTF_mesh l_mesh = DEFAULT_GLTF_MESH;

                    // Name
                    GLTF_t$bind$value( _element, l_rootFieldName, "name", str,
                                       char*, &( l_mesh.name ),
                                       duplicateString );

                    // Primitives
                    {
                        l_mesh.primitives =
                            createArray( struct GLTF_mesh_primitive* );

                        const char* l_fieldName = "primitives";

                        field_t l_field = GLTF_t$field$get2(
                            &_element, l_rootFieldName, l_fieldName );

                        if ( l_field ) {
                            FOR_JSON_ARRAY( l_field ) {
                                const char* l_rootFieldName =
                                    "meshes.primitives";

                                struct GLTF_mesh_primitive l_primitive =
                                    DEFAULT_GLTF_MESH_PRIMITIVES;

                                // Attributes
                                {
                                    const char* l_fieldName = "attributes";

                                    field_t l_field = GLTF_t$field$get2(
                                        &_element, l_rootFieldName,
                                        l_fieldName );

                                    if ( l_field ) {
                                        const char* l_rootFieldName =
                                            "meshes.primitives.attributes";

                                        struct GLTF_mesh_primitive_attributes*
                                            l_attributes =
                                                &( l_primitive.attributes );

                                        // POSITION
                                        GLTF_t$bind$value(
                                            l_field, l_rootFieldName,
                                            "POSITION", uint, uint16_t,
                                            &( l_attributes->POSITION ),
                                            ( uint16_t ));

                                        // NORMAL
                                        GLTF_t$bind$value(
                                            l_field, l_rootFieldName, "NORMAL",
                                            uint, uint16_t,
                                            &( l_attributes->NORMAL ),
                                            ( uint16_t ));

                                        // TANGENT
                                        GLTF_t$bind$value(
                                            l_field, l_rootFieldName, "TANGENT",
                                            uint, uint16_t,
                                            &( l_attributes->TANGENT ),
                                            ( uint16_t ));

                                        // TEXCOORD_0
                                        GLTF_t$bind$value(
                                            l_field, l_rootFieldName,
                                            "TEXCOORD_0", uint, uint16_t,
                                            &( l_attributes->TEXCOORD_0 ),
                                            ( uint16_t ));

                                        // TEXCOORD_1
                                        GLTF_t$bind$value(
                                            l_field, l_rootFieldName,
                                            "TEXCOORD_1", uint, uint16_t,
                                            &( l_attributes->TEXCOORD_1 ),
                                            ( uint16_t ));

                                        // JOINTS_0
                                        GLTF_t$bind$value(
                                            l_field, l_rootFieldName,
                                            "JOINTS_0", uint, uint16_t,
                                            &( l_attributes->JOINTS_0 ),
                                            ( uint16_t ));

                                        // WEIGHTS_0
                                        GLTF_t$bind$value(
                                            l_field, l_rootFieldName,
                                            "WEIGHTS_0", uint, uint16_t,
                                            &( l_attributes->WEIGHTS_0 ),
                                            ( uint16_t ));
                                    }
                                }

                                // Indices
                                GLTF_t$bind$value( _element, l_rootFieldName,
                                                   "indices", uint, uint8_t,
                                                   &( l_primitive.indices ),
                                                   ( uint8_t ));

                                // Mode
                                {
                                    enum GLTF_mesh_primitive_mode l_mode;

                                    GLTF_t$bind$value(
                                        _element, l_rootFieldName, "mode", uint,
                                        uint8_t, &l_mode,
                                        ( enum GLTF_mesh_primitive_mode ) );

                                    l_primitive.mode = l_mode;
                                }

                                // Material
                                GLTF_t$bind$value( _element, l_rootFieldName,
                                                   "material", uint, uint8_t,
                                                   &( l_primitive.material ),
                                                   ( uint8_t ));

                                // Targets
                                {
                                    l_primitive.targets = createArray(
                                        struct GLTF_mesh_primitive_target* );

                                    const char* l_fieldName = "targets";

                                    field_t l_field = GLTF_t$field$get2(
                                        &_element, l_rootFieldName,
                                        l_fieldName );

                                    if ( l_field ) {
                                        FOR_JSON_ARRAY( l_field ) {
                                            const char* l_rootFieldName =
                                                "meshes.primitives.targets";

                                            struct GLTF_mesh_primitive_target
                                                l_target =
                                                    DEFAULT_GLTF_MESH_PRIMITIVES_TARGETS;

                                            // POSITION
                                            GLTF_t$bind$value(
                                                _element, l_rootFieldName,
                                                "POSITION", uint, uint16_t,
                                                &( l_target.POSITION ),
                                                ( uint16_t ));

                                            // NORMAL
                                            GLTF_t$bind$value(
                                                _element, l_rootFieldName,
                                                "NORMAL", uint, uint16_t,
                                                &( l_target.NORMAL ),
                                                ( uint16_t ));

                                            // TANGENT
                                            GLTF_t$bind$value(
                                                _element, l_rootFieldName,
                                                "TANGENT", uint, uint16_t,
                                                &( l_target.TANGENT ),
                                                ( uint16_t ));

                                            struct GLTF_mesh_primitive_target*
                                                l_targetAllocated =
                                                    ( struct
                                                      GLTF_mesh_primitive_target* )
                                                        malloc( sizeof(
                                                            struct
                                                            GLTF_mesh_primitive_target ) );

                                            __builtin_memcpy(
                                                l_targetAllocated, &l_target,
                                                sizeof(
                                                    struct
                                                    GLTF_mesh_primitive_target ) );

                                            insertIntoArray(
                                                &( l_primitive.targets ),
                                                l_targetAllocated );
                                        }
                                    }
                                }

                                struct GLTF_mesh_primitive*
                                    l_primitiveAllocated =
                                        ( struct GLTF_mesh_primitive* )malloc(
                                            sizeof(
                                                struct GLTF_mesh_primitive ) );

                                __builtin_memcpy(
                                    l_primitiveAllocated, &l_primitive,
                                    sizeof( struct GLTF_mesh_primitive ) );

                                insertIntoArray( &( l_mesh.primitives ),
                                                 l_primitiveAllocated );
                            }
                        }
                    }

                    // Weights
                    GLTF_t$bind$value$array( _element, l_rootFieldName,
                                             "weights", real, float,
                                             &( l_mesh.weights ), ( float ));

                    struct GLTF_mesh* l_meshAllocated =
                        ( struct GLTF_mesh* )malloc(
                            sizeof( struct GLTF_mesh ) );

                    __builtin_memcpy( l_meshAllocated, &l_mesh,
                                      sizeof( struct GLTF_mesh ) );

                    insertIntoArray( &( _GLTF->meshes ), l_meshAllocated );
                }
            }

            // Accessors
            {
                const char* l_rootFieldName = "accessors";

                rootField_t l_rootField =
                    GLTF_t$get$rootField( l_root, l_rootFieldName );

                FOR_JSON_ARRAY( l_rootField ) {
                    struct GLTF_accessor l_accessor = DEFAULT_GLTF_ACCESSOR;

                    // Name
                    GLTF_t$bind$value( _element, l_rootFieldName, "name", str,
                                       char*, &( l_accessor.name ),
                                       duplicateString );

                    // BufferView
                    GLTF_t$bind$value( _element, l_rootFieldName, "bufferView",
                                       uint, uint8_t,
                                       &( l_accessor.bufferView ), ( uint8_t ));

                    // ByteOffset
                    GLTF_t$bind$value( _element, l_rootFieldName, "byteOffset",
                                       uint, size_t, &( l_accessor.byteOffset ),
                                       ( size_t ));

                    // ComponentType
                    {
                        enum GLTF_accessor_componentType l_componentType;

                        GLTF_t$bind$value(
                            _element, l_rootFieldName, "componentType", uint,
                            uint16_t, &l_componentType,
                            ( enum GLTF_accessor_componentType ) );

                        l_accessor.componentType = l_componentType;
                    }

                    // Normalized
                    GLTF_t$bind$value( _element, l_rootFieldName, "normalized",
                                       bool, bool, &( l_accessor.normalized ),
                                       ( bool ));

                    // Count
                    GLTF_t$bind$value( _element, l_rootFieldName, "count", uint,
                                       uint32_t, &( l_accessor.count ),
                                       ( uint32_t ));

                    // Type is length by specification
                    // Type
                    {
                        char* l_type = NULL;

                        GLTF_t$bind$value( _element, l_rootFieldName, "type",
                                           str, char*, &( l_type ),
                                           duplicateString );

                        l_accessor.type =
                            GLTF_t$accessor$type$fromString( l_type );
                    }

                    // Max
                    GLTF_t$bind$value$array$range(
                        l_accessor.type, _element, l_rootFieldName, "max", real,
                        float, &( l_accessor.max ), ( float ));

                    // Min
                    GLTF_t$bind$value$array$range(
                        l_accessor.type, _element, l_rootFieldName, "min", real,
                        float, &( l_accessor.min ), ( float ));

                    // Sparse
                    {
                        const char* l_fieldName = "sparse";

                        field_t l_field = GLTF_t$get$field(
                            l_rootField, l_rootFieldName, l_fieldName );

                        if ( l_field ) {
                            // Count
                            GLTF_t$bind$value(
                                l_field, l_fieldName, "count", uint, uint32_t,
                                &( l_accessor.sparse.count ), ( uint32_t ));

                            // One item
                            // Indices
                            {
                                const char* l_fieldName = "indices";

                                field_t l_indicesField = GLTF_t$get$field(
                                    l_field, l_rootFieldName, l_fieldName );

                                if ( l_indicesField ) {
                                    // BufferView
                                    GLTF_t$bind$value(
                                        l_indicesField, l_fieldName,
                                        "bufferView", uint, uint8_t,
                                        &( l_accessor.sparse.indices
                                               .bufferView ),
                                        ( uint8_t ));

                                    // ByteOffset
                                    GLTF_t$bind$value(
                                        l_indicesField, l_fieldName,
                                        "byteOffset", uint, size_t,
                                        &( l_accessor.sparse.indices
                                               .byteOffset ),
                                        ( size_t ));

                                    // ComponentType
                                    {
                                        enum GLTF_accessor_componentType
                                            l_componentType;

                                        GLTF_t$bind$value(
                                            l_indicesField, l_fieldName,
                                            "componentType", uint, uint16_t,
                                            &l_componentType,
                                            ( enum GLTF_accessor_componentType ) );

                                        l_accessor.sparse.indices
                                            .componentType = l_componentType;
                                    }
                                }
                            }

                            // One item
                            // Values
                            {
                                const char* l_fieldName = "values";

                                field_t l_valuesField = GLTF_t$get$field(
                                    l_field, l_rootFieldName, l_fieldName );

                                if ( l_valuesField ) {
                                    // BufferView
                                    GLTF_t$bind$value(
                                        l_valuesField, l_fieldName,
                                        "bufferView", uint, uint8_t,
                                        &( l_accessor.sparse.values
                                               .bufferView ),
                                        ( uint8_t ));

                                    // ByteOffset
                                    GLTF_t$bind$value(
                                        l_valuesField, l_fieldName,
                                        "byteOffset", uint, size_t,
                                        &( l_accessor.sparse.values
                                               .byteOffset ),
                                        ( size_t ));
                                }
                            }
                        }
                    }

                    struct GLTF_accessor* l_accesssorAllocated =
                        ( struct GLTF_accessor* )malloc(
                            sizeof( struct GLTF_accessor ) );

                    __builtin_memcpy( l_accesssorAllocated, &l_accessor,
                                      sizeof( struct GLTF_accessor ) );

                    insertIntoArray( &( _GLTF->accessors ),
                                     l_accesssorAllocated );
                }
            }

            // Materials
            {
                const char* l_rootFieldName = "materials";

                rootField_t l_rootField =
                    GLTF_t$get$rootField( l_root, l_rootFieldName );

                FOR_JSON_ARRAY( l_rootField ) {
                    struct GLTF_material l_material = DEFAULT_GLTF_MATERIAL;

                    // Name
                    GLTF_t$bind$value( _element, l_rootFieldName, "name", str,
                                       char*, &( l_material.name ),
                                       duplicateString );

                    // PBRMetallicRoughness
                    {
                        const char* l_fieldName = "pbrMetallicRoughness";

                        field_t l_field = GLTF_t$field$get2(
                            &_element, l_rootFieldName, l_fieldName );

                        if ( l_field ) {
                            struct GLTF_material_pbrMetallicRoughness*
                                l_pbrMetallicRoughness =
                                    &( l_material.pbrMetallicRoughness );

                            // BaseColorFactor
                            GLTF_t$bind$value$array$range(
                                4, l_field, l_rootFieldName, "baseColorFactor",
                                real, float,
                                &( l_pbrMetallicRoughness->baseColorFactor ),
                                ( float ));

                            // MetallicFactor
                            GLTF_t$bind$value(
                                l_field, l_rootFieldName, "metallicFactor",
                                real, float,
                                &( l_pbrMetallicRoughness->metallicFactor ),
                                ( float ));

                            // RoughnessFactor
                            GLTF_t$bind$value(
                                l_field, l_rootFieldName, "roughnessFactor",
                                real, float,
                                &( l_pbrMetallicRoughness->roughnessFactor ),
                                ( float ));

                            // BaseColorTexture
                            {
                                const char* l_fieldName = "baseColorTexture";

                                field_t l_field = GLTF_t$field$get2(
                                    &_element, l_rootFieldName, l_fieldName );

                                if ( l_field ) {
                                    struct GLTF_texture_info*
                                        l_baseColorTexture =
                                            &( l_pbrMetallicRoughness
                                                   ->baseColorTexture );

                                    // Index
                                    GLTF_t$bind$value(
                                        l_field, l_rootFieldName, "index", uint,
                                        uint8_t, &( l_baseColorTexture->index ),
                                        ( uint8_t ));

                                    // TexCoord
                                    GLTF_t$bind$value(
                                        l_field, l_rootFieldName, "texCoord",
                                        uint, uint32_t,
                                        &( l_baseColorTexture->texCoord ),
                                        ( uint32_t ));
                                }
                            }

                            // MetallicRoughnessTexture
                            {
                                const char* l_fieldName =
                                    "metallicRoughnessTexture";

                                field_t l_field = GLTF_t$field$get2(
                                    &_element, l_rootFieldName, l_fieldName );

                                if ( l_field ) {
                                    struct GLTF_texture_info*
                                        l_metallicRoughnessTexture =
                                            &( l_pbrMetallicRoughness
                                                   ->metallicRoughnessTexture );

                                    // Index
                                    GLTF_t$bind$value(
                                        l_field, l_rootFieldName, "index", uint,
                                        uint8_t,
                                        &( l_metallicRoughnessTexture->index ),
                                        ( uint8_t ));

                                    // TexCoord
                                    GLTF_t$bind$value(
                                        l_field, l_rootFieldName, "texCoord",
                                        uint, uint32_t,
                                        &( l_metallicRoughnessTexture
                                               ->texCoord ),
                                        ( uint32_t ));
                                }
                            }
                        }
                    }

                    // NormalTexture
                    {
                        const char* l_fieldName = "normalTexture";

                        field_t l_field = GLTF_t$field$get2(
                            &_element, l_rootFieldName, l_fieldName );

                        if ( l_field ) {
                            struct GLTF_material_normal_texture_info*
                                l_normalTexture = &( l_material.normalTexture );

                            // Index
                            GLTF_t$bind$value( l_field, l_rootFieldName,
                                               "index", uint, uint8_t,
                                               &( l_normalTexture->index ),
                                               ( uint8_t ));

                            // TexCoord
                            GLTF_t$bind$value( l_field, l_rootFieldName,
                                               "texCoord", uint, uint32_t,
                                               &( l_normalTexture->texCoord ),
                                               ( uint32_t ));

                            // Scale
                            GLTF_t$bind$value(
                                l_field, l_rootFieldName, "scale", real, float,
                                &( l_normalTexture->scale ), ( float ));
                        }
                    }

                    // OcclusionTexture
                    {
                        const char* l_fieldName = "occlusionTexture";

                        field_t l_field = GLTF_t$field$get2(
                            &_element, l_rootFieldName, l_fieldName );

                        if ( l_field ) {
                            struct GLTF_material_occlusion_texture_info*
                                l_occlusionTexture =
                                    &( l_material.occlusionTexture );

                            // Index
                            GLTF_t$bind$value( l_field, l_rootFieldName,
                                               "index", uint, uint8_t,
                                               &( l_occlusionTexture->index ),
                                               ( uint8_t ));

                            // TexCoord
                            GLTF_t$bind$value(
                                l_field, l_rootFieldName, "texCoord", uint,
                                uint32_t, &( l_occlusionTexture->texCoord ),
                                ( uint32_t ));

                            // Strength
                            GLTF_t$bind$value(
                                l_field, l_rootFieldName, "strength", real,
                                float, &( l_occlusionTexture->strength ),
                                ( float ));
                        }
                    }

                    // EmissiveTexture
                    {
                        const char* l_fieldName = "emissiveTexture";

                        field_t l_field = GLTF_t$field$get2(
                            &_element, l_rootFieldName, l_fieldName );

                        if ( l_field ) {
                            struct GLTF_texture_info* l_emissiveTexture =
                                &( l_material.emissiveTexture );

                            // Index
                            GLTF_t$bind$value( l_field, l_rootFieldName,
                                               "index", uint, uint8_t,
                                               &( l_emissiveTexture->index ),
                                               ( uint8_t ));

                            // TexCoord
                            GLTF_t$bind$value( l_field, l_rootFieldName,
                                               "texCoord", uint, uint32_t,
                                               &( l_emissiveTexture->texCoord ),
                                               ( uint32_t ));
                        }
                    }

                    // EmissiveFactor
                    GLTF_t$bind$value$array$range(
                        3, _element, l_rootFieldName, "emissiveFactor", real,
                        float, &( l_material.emissiveFactor ), ( float ));

                    // AlphaMode
                    {
                        char* l_alphaMode = NULL;

                        GLTF_t$bind$value( _element, l_rootFieldName,
                                           "alphaMode", str, char*,
                                           &( l_alphaMode ), duplicateString );

                        l_material.alphaMode =
                            GLTF_t$material$alphaMode$fromString( l_alphaMode );
                    }

                    // AlphaCutoff
                    GLTF_t$bind$value( _element, l_rootFieldName, "alphaCutoff",
                                       real, float, &( l_material.alphaCutoff ),
                                       ( float ));

                    // DoubleSided
                    GLTF_t$bind$value( _element, l_rootFieldName, "doubleSided",
                                       bool, bool, &( l_material.doubleSided ),
                                       ( bool ));

                    struct GLTF_material* l_materialAllocated =
                        ( struct GLTF_material* )malloc(
                            sizeof( struct GLTF_material ) );

                    __builtin_memcpy( l_materialAllocated, &l_material,
                                      sizeof( struct GLTF_material ) );

                    insertIntoArray( &( _GLTF->materials ),
                                     l_materialAllocated );
                }
            }

            // BufferViews
            {
                const char* l_rootFieldName = "bufferViews";

                rootField_t l_rootField =
                    GLTF_t$get$rootField( l_root, l_rootFieldName );

                FOR_JSON_ARRAY( l_rootField ) {
                    struct GLTF_bufferView l_bufferView =
                        DEFAULT_GLTF_BUFFER_VIEW;

                    // Name
                    GLTF_t$bind$value( _element, l_rootFieldName, "name", str,
                                       char*, &( l_bufferView.name ),
                                       duplicateString );

                    // Buffer
                    GLTF_t$bind$value( _element, l_rootFieldName, "buffer",
                                       uint, uint8_t, &( l_bufferView.buffer ),
                                       ( uint8_t ));

                    // ByteOffset
                    GLTF_t$bind$value(
                        _element, l_rootFieldName, "byteOffset", uint, size_t,
                        &( l_bufferView.byteOffset ), ( size_t ));

                    // ByteLength
                    GLTF_t$bind$value(
                        _element, l_rootFieldName, "byteLength", uint, size_t,
                        &( l_bufferView.byteLength ), ( size_t ));

                    // Target
                    GLTF_t$bind$value( _element, l_rootFieldName, "target",
                                       uint, uint32_t, &( l_bufferView.target ),
                                       ( uint32_t ));

                    // ByteStride
                    GLTF_t$bind$value(
                        _element, l_rootFieldName, "byteStride", uint, uint32_t,
                        &( l_bufferView.byteStride ), ( uint32_t ));

                    struct GLTF_bufferView* l_bufferViewAllocated =
                        ( struct GLTF_bufferView* )malloc(
                            sizeof( struct GLTF_bufferView ) );

                    __builtin_memcpy( l_bufferViewAllocated, &l_bufferView,
                                      sizeof( struct GLTF_bufferView ) );

                    insertIntoArray( &( _GLTF->bufferViews ),
                                     l_bufferViewAllocated );
                }
            }

            // Buffers
            {
                const char* l_rootFieldName = "buffers";

                rootField_t l_rootField =
                    GLTF_t$get$rootField( l_root, l_rootFieldName );

                FOR_JSON_ARRAY( l_rootField ) {
                    struct GLTF_buffer l_buffer = DEFAULT_GLTF_BUFFER;

                    // Name
                    GLTF_t$bind$value( _element, l_rootFieldName, "name", str,
                                       char*, &( l_buffer.name ),
                                       duplicateString );

                    // ByteLength
                    GLTF_t$bind$value( _element, l_rootFieldName, "byteLength",
                                       uint, size_t, &( l_buffer.byteLength ),
                                       ( size_t ));

                    // Uri
                    GLTF_t$bind$value( _element, l_rootFieldName, "uri", str,
                                       char*, &( l_buffer.uri ),
                                       duplicateString );

                    struct GLTF_buffer* l_bufferAllocated =
                        ( struct GLTF_buffer* )malloc(
                            sizeof( struct GLTF_buffer ) );

                    __builtin_memcpy( l_bufferAllocated, &l_buffer,
                                      sizeof( struct GLTF_buffer ) );

                    insertIntoArray( &( _GLTF->buffers ), l_bufferAllocated );
                }
            }

            // Samplers
            {
                const char* l_rootFieldName = "samplers";

                rootField_t l_rootField =
                    GLTF_t$get$rootField( l_root, l_rootFieldName );

                FOR_JSON_ARRAY( l_rootField ) {
                    struct GLTF_sampler l_sampler = DEFAULT_GLTF_SAMPLER;

                    // Name
                    GLTF_t$bind$value( _element, l_rootFieldName, "name", str,
                                       char*, &( l_sampler.name ),
                                       duplicateString );

                    // MagFilter
                    {
                        enum GLTF_sampler_filter l_magFilter;

                        GLTF_t$bind$value( _element, l_rootFieldName,
                                           "magFilter", uint, uint16_t,
                                           &l_magFilter,
                                           ( enum GLTF_sampler_filter ) );

                        l_sampler.magFilter = l_magFilter;
                    }

                    // MinFilter
                    {
                        enum GLTF_sampler_filter l_minFilter;

                        GLTF_t$bind$value( _element, l_rootFieldName,
                                           "minFilter", uint, uint16_t,
                                           &l_minFilter,
                                           ( enum GLTF_sampler_filter ) );

                        l_sampler.minFilter = l_minFilter;
                    }

                    // WrapS
                    {
                        enum GLTF_sampler_wrapping l_wrapS;

                        GLTF_t$bind$value( _element, l_rootFieldName, "wrapS",
                                           uint, uint16_t, &l_wrapS,
                                           ( enum GLTF_sampler_wrapping ) );

                        l_sampler.wrapS = l_wrapS;
                    }

                    // WrapT
                    {
                        enum GLTF_sampler_wrapping l_wrapT;

                        GLTF_t$bind$value( _element, l_rootFieldName, "wrapT",
                                           uint, uint16_t, &l_wrapT,
                                           ( enum GLTF_sampler_wrapping ) );

                        l_sampler.wrapT = l_wrapT;
                    }

                    struct GLTF_sampler* l_samplerAllocated =
                        ( struct GLTF_sampler* )malloc(
                            sizeof( struct GLTF_sampler ) );

                    __builtin_memcpy( l_samplerAllocated, &l_sampler,
                                      sizeof( struct GLTF_sampler ) );

                    insertIntoArray( &( _GLTF->samplers ), l_samplerAllocated );
                }
            }

            // Textures
            {
                const char* l_rootFieldName = "textures";

                rootField_t l_rootField =
                    GLTF_t$get$rootField( l_root, l_rootFieldName );

                FOR_JSON_ARRAY( l_rootField ) {
                    struct GLTF_texture l_texture = DEFAULT_GLTF_TEXTURE;

                    // Name
                    GLTF_t$bind$value( _element, l_rootFieldName, "name", str,
                                       char*, &( l_texture.name ),
                                       duplicateString );

                    // Sampler
                    GLTF_t$bind$value( _element, l_rootFieldName, "sampler",
                                       uint, uint8_t, &( l_texture.sampler ),
                                       ( uint8_t ));

                    // Source
                    GLTF_t$bind$value( _element, l_rootFieldName, "source",
                                       uint, uint8_t, &( l_texture.source ),
                                       ( uint8_t ));

                    struct GLTF_texture* l_textureAllocated =
                        ( struct GLTF_texture* )malloc(
                            sizeof( struct GLTF_texture ) );

                    __builtin_memcpy( l_textureAllocated, &l_texture,
                                      sizeof( struct GLTF_texture ) );

                    insertIntoArray( &( _GLTF->textures ), l_textureAllocated );
                }
            }

            // Skins
            {
                const char* l_rootFieldName = "skins";

                rootField_t l_rootField =
                    GLTF_t$get$rootField( l_root, l_rootFieldName );

                FOR_JSON_ARRAY( l_rootField ) {
                    struct GLTF_skin l_skin = DEFAULT_GLTF_SKIN;

                    // Name
                    GLTF_t$bind$value( _element, l_rootFieldName, "name", str,
                                       char*, &( l_skin.name ),
                                       duplicateString );

                    // InverseBindMatrices
                    GLTF_t$bind$value(
                        _element, l_rootFieldName, "inverseBindMatrices", uint,
                        uint8_t, &( l_skin.inverseBindMatrices ), ( uint8_t ));

                    // Skeleton
                    GLTF_t$bind$value( _element, l_rootFieldName, "skeleton",
                                       uint, uint16_t, &( l_skin.skeleton ),
                                       ( uint16_t ));

                    // Joints
                    GLTF_t$bind$value$array( _element, l_rootFieldName,
                                             "joints", uint, uint16_t,
                                             &( l_skin.joints ), ( uint16_t ));

                    struct GLTF_skin* l_skinAllocated =
                        ( struct GLTF_skin* )malloc(
                            sizeof( struct GLTF_skin ) );

                    __builtin_memcpy( l_skinAllocated, &l_skin,
                                      sizeof( struct GLTF_skin ) );

                    insertIntoArray( &( _GLTF->skins ), l_skinAllocated );
                }
            }

            // Images
            {
                const char* l_rootFieldName = "images";

                rootField_t l_rootField =
                    GLTF_t$get$rootField( l_root, l_rootFieldName );

                FOR_JSON_ARRAY( l_rootField ) {
                    struct GLTF_image l_image = DEFAULT_GLTF_IMAGE;

                    // Name
                    GLTF_t$bind$value( _element, l_rootFieldName, "name", str,
                                       char*, &( l_image.name ),
                                       duplicateString );

                    // Uri
                    GLTF_t$bind$value( _element, l_rootFieldName, "uri", str,
                                       char*, &( l_image.uri ),
                                       duplicateString );

                    // BufferView
                    GLTF_t$bind$value( _element, l_rootFieldName, "bufferView",
                                       uint, uint8_t, &( l_image.bufferView ),
                                       ( uint8_t ));

                    struct GLTF_image* l_imageAllocated =
                        ( struct GLTF_image* )malloc(
                            sizeof( struct GLTF_image ) );

                    __builtin_memcpy( l_imageAllocated, &l_image,
                                      sizeof( struct GLTF_image ) );

                    insertIntoArray( &( _GLTF->images ), l_imageAllocated );
                }
            }

            // Cameras
            {
                const char* l_rootFieldName = "cameras";

                rootField_t l_rootField =
                    GLTF_t$get$rootField( l_root, l_rootFieldName );

                FOR_JSON_ARRAY( l_rootField ) {
                    struct GLTF_camera l_camera = DEFAULT_GLTF_CAMERA;

                    // Name
                    GLTF_t$bind$value( _element, l_rootFieldName, "name", str,
                                       char*, &( l_camera.name ),
                                       duplicateString );

                    // Type
                    {
                        char* l_type = NULL;

                        GLTF_t$bind$value( _element, l_rootFieldName, "type",
                                           str, char*, &( l_type ),
                                           duplicateString );

                        l_camera.type = GLTF_t$camera$type$fromString( l_type );
                    }

                    if ( l_camera.type == ORTHOGRAPHIC ) {
                        const char* l_fieldName = "orthographic";

                        field_t l_orthographic = GLTF_t$get$field(
                            l_rootField, l_rootFieldName, l_fieldName );

                        // Xmag
                        GLTF_t$bind$value(
                            l_orthographic, l_rootFieldName, "xmag", real,
                            float, &( l_camera.orthographic.xmag ), ( float ));

                        // Ymag
                        GLTF_t$bind$value(
                            l_orthographic, l_rootFieldName, "ymag", real,
                            float, &( l_camera.orthographic.ymag ), ( float ));

                        // Zfar
                        GLTF_t$bind$value(
                            l_orthographic, l_rootFieldName, "zfar", real,
                            float, &( l_camera.orthographic.zfar ), ( float ));

                        // Znear
                        GLTF_t$bind$value(
                            l_orthographic, l_rootFieldName, "znear", real,
                            float, &( l_camera.orthographic.znear ), ( float ));

                    } else if ( l_camera.type == PERSPECTIVE ) {
                        const char* l_fieldName = "perspective";

                        field_t l_perspective = GLTF_t$get$field(
                            l_rootField, l_rootFieldName, l_fieldName );

                        // AspectRatio
                        GLTF_t$bind$value(
                            l_perspective, l_rootFieldName, "aspectRatio", real,
                            float, &( l_camera.perspective.aspectRatio ),
                            ( float ));

                        // YFOV
                        GLTF_t$bind$value(
                            l_perspective, l_rootFieldName, "yfov", real, float,
                            &( l_camera.perspective.yfov ), ( float ));

                        // Zfar
                        GLTF_t$bind$value(
                            l_perspective, l_rootFieldName, "zfar", real, float,
                            &( l_camera.perspective.zfar ), ( float ));

                        // Znear
                        GLTF_t$bind$value(
                            l_perspective, l_rootFieldName, "znear", real,
                            float, &( l_camera.perspective.znear ), ( float ));
                    }

                    struct GLTF_camera* l_cameraAllocated =
                        ( struct GLTF_camera* )malloc(
                            sizeof( struct GLTF_camera ) );

                    __builtin_memcpy( l_cameraAllocated, &l_camera,
                                      sizeof( struct GLTF_camera ) );

                    insertIntoArray( &( _GLTF->cameras ), l_cameraAllocated );
                }
            }

            // Animations
            {
                const char* l_rootFieldName = "animations";

                rootField_t l_rootField =
                    GLTF_t$get$rootField( l_root, l_rootFieldName );

                FOR_JSON_ARRAY( l_rootField ) {
                    struct GLTF_animation l_animation = DEFAULT_GLTF_ANIMATION;

                    // Name
                    GLTF_t$bind$value( _element, l_rootFieldName, "name", str,
                                       char*, &( l_animation.name ),
                                       duplicateString );

                    // Channels
                    {
                        l_animation.channels =
                            createArray( struct GLTF_animation_channel* );

                        const char* l_fieldName = "channels";

                        field_t l_field = GLTF_t$field$get2(
                            &_element, l_rootFieldName, l_fieldName );

                        if ( l_field ) {
                            FOR_JSON_ARRAY( l_field ) {
                                const char* l_rootFieldName =
                                    "animations.channels";

                                struct GLTF_animation_channel l_channel =
                                    DEFAULT_GLTF_ANIMATION_CHANNEL;

                                // Sampler
                                GLTF_t$bind$value( _element, l_rootFieldName,
                                                   "sampler", uint, uint8_t,
                                                   &( l_channel.sampler ),
                                                   ( uint8_t ));

                                // Target
                                {
                                    const char* l_fieldName = "target";

                                    field_t l_field = GLTF_t$field$get2(
                                        &_element, l_rootFieldName,
                                        l_fieldName );

                                    if ( l_field ) {
                                        const char* l_rootFieldName =
                                            "animations.channels.target";

                                        // Node
                                        GLTF_t$bind$value(
                                            l_field, l_rootFieldName, "node",
                                            uint, uint8_t,
                                            &( l_channel.target.node ),
                                            ( uint8_t ));

                                        // Path
                                        {
                                            char* l_path = NULL;

                                            GLTF_t$bind$value(
                                                l_field, l_rootFieldName,
                                                "path", str, char*, &( l_path ),
                                                duplicateString );

                                            l_channel.target.path =
                                                GLTF_t$animation$channel$target$path$fromString(
                                                    l_path );
                                        }
                                    }
                                }

                                struct GLTF_animation_channel*
                                    l_channelAllocated =
                                        ( struct GLTF_animation_channel* )
                                            malloc( sizeof(
                                                struct
                                                GLTF_animation_channel ) );

                                __builtin_memcpy(
                                    l_channelAllocated, &l_channel,
                                    sizeof( struct GLTF_animation_channel ) );

                                insertIntoArray( &( l_animation.channels ),
                                                 l_channelAllocated );
                            }
                        }
                    }

                    // Samplers
                    {
                        l_animation.samplers =
                            createArray( struct GLTF_animation_sampler* );

                        const char* l_fieldName = "samplers";

                        field_t l_field = GLTF_t$field$get2(
                            &_element, l_rootFieldName, l_fieldName );

                        if ( l_field ) {
                            FOR_JSON_ARRAY( l_field ) {
                                const char* l_rootFieldName =
                                    "animations.samplers";

                                struct GLTF_animation_sampler l_sampler =
                                    DEFAULT_GLTF_ANIMATION_SAMPLER;

                                // Input
                                GLTF_t$bind$value(
                                    _element, l_rootFieldName, "input", uint,
                                    uint8_t, &( l_sampler.input ), ( uint8_t ));

                                // Interpolation
                                {
                                    char* l_interpolation = NULL;

                                    GLTF_t$bind$value(
                                        _element, l_rootFieldName,
                                        "interpolation", str, char*,
                                        &( l_interpolation ), duplicateString );

                                    l_sampler.interpolation =
                                        GLTF_t$animation$sampler$interpolation$fromString(
                                            l_interpolation );
                                }

                                // Output
                                GLTF_t$bind$value( _element, l_rootFieldName,
                                                   "output", uint, uint8_t,
                                                   &( l_sampler.output ),
                                                   ( uint8_t ));

                                struct GLTF_animation_sampler*
                                    l_samplerAllocated =
                                        ( struct GLTF_animation_sampler* )
                                            malloc( sizeof(
                                                struct
                                                GLTF_animation_sampler ) );

                                __builtin_memcpy(
                                    l_samplerAllocated, &l_sampler,
                                    sizeof( struct GLTF_animation_sampler ) );

                                insertIntoArray( &( l_animation.samplers ),
                                                 l_samplerAllocated );
                            }
                        }
                    }

                    struct GLTF_animation* l_animationAllocated =
                        ( struct GLTF_animation* )malloc(
                            sizeof( struct GLTF_animation ) );

                    __builtin_memcpy( l_animationAllocated, &l_animation,
                                      sizeof( struct GLTF_animation ) );

                    insertIntoArray( &( _GLTF->animations ),
                                     l_animationAllocated );
                }
            }
        }

#if defined( LOG_PARSED_GLTF )

        // Log
        {
            size_t l_logBufferSize = LOG_MAX_TRANSACTION_SIZE_DEFAULT;
            char* l_logBuffer =
                ( char* )malloc( l_logBufferSize * sizeof( char ) );

#define APPEND_TO_LOG_BUFFER( _format, ... )                                \
    do {                                                                    \
        const size_t l_logBufferCurrentSize =                               \
            ( __builtin_strlen( l_logBuffer ) * sizeof( char ) );           \
        const size_t l_neededBufferSize =                                   \
            snprintf( NULL, 0, _format, ##__VA_ARGS__ );                    \
        if ( ( l_logBufferCurrentSize + l_neededBufferSize ) >=             \
             l_logBufferSize ) {                                            \
            l_logBufferSize +=                                              \
                ( ( l_neededBufferSize + 1 ) * sizeof( char ) );            \
            l_logBuffer = ( char* )realloc( l_logBuffer, l_logBufferSize ); \
        }                                                                   \
        sprintf( ( l_logBuffer + l_logBufferCurrentSize ), _format,         \
                 ##__VA_ARGS__ );                                           \
    } while ( 0 )

#define APPEND_ARRAY_TO_LOG_BUFFER( _array, _elementFormat, _format )     \
    do {                                                                  \
        char l_arrayAsString[ 256 ] = "";                                 \
        joinArray( ( _array ), l_arrayAsString, _elementFormat, " " );    \
        APPEND_TO_LOG_BUFFER(                                             \
            ( _format ),                                                  \
            ( ( *l_arrayAsString ) ? ( l_arrayAsString ) : ( "N/A" ) ) ); \
    } while ( 0 )

#define LOG_ARRAY_IF_NOT_DEFAULT( _array, _elementFormat, _defaultArray,     \
                                  _format )                                  \
    do {                                                                     \
        bool l_isNotDefault = false;                                         \
        const typeof( *( _array ) ) l_arrayDefault[] = _defaultArray;        \
        FOR_RANGE( size_t, 0, arrayLengthNative( l_arrayDefault ) ) {        \
            if ( ( typeof( *( _array ) ) )( ( _array )[ _index ] ) !=        \
                 l_arrayDefault[ _index ] ) {                                \
                l_isNotDefault = true;                                       \
                break;                                                       \
            }                                                                \
        }                                                                    \
        if ( l_isNotDefault ) {                                              \
            char l_arrayAsString[ 256 ] = "";                                \
            joinArrayNative( _array, l_arrayAsString, _elementFormat, " " ); \
            APPEND_TO_LOG_BUFFER( ( _format ), l_arrayAsString );            \
        } else {                                                             \
            APPEND_TO_LOG_BUFFER( ( _format ), "N/A" );                      \
        }                                                                    \
    } while ( 0 )

#define LOG_ARRAY_RANGE_IF_NOT_DEFAULT( _range, _array, _elementFormat,      \
                                        _defaultArray, _format )             \
    do {                                                                     \
        bool l_isNotDefault = false;                                         \
        const typeof( *( _array ) ) l_arrayDefault[] = _defaultArray;        \
        FOR_RANGE( size_t, 0, _range ) {                                     \
            if ( ( typeof( *( _array ) ) )( ( _array )[ _index ] ) !=        \
                 l_arrayDefault[ _index ] ) {                                \
                l_isNotDefault = true;                                       \
                break;                                                       \
            }                                                                \
        }                                                                    \
        if ( l_isNotDefault ) {                                              \
            char l_arrayAsString[ 256 ] = "";                                \
            joinArrayRange( _range, _array, l_arrayAsString, _elementFormat, \
                            " " );                                           \
            APPEND_TO_LOG_BUFFER( ( _format ), l_arrayAsString );            \
        } else {                                                             \
            APPEND_TO_LOG_BUFFER( ( _format ), "N/A" );                      \
        }                                                                    \
    } while ( 0 )

            // Asset
            {
                APPEND_TO_LOG_BUFFER(
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
            }

            // Scene
            {
                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m=== GLTF Scene Info ===\033[0m\n"
                    "  \033[1;34mScene\033[0m    : \033[1;36m%u\033[0m\n"
                    "\033[1;32m========================\033[0m\n",
                    _GLTF->scene );
            }

            // Scenes
            if ( arrayLength( _GLTF->scenes ) ) {
                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m=== GLTF Scenes Info ===\033[0m\n" );

                FOR_ARRAY( struct GLTF_scene* const*, _GLTF->scenes ) {
                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m=== GLTF Scene Info ===\033[0m\n" );

                    const struct GLTF_scene* l_scene = *_element;

                    // Name
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mName\033[0m  : \033[1;36m'%s'\033[0m\n",
                        ( ( l_scene->name ) ? ( l_scene->name ) : ( "N/A" ) ) );

                    // Nodes
                    APPEND_ARRAY_TO_LOG_BUFFER( l_scene->nodes, "%u",
                                                "  \033[1;34mNodes\033[0m  : "
                                                "\033[1;36m'%s'\033[0m\n" );

                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m========================\033[0m\n" );
                }

                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m========================\033[0m\n" );
            }

            // Nodes
            if ( arrayLength( _GLTF->nodes ) ) {
                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m=== GLTF Nodes Info ===\033[0m\n" );

                FOR_ARRAY( struct GLTF_node**, _GLTF->nodes ) {
                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m=== GLTF Node Info ===\033[0m\n" );

                    const struct GLTF_node* l_element = *_element;

                    // Name
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mName\033[0m  : \033[1;36m'%s'\033[0m\n",
                        ( ( l_element->name ) ? ( l_element->name )
                                              : ( "N/A" ) ) );

                    // Children
                    APPEND_ARRAY_TO_LOG_BUFFER(
                        l_element->children, "%u",
                        "  \033[1;34mChildren\033[0m  : "
                        "\033[1;36m'%s'\033[0m\n" );

                    // Matrix
                    LOG_ARRAY_IF_NOT_DEFAULT( l_element->matrix, "%f",
                                              DEFAULT_GLTF_NODE_MATRIX,
                                              "  \033[1;34mMatrix\033[0m  : "
                                              "\033[1;36m'%s'\033[0m\n" );

                    // Mesh
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mMesh\033[0m  : \033[1;36m'%u'\033[0m\n",
                        ( ( l_element->mesh ) ? ( l_element->mesh ) : ( 0 ) ) );

                    // Weights
                    APPEND_ARRAY_TO_LOG_BUFFER( l_element->weights, "%f",
                                                "  \033[1;34mWeights\033[0m  : "
                                                "\033[1;36m'%s'\033[0m\n" );

                    // Translation
                    LOG_ARRAY_IF_NOT_DEFAULT(
                        l_element->translation, "%f",
                        DEFAULT_GLTF_NODE_TRANSLATION,
                        "  \033[1;34mTranslation\033[0m  : "
                        "\033[1;36m'%s'\033[0m\n" );

                    // Scale
                    LOG_ARRAY_IF_NOT_DEFAULT( l_element->scale, "%f",
                                              DEFAULT_GLTF_NODE_SCALE,
                                              "  \033[1;34mScale\033[0m  : "
                                              "\033[1;36m'%s'\033[0m\n" );

                    // Rotation
                    LOG_ARRAY_IF_NOT_DEFAULT( l_element->rotation, "%f",
                                              DEFAULT_GLTF_NODE_ROTATION,
                                              "  \033[1;34mRotation\033[0m  : "
                                              "\033[1;36m'%s'\033[0m\n" );

                    // Skin
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mSkin\033[0m  : "
                        "\033[1;36m'%u'\033[0m\n",
                        ( ( l_element->skin ) ? ( l_element->skin ) : ( 0 ) ) );

                    // Camera
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mCamera\033[0m  : "
                        "\033[1;36m'%u'\033[0m\n",
                        ( ( l_element->camera ) ? ( l_element->camera )
                                                : ( 0 ) ) );

                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m========================\033[0m\n" );
                }

                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m========================\033[0m\n" );
            }

            // Meshes
            if ( arrayLength( _GLTF->accessors ) ) {
                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m=== GLTF Meshes Info ===\033[0m\n" );

                FOR_ARRAY( struct GLTF_mesh**, _GLTF->meshes ) {
                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m=== GLTF Mesh Info ===\033[0m\n" );

                    const struct GLTF_mesh* l_element = *_element;

                    // Name
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mName\033[0m  : \033[1;36m'%s'\033[0m\n",
                        ( ( l_element->name ) ? ( l_element->name )
                                              : ( "N/A" ) ) );

                    // Primitives
                    if ( arrayLength( l_element->primitives ) ) {
                        APPEND_TO_LOG_BUFFER(
                            "\033[1;32m=== GLTF Primitives Info ===\033[0m\n" );

                        FOR_ARRAY( struct GLTF_mesh_primitive* const*,
                                   l_element->primitives ) {
                            APPEND_TO_LOG_BUFFER(
                                "\033[1;32m=== GLTF Primitive Info "
                                "===\033[0m\n" );

                            const struct GLTF_mesh_primitive* l_primitive =
                                *_element;

                            // Attributes
                            {
                                APPEND_TO_LOG_BUFFER(
                                    "\033[1;32m=== GLTF Primitive Attributes "
                                    "Info "
                                    "===\033[0m\n" );

                                const struct GLTF_mesh_primitive_attributes
                                    l_attributes = l_primitive->attributes;

                                // POSITION
                                APPEND_TO_LOG_BUFFER(
                                    "  \033[1;34mPosition\033[0m  : "
                                    "\033[1;36m'%u'\033[0m\n",
                                    ( ( l_attributes.POSITION )
                                          ? ( l_attributes.POSITION )
                                          : ( 0 ) ) );

                                // NORMAL
                                APPEND_TO_LOG_BUFFER(
                                    "  \033[1;34mNormal\033[0m  : "
                                    "\033[1;36m'%u'\033[0m\n",
                                    ( ( l_attributes.NORMAL )
                                          ? ( l_attributes.NORMAL )
                                          : ( 0 ) ) );

                                // TANGENT
                                APPEND_TO_LOG_BUFFER(
                                    "  \033[1;34mTangent\033[0m  : "
                                    "\033[1;36m'%u'\033[0m\n",
                                    ( ( l_attributes.TANGENT )
                                          ? ( l_attributes.TANGENT )
                                          : ( 0 ) ) );

                                // TEXCOORD_0
                                APPEND_TO_LOG_BUFFER(
                                    "  \033[1;34mTexCoord_0\033[0m  : "
                                    "\033[1;36m'%u'\033[0m\n",
                                    ( ( l_attributes.TEXCOORD_0 )
                                          ? ( l_attributes.TEXCOORD_0 )
                                          : ( 0 ) ) );

                                // TEXCOORD_1
                                APPEND_TO_LOG_BUFFER(
                                    "  \033[1;34mTexCoord_1\033[0m  : "
                                    "\033[1;36m'%u'\033[0m\n",
                                    ( ( l_attributes.TEXCOORD_1 )
                                          ? ( l_attributes.TEXCOORD_1 )
                                          : ( 0 ) ) );

                                // JOINTS_0
                                APPEND_TO_LOG_BUFFER(
                                    "  \033[1;34mJoints_0\033[0m  : "
                                    "\033[1;36m'%u'\033[0m\n",
                                    ( ( l_attributes.JOINTS_0 )
                                          ? ( l_attributes.JOINTS_0 )
                                          : ( 0 ) ) );

                                // WEIGHTS_0
                                APPEND_TO_LOG_BUFFER(
                                    "  \033[1;34mWeights_0\033[0m  : "
                                    "\033[1;36m'%u'\033[0m\n",
                                    ( ( l_attributes.WEIGHTS_0 )
                                          ? ( l_attributes.WEIGHTS_0 )
                                          : ( 0 ) ) );

                                APPEND_TO_LOG_BUFFER(
                                    "\033[1;32m========================\033["
                                    "0m\n" );
                            }

                            // Indices
                            APPEND_TO_LOG_BUFFER(
                                "  \033[1;34mIndices\033[0m  : "
                                "\033[1;36m'%u'\033[0m\n",
                                ( ( l_primitive->indices )
                                      ? ( l_primitive->indices )
                                      : ( 0 ) ) );

                            // Mode
                            APPEND_TO_LOG_BUFFER(
                                "  \033[1;34mMode\033[0m  : "
                                "\033[1;36m'%s'\033[0m\n",
                                GLTF_t$mesh$primitive$mode$toString(
                                    l_primitive->mode ) );

                            // Material
                            APPEND_TO_LOG_BUFFER(
                                "  \033[1;34mMaterial\033[0m  : "
                                "\033[1;36m'%u'\033[0m\n",
                                ( ( l_primitive->material )
                                      ? ( l_primitive->material )
                                      : ( 0 ) ) );

                            // Targets
                            if ( arrayLength( l_primitive->targets ) ) {
                                APPEND_TO_LOG_BUFFER(
                                    "\033[1;32m=== GLTF Primitive Targets Info "
                                    "===\033[0m\n" );

                                FOR_ARRAY(
                                    struct GLTF_mesh_primitive_target* const*,
                                    l_primitive->targets ) {
                                    APPEND_TO_LOG_BUFFER(
                                        "\033[1;32m=== GLTF Primitive Target "
                                        "Info "
                                        "===\033[0m\n" );

                                    const struct GLTF_mesh_primitive_target*
                                        l_element = *_element;

                                    // POSITION
                                    APPEND_TO_LOG_BUFFER(
                                        "  \033[1;34mPosition\033[0m  : "
                                        "\033[1;36m'%u'\033[0m\n",
                                        ( ( l_element->POSITION )
                                              ? ( l_element->POSITION )
                                              : ( 0 ) ) );

                                    // NORMAL
                                    APPEND_TO_LOG_BUFFER(
                                        "  \033[1;34mNormal\033[0m  : "
                                        "\033[1;36m'%u'\033[0m\n",
                                        ( ( l_element->NORMAL )
                                              ? ( l_element->NORMAL )
                                              : ( 0 ) ) );

                                    // TANGENT
                                    APPEND_TO_LOG_BUFFER(
                                        "  \033[1;34mTangent\033[0m  : "
                                        "\033[1;36m'%u'\033[0m\n",
                                        ( ( l_element->TANGENT )
                                              ? ( l_element->TANGENT )
                                              : ( 0 ) ) );

                                    APPEND_TO_LOG_BUFFER(
                                        "\033[1;32m========================"
                                        "\033[0m\n" );
                                }

                                APPEND_TO_LOG_BUFFER(
                                    "\033[1;32m========================\033["
                                    "0m\n" );
                            }

                            APPEND_TO_LOG_BUFFER(
                                "\033[1;32m========================\033[0m\n" );
                        }

                        APPEND_TO_LOG_BUFFER(
                            "\033[1;32m========================\033[0m\n" );
                    }

                    // Weights
                    APPEND_ARRAY_TO_LOG_BUFFER( l_element->weights, "%f",
                                                "  \033[1;34mWeights\033[0m  : "
                                                "\033[1;36m'%s'\033[0m\n" );

                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m========================\033[0m\n" );
                }

                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m========================\033[0m\n" );
            }

            // Accessors
            if ( arrayLength( _GLTF->accessors ) ) {
                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m=== GLTF Accessors Info ===\033[0m\n" );

                FOR_ARRAY( struct GLTF_accessor**, _GLTF->accessors ) {
                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m=== GLTF Accessor Info ===\033[0m\n" );

                    const struct GLTF_accessor* l_element = *_element;

                    // Name
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mName\033[0m  : \033[1;36m'%s'\033[0m\n",
                        ( ( l_element->name ) ? ( l_element->name )
                                              : ( "N/A" ) ) );

                    // BufferView
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mBufferView\033[0m  : "
                        "\033[1;36m'%u'\033[0m\n",
                        ( ( l_element->bufferView ) ? ( l_element->bufferView )
                                                    : ( 0 ) ) );

                    // ByteOffset
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mByteOffset\033[0m  : "
                        "\033[1;36m'%zu'\033[0m\n",
                        ( ( l_element->byteOffset ) ? ( l_element->byteOffset )
                                                    : ( 0 ) ) );

                    // ComponentType
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mComponentType\033[0m  : "
                        "\033[1;36m'%s'\033[0m\n",
                        GLTF_t$accessor$componentType$toString(
                            l_element->componentType ) );

                    // Normalized
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mNormalized\033[0m  : "
                        "\033[1;36m'%s'\033[0m\n",
                        ( ( l_element->normalized ) ? ( "True" )
                                                    : ( "False" ) ) );

                    // Count
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mCount\033[0m  : \033[1;36m'%u'\033[0m\n",
                        ( ( l_element->count ) ? ( l_element->count )
                                               : ( 0 ) ) );

                    // Type
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mType\033[0m  : "
                        "\033[1;36m'%s'\033[0m\n",
                        GLTF_t$accessor$type$toString( l_element->type ) );

                    // Max
                    LOG_ARRAY_RANGE_IF_NOT_DEFAULT( l_element->type,
                                                    l_element->max, "%f",
                                                    DEFAULT_GLTF_ACCESSOR_MAX,
                                                    "  \033[1;34mMax\033[0m  : "
                                                    "\033[1;36m'%s'\033[0m\n" );

                    // Min
                    LOG_ARRAY_RANGE_IF_NOT_DEFAULT( l_element->type,
                                                    l_element->min, "%f",
                                                    DEFAULT_GLTF_ACCESSOR_MIN,
                                                    "  \033[1;34mMin\033[0m  : "
                                                    "\033[1;36m'%s'\033[0m\n" );

                    // Sparse
                    if ( l_element->sparse.count ) {
                        APPEND_TO_LOG_BUFFER(
                            "\033[1;32m=== GLTF Accessor Sparse Info "
                            "===\033[0m\n" );

                        // Count
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mCount\033[0m  : "
                            "\033[1;36m'%u'\033[0m\n",
                            ( ( l_element->sparse.count )
                                  ? ( l_element->sparse.count )
                                  : ( 0 ) ) );
                        // Indices
                        {
                            APPEND_TO_LOG_BUFFER(
                                "\033[1;32m=== GLTF Accessor Sparse Index Info "
                                "===\033[0m\n" );

                            // BufferView
                            APPEND_TO_LOG_BUFFER(
                                "  \033[1;34mBufferView\033[0m  : "
                                "\033[1;36m'%u'\033[0m\n",
                                ( ( l_element->sparse.indices.bufferView )
                                      ? ( l_element->sparse.indices.bufferView )
                                      : ( 0 ) ) );

                            // ByteOffset
                            APPEND_TO_LOG_BUFFER(
                                "  \033[1;34mByteOffset\033[0m  : "
                                "\033[1;36m'%zu'\033[0m\n",
                                ( ( l_element->sparse.indices.byteOffset )
                                      ? ( l_element->sparse.indices.byteOffset )
                                      : ( 0 ) ) );

                            // ComponentType
                            APPEND_TO_LOG_BUFFER(
                                "  \033[1;34mComponentType\033[0m  : "
                                "\033[1;36m'%s'\033[0m\n",
                                GLTF_t$accessor$componentType$toString(
                                    l_element->sparse.indices.componentType ) );

                            APPEND_TO_LOG_BUFFER(
                                "\033[1;32m========================\033[0m\n" );
                        }

                        // Values
                        {
                            APPEND_TO_LOG_BUFFER(
                                "\033[1;32m=== GLTF Accessor Sparse Value Info "
                                "===\033[0m\n" );

                            // BufferView
                            APPEND_TO_LOG_BUFFER(
                                "  \033[1;34mBufferView\033[0m  : "
                                "\033[1;36m'%u'\033[0m\n",
                                ( ( l_element->sparse.values.bufferView )
                                      ? ( l_element->sparse.values.bufferView )
                                      : ( 0 ) ) );

                            // ByteOffset
                            APPEND_TO_LOG_BUFFER(
                                "  \033[1;34mByteOffset\033[0m  : "
                                "\033[1;36m'%zu'\033[0m\n",
                                ( ( l_element->sparse.values.byteOffset )
                                      ? ( l_element->sparse.values.byteOffset )
                                      : ( 0 ) ) );

                            APPEND_TO_LOG_BUFFER(
                                "\033[1;32m========================\033[0m\n" );
                        }

                        APPEND_TO_LOG_BUFFER(
                            "\033[1;32m========================\033[0m\n" );
                    }

                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m========================\033[0m\n" );
                }

                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m========================\033[0m\n" );
            }

            // Materials
            if ( arrayLength( _GLTF->materials ) ) {
                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m=== GLTF Materials Info ===\033[0m\n" );

                FOR_ARRAY( struct GLTF_material**, _GLTF->materials ) {
                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m=== GLTF Material Info ===\033[0m\n" );

                    struct GLTF_material* l_element = *_element;

                    // Name
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mName\033[0m  : \033[1;36m'%s'\033[0m\n",
                        ( ( l_element->name ) ? ( l_element->name )
                                              : ( "N/A" ) ) );

                    // PBRMetallicRoughness
                    {
                        APPEND_TO_LOG_BUFFER(
                            "\033[1;32m=== GLTF PBR Metallic Roughness Info "
                            "===\033[0m\n" );

                        struct GLTF_material_pbrMetallicRoughness*
                            l_pbrMetallicRoughness =
                                &( l_element->pbrMetallicRoughness );

                        // BaseColorFactor
                        LOG_ARRAY_IF_NOT_DEFAULT(
                            l_pbrMetallicRoughness->baseColorFactor, "%f",
                            DEFAULT_GLTF_MATERIAL_PBR_METALLIC_ROUGHNESS_BASE_COLOR_FACTOR,
                            "  \033[1;34mBaseColorFactor\033[0m  : "
                            "\033[1;36m'%s'\033[0m\n" );

                        // MetallicFactor
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mMetallicFactor\033[0m  : "
                            "\033[1;36m'%f'\033[0m\n",
                            ( ( l_pbrMetallicRoughness->metallicFactor )
                                  ? ( l_pbrMetallicRoughness->metallicFactor )
                                  : ( 0 ) ) );

                        // RoughnessFactor
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mRoughnessFactor\033[0m  : "
                            "\033[1;36m'%f'\033[0m\n",
                            ( ( l_pbrMetallicRoughness->roughnessFactor )
                                  ? ( l_pbrMetallicRoughness->roughnessFactor )
                                  : ( 0 ) ) );

                        // BaseColorTexture
                        {
                            APPEND_TO_LOG_BUFFER(
                                "\033[1;32m=== GLTF Base Color Texture Info "
                                "===\033[0m\n" );

                            struct GLTF_texture_info* l_baseColorTexture =
                                &( l_pbrMetallicRoughness->baseColorTexture );

                            // Index
                            APPEND_TO_LOG_BUFFER(
                                "  \033[1;34mIndex\033[0m  : "
                                "\033[1;36m'%u'\033[0m\n",
                                ( ( l_baseColorTexture->index )
                                      ? ( l_baseColorTexture->index )
                                      : ( 0 ) ) );

                            // TexCoord
                            APPEND_TO_LOG_BUFFER(
                                "  \033[1;34mTexCoord\033[0m  : "
                                "\033[1;36m'%u'\033[0m\n",
                                ( ( l_baseColorTexture->texCoord )
                                      ? ( l_baseColorTexture->texCoord )
                                      : ( 0 ) ) );

                            APPEND_TO_LOG_BUFFER(
                                "\033[1;32m========================\033[0m\n" );
                        }

                        // MetallicRoughnessTexture
                        {
                            APPEND_TO_LOG_BUFFER(
                                "\033[1;32m=== GLTF Metallic Roughness Texture "
                                "Info ===\033[0m\n" );

                            struct GLTF_texture_info*
                                l_metallicRoughnessTexture =
                                    &( l_pbrMetallicRoughness
                                           ->metallicRoughnessTexture );

                            // Index
                            APPEND_TO_LOG_BUFFER(
                                "  \033[1;34mIndex\033[0m  : "
                                "\033[1;36m'%u'\033[0m\n",
                                ( ( l_metallicRoughnessTexture->index )
                                      ? ( l_metallicRoughnessTexture->index )
                                      : ( 0 ) ) );

                            // TexCoord
                            APPEND_TO_LOG_BUFFER(
                                "  \033[1;34mTexCoord\033[0m  : "
                                "\033[1;36m'%u'\033[0m\n",
                                ( ( l_metallicRoughnessTexture->texCoord )
                                      ? ( l_metallicRoughnessTexture->texCoord )
                                      : ( 0 ) ) );

                            APPEND_TO_LOG_BUFFER(
                                "\033[1;32m========================\033[0m\n" );
                        }

                        APPEND_TO_LOG_BUFFER(
                            "\033[1;32m========================\033[0m\n" );
                    }

                    // NormalTexture
                    {
                        APPEND_TO_LOG_BUFFER(
                            "\033[1;32m=== GLTF Normal Texture Info "
                            "===\033[0m\n" );

                        struct GLTF_material_normal_texture_info*
                            l_normalTexture = &( l_element->normalTexture );

                        // Index
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mIndex\033[0m  : "
                            "\033[1;36m'%u'\033[0m\n",
                            ( ( l_normalTexture->index )
                                  ? ( l_normalTexture->index )
                                  : ( 0 ) ) );

                        // TexCoord
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mTexCoord\033[0m  : "
                            "\033[1;36m'%u'\033[0m\n",
                            ( ( l_normalTexture->texCoord )
                                  ? ( l_normalTexture->texCoord )
                                  : ( 0 ) ) );

                        // Scale
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mScale\033[0m  : "
                            "\033[1;36m'%f'\033[0m\n",
                            ( ( l_normalTexture->scale )
                                  ? ( l_normalTexture->scale )
                                  : ( 0 ) ) );

                        APPEND_TO_LOG_BUFFER(
                            "\033[1;32m========================\033[0m\n" );
                    }

                    // OcclusionTexture
                    {
                        APPEND_TO_LOG_BUFFER(
                            "\033[1;32m=== GLTF Occlusion Texture Info "
                            "===\033[0m\n" );

                        struct GLTF_material_occlusion_texture_info*
                            l_occlusionTexture =
                                &( l_element->occlusionTexture );

                        // Index
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mIndex\033[0m  : "
                            "\033[1;36m'%u'\033[0m\n",
                            ( ( l_occlusionTexture->index )
                                  ? ( l_occlusionTexture->index )
                                  : ( 0 ) ) );

                        // TexCoord
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mTexCoord\033[0m  : "
                            "\033[1;36m'%u'\033[0m\n",
                            ( ( l_occlusionTexture->texCoord )
                                  ? ( l_occlusionTexture->texCoord )
                                  : ( 0 ) ) );

                        // Strength
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mStrength\033[0m  : "
                            "\033[1;36m'%f'\033[0m\n",
                            ( ( l_occlusionTexture->strength )
                                  ? ( l_occlusionTexture->strength )
                                  : ( 0 ) ) );

                        APPEND_TO_LOG_BUFFER(
                            "\033[1;32m========================\033[0m\n" );
                    }

                    // EmissiveTexture
                    {
                        APPEND_TO_LOG_BUFFER(
                            "\033[1;32m=== GLTF Emissive Texture Info "
                            "===\033[0m\n" );

                        struct GLTF_texture_info* l_emissiveTexture =
                            &( l_element->emissiveTexture );

                        // Index
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mIndex\033[0m  : "
                            "\033[1;36m'%u'\033[0m\n",
                            ( ( l_emissiveTexture->index )
                                  ? ( l_emissiveTexture->index )
                                  : ( 0 ) ) );

                        // TexCoord
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mTexCoord\033[0m  : "
                            "\033[1;36m'%u'\033[0m\n",
                            ( ( l_emissiveTexture->texCoord )
                                  ? ( l_emissiveTexture->texCoord )
                                  : ( 0 ) ) );

                        APPEND_TO_LOG_BUFFER(
                            "\033[1;32m========================\033[0m\n" );
                    }

                    // EmissiveFactor
                    LOG_ARRAY_IF_NOT_DEFAULT(
                        l_element->emissiveFactor, "%f",
                        DEFAULT_GLTF_MATERIAL_EMISSIVE_FACTOR,
                        "  \033[1;34mEmissiveFactor\033[0m  : "
                        "\033[1;36m'%s'\033[0m\n" );

                    // AlphaMode
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mAlphaMode\033[0m  : "
                        "\033[1;36m'%s'\033[0m\n",
                        GLTF_t$material$alphaMode$toString(
                            l_element->alphaMode ) );

                    // AlphaCutoff
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mAlphaCutoff\033[0m  : "
                        "\033[1;36m'%f'\033[0m\n",
                        ( ( l_element->alphaCutoff )
                              ? ( l_element->alphaCutoff )
                              : ( 0 ) ) );

                    // DoubleSided
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mDoubleSided\033[0m  : "
                        "\033[1;36m'%s'\033[0m\n",
                        ( ( l_element->doubleSided ) ? ( "True" )
                                                     : ( "False" ) ) );

                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m========================\033[0m\n" );
                }

                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m========================\033[0m\n" );
            }

            // BufferViews
            if ( arrayLength( _GLTF->bufferViews ) ) {
                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m=== GLTF Buffer Views Info ===\033[0m\n" );

                FOR_ARRAY( struct GLTF_bufferView**, _GLTF->bufferViews ) {
                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m=== GLTF Buffer View Info ===\033[0m\n" );

                    struct GLTF_bufferView* l_element = *_element;

                    // Name
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mName\033[0m  : \033[1;36m'%s'\033[0m\n",
                        ( ( l_element->name ) ? ( l_element->name )
                                              : ( "N/A" ) ) );

                    // Buffer
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mBuffer\033[0m  : \033[1;36m'%u'\033[0m\n",
                        ( ( l_element->buffer ) ? ( l_element->buffer )
                                                : ( 0 ) ) );

                    // ByteOffset
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mByteOffset\033[0m  : "
                        "\033[1;36m'%zu'\033[0m\n",
                        ( ( l_element->byteOffset ) ? ( l_element->byteOffset )
                                                    : ( 0 ) ) );

                    // ByteLength
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mByteLength\033[0m  : "
                        "\033[1;36m'%zu'\033[0m\n",
                        ( ( l_element->byteLength ) ? ( l_element->byteLength )
                                                    : ( 0 ) ) );

                    // Target
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mTarget\033[0m  : \033[1;36m'%u'\033[0m\n",
                        ( ( l_element->target ) ? ( l_element->target )
                                                : ( 0 ) ) );

                    // ByteStride
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mByteStride\033[0m  : "
                        "\033[1;36m'%u'\033[0m\n",
                        ( ( l_element->byteStride ) ? ( l_element->byteStride )
                                                    : ( 0 ) ) );

                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m========================\033[0m\n" );
                }

                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m========================\033[0m\n" );
            }

            // Buffers
            if ( arrayLength( _GLTF->buffers ) ) {
                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m=== GLTF Buffers Info ===\033[0m\n" );

                FOR_ARRAY( struct GLTF_buffer**, _GLTF->buffers ) {
                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m=== GLTF Buffer Info ===\033[0m\n" );

                    struct GLTF_buffer* l_element = *_element;

                    // Name
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mName\033[0m  : \033[1;36m'%s'\033[0m\n",
                        ( ( l_element->name ) ? ( l_element->name )
                                              : ( "N/A" ) ) );

                    // ByteLength
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mByteLength\033[0m  : "
                        "\033[1;36m'%zu'\033[0m\n",
                        ( ( l_element->byteLength ) ? ( l_element->byteLength )
                                                    : ( 0 ) ) );

                    // Uri
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mUri\033[0m  : \033[1;36m'%s'\033[0m\n",
                        ( ( l_element->uri ) ? ( l_element->uri )
                                             : ( "N/A" ) ) );

                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m========================\033[0m\n" );
                }

                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m========================\033[0m\n" );
            }

            // Samplers
            if ( arrayLength( _GLTF->samplers ) ) {
                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m=== GLTF Samplers Info ===\033[0m\n" );

                FOR_ARRAY( struct GLTF_sampler**, _GLTF->samplers ) {
                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m=== GLTF Sampler Info ===\033[0m\n" );

                    struct GLTF_sampler* l_element = *_element;

                    // Name
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mName\033[0m  : \033[1;36m'%s'\033[0m\n",
                        ( ( l_element->name ) ? ( l_element->name )
                                              : ( "N/A" ) ) );

                    // MagFilter
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mMagFilter\033[0m  : "
                        "\033[1;36m'%s'\033[0m\n",
                        GLTF_t$sampler$filter$toString(
                            l_element->magFilter ) );

                    // MinFilter
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mMinFilter\033[0m  : "
                        "\033[1;36m'%s'\033[0m\n",
                        GLTF_t$sampler$filter$toString(
                            l_element->minFilter ) );

                    // WrapS
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mWrapS\033[0m  : "
                        "\033[1;36m'%s'\033[0m\n",
                        GLTF_t$sampler$wrapping$toString( l_element->wrapS ) );

                    // WrapT
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mWrapT\033[0m  : "
                        "\033[1;36m'%s'\033[0m\n",
                        GLTF_t$sampler$wrapping$toString( l_element->wrapT ) );

                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m========================\033[0m\n" );
                }

                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m========================\033[0m\n" );
            }

            // Textures
            if ( arrayLength( _GLTF->textures ) ) {
                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m=== GLTF Textures Info ===\033[0m\n" );

                FOR_ARRAY( struct GLTF_texture**, _GLTF->textures ) {
                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m=== GLTF Texture Info ===\033[0m\n" );

                    struct GLTF_texture* l_element = *_element;

                    // Name
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mName\033[0m  : \033[1;36m'%s'\033[0m\n",
                        ( ( l_element->name ) ? ( l_element->name )
                                              : ( "N/A" ) ) );

                    // Sampler
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mSampler\033[0m  : "
                        "\033[1;36m'%u'\033[0m\n",
                        ( ( l_element->sampler ) ? ( l_element->sampler )
                                                 : ( 0 ) ) );

                    // Source
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mSource\033[0m  : "
                        "\033[1;36m'%u'\033[0m\n",
                        ( ( l_element->source ) ? ( l_element->source )
                                                : ( 0 ) ) );

                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m========================\033[0m\n" );
                }

                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m========================\033[0m\n" );
            }

            // Skins
            if ( arrayLength( _GLTF->skins ) ) {
                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m=== GLTF Skins Info ===\033[0m\n" );

                FOR_ARRAY( struct GLTF_skin**, _GLTF->skins ) {
                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m=== GLTF Skin Info ===\033[0m\n" );

                    struct GLTF_skin* l_element = *_element;

                    // Name
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mName\033[0m  : \033[1;36m'%s'\033[0m\n",
                        ( ( l_element->name ) ? ( l_element->name )
                                              : ( "N/A" ) ) );

                    // InverseBindMatrices
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mInverseBindMatrices\033[0m  : "
                        "\033[1;36m'%u'\033[0m\n",
                        ( ( l_element->inverseBindMatrices )
                              ? ( l_element->inverseBindMatrices )
                              : ( 0 ) ) );

                    // Skeleton
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mSkeleton\033[0m  : "
                        "\033[1;36m'%u'\033[0m\n",
                        ( ( l_element->skeleton ) ? ( l_element->skeleton )
                                                  : ( 0 ) ) );

                    // Joints
                    APPEND_ARRAY_TO_LOG_BUFFER( l_element->joints, "%u",
                                                "  \033[1;34mJoints\033[0m  : "
                                                "\033[1;36m'%s'\033[0m\n" );

                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m========================\033[0m\n" );
                }

                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m========================\033[0m\n" );
            }

            // Images
            if ( arrayLength( _GLTF->images ) ) {
                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m=== GLTF Images Info ===\033[0m\n" );

                FOR_ARRAY( struct GLTF_image**, _GLTF->images ) {
                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m=== GLTF Image Info ===\033[0m\n" );

                    struct GLTF_image* l_element = *_element;

                    // Name
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mName\033[0m  : \033[1;36m'%s'\033[0m\n",
                        ( ( l_element->name ) ? ( l_element->name )
                                              : ( "N/A" ) ) );

                    // Uri
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mUri\033[0m  : \033[1;36m'%s'\033[0m\n",
                        ( ( l_element->uri ) ? ( l_element->uri )
                                             : ( "N/A" ) ) );

                    // BufferView
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mBufferView\033[0m  : "
                        "\033[1;36m'%u'\033[0m\n",
                        ( ( l_element->bufferView ) ? ( l_element->bufferView )
                                                    : ( 0 ) ) );

                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m========================\033[0m\n" );
                }

                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m========================\033[0m\n" );
            }

            // Cameras
            if ( arrayLength( _GLTF->cameras ) ) {
                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m=== GLTF Cameras Info ===\033[0m\n" );

                FOR_ARRAY( struct GLTF_camera**, _GLTF->cameras ) {
                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m=== GLTF Camera Info ===\033[0m\n" );

                    struct GLTF_camera* l_element = *_element;

                    // Name
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mName\033[0m  : \033[1;36m'%s'\033[0m\n",
                        ( ( l_element->name ) ? ( l_element->name )
                                              : ( "N/A" ) ) );

                    // Type
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mType\033[0m  : "
                        "\033[1;36m'%s'\033[0m\n",
                        GLTF_t$camera$type$toString( l_element->type ) );

                    // Orthographic
                    if ( l_element->type == ORTHOGRAPHIC ) {
                        APPEND_TO_LOG_BUFFER(
                            "\033[1;32m=== GLTF Camera Orthographic Info "
                            "===\033[0m\n" );

                        // Xmag
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mXmag\033[0m  : "
                            "\033[1;36m'%f'\033[0m\n",
                            ( ( l_element->orthographic.xmag )
                                  ? ( l_element->orthographic.xmag )
                                  : ( 0 ) ) );

                        // Ymag
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mYmag\033[0m  : "
                            "\033[1;36m'%f'\033[0m\n",
                            ( ( l_element->orthographic.ymag )
                                  ? ( l_element->orthographic.ymag )
                                  : ( 0 ) ) );

                        // Zfar
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mZfar\033[0m  : "
                            "\033[1;36m'%f'\033[0m\n",
                            ( ( l_element->orthographic.zfar )
                                  ? ( l_element->orthographic.zfar )
                                  : ( 0 ) ) );

                        // Znear
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mZnear\033[0m  : "
                            "\033[1;36m'%f'\033[0m\n",
                            ( ( l_element->orthographic.znear )
                                  ? ( l_element->orthographic.znear )
                                  : ( 0 ) ) );

                        APPEND_TO_LOG_BUFFER(
                            "\033[1;32m========================\033[0m\n" );
                    }

                    // Perspective
                    if ( l_element->type == PERSPECTIVE ) {
                        APPEND_TO_LOG_BUFFER(
                            "\033[1;32m=== GLTF Camera Perspective Info "
                            "===\033[0m\n" );

                        // AspectRatio
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mAspectRation\033[0m  : "
                            "\033[1;36m'%f'\033[0m\n",
                            ( ( l_element->perspective.aspectRatio )
                                  ? ( l_element->perspective.aspectRatio )
                                  : ( 0 ) ) );

                        // YFOV
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mYFOV\033[0m  : "
                            "\033[1;36m'%f'\033[0m\n",
                            ( ( l_element->perspective.yfov )
                                  ? ( l_element->perspective.yfov )
                                  : ( 0 ) ) );

                        // Zfar
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mZfar\033[0m  : "
                            "\033[1;36m'%f'\033[0m\n",
                            ( ( l_element->perspective.zfar )
                                  ? ( l_element->perspective.zfar )
                                  : ( 0 ) ) );

                        // Znear
                        APPEND_TO_LOG_BUFFER(
                            "  \033[1;34mZnear\033[0m  : "
                            "\033[1;36m'%f'\033[0m\n",
                            ( ( l_element->perspective.znear )
                                  ? ( l_element->perspective.znear )
                                  : ( 0 ) ) );

                        APPEND_TO_LOG_BUFFER(
                            "\033[1;32m========================\033[0m\n" );
                    }

                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m========================\033[0m\n" );
                }

                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m========================\033[0m\n" );
            }

            // Animations
            if ( arrayLength( _GLTF->animations ) ) {
                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m=== GLTF Animations Info ===\033[0m\n" );

                FOR_ARRAY( struct GLTF_animation**, _GLTF->animations ) {
                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m=== GLTF Animation Info ===\033[0m\n" );

                    struct GLTF_animation* l_element = *_element;

                    // Name
                    APPEND_TO_LOG_BUFFER(
                        "  \033[1;34mName\033[0m  : \033[1;36m'%s'\033[0m\n",
                        ( ( l_element->name ) ? ( l_element->name )
                                              : ( "N/A" ) ) );

                    // Channels
                    if ( arrayLength( l_element->channels ) ) {
                        FOR_ARRAY( struct GLTF_animation_channel**,
                                   l_element->channels ) {
                            APPEND_TO_LOG_BUFFER(
                                "\033[1;32m=== GLTF Animation Channel Info "
                                "===\033[0m\n" );

                            // Sampler
                            APPEND_TO_LOG_BUFFER(
                                "  \033[1;34mSampler\033[0m  : "
                                "\033[1;36m'%u'\033[0m\n",
                                ( ( ( *_element )->sampler )
                                      ? ( ( *_element )->sampler )
                                      : ( 0 ) ) );

                            // Target
                            {
                                APPEND_TO_LOG_BUFFER(
                                    "\033[1;32m=== GLTF Animation Channel "
                                    "Target Info ===\033[0m\n" );

                                // Node
                                APPEND_TO_LOG_BUFFER(
                                    "  \033[1;34mNode\033[0m  : "
                                    "\033[1;36m'%u'\033[0m\n",
                                    ( ( ( *_element )->target.node )
                                          ? ( ( *_element )->target.node )
                                          : ( 0 ) ) );

                                // Path
                                APPEND_TO_LOG_BUFFER(
                                    "  \033[1;34mPath\033[0m  : "
                                    "\033[1;36m'%s'\033[0m\n",
                                    GLTF_t$animation$channel$target$path$toString(
                                        ( *_element )->target.path ) );

                                APPEND_TO_LOG_BUFFER(
                                    "\033[1;32m========================\033["
                                    "0m\n" );
                            }

                            APPEND_TO_LOG_BUFFER(
                                "\033[1;32m========================\033[0m\n" );
                        }
                    }

                    // Samplers
                    if ( arrayLength( l_element->samplers ) ) {
                        FOR_ARRAY( struct GLTF_animation_sampler**,
                                   l_element->samplers ) {
                            APPEND_TO_LOG_BUFFER(
                                "\033[1;32m=== GLTF Animation Sampler Info "
                                "===\033[0m\n" );

                            // Input
                            APPEND_TO_LOG_BUFFER(
                                "  \033[1;34mInput\033[0m  : "
                                "\033[1;36m'%u'\033[0m\n",
                                ( ( ( *_element )->input )
                                      ? ( ( *_element )->input )
                                      : ( 0 ) ) );

                            // Interpolation
                            APPEND_TO_LOG_BUFFER(
                                "  \033[1;34mInterpolation\033[0m  : "
                                "\033[1;36m'%s'\033[0m\n",
                                GLTF_t$animation$sampler$interpolation$toString(
                                    ( *_element )->interpolation ) );

                            // Output
                            APPEND_TO_LOG_BUFFER(
                                "  \033[1;34mOutput\033[0m  : "
                                "\033[1;36m'%u'\033[0m\n",
                                ( ( ( *_element )->output )
                                      ? ( ( *_element )->output )
                                      : ( 0 ) ) );

                            APPEND_TO_LOG_BUFFER(
                                "\033[1;32m========================\033[0m\n" );
                        }
                    }

                    APPEND_TO_LOG_BUFFER(
                        "\033[1;32m========================\033[0m\n" );
                }

                APPEND_TO_LOG_BUFFER(
                    "\033[1;32m========================\033[0m\n" );
            }

#undef LOG_ARRAY_IF_NOT_DEFAULT
#undef APPEND_ARRAY_TO_LOG_BUFFER
#undef APPEND_TO_LOG_BUFFER

            log$transaction$query$format( ( logLevel_t )debug, "%s",
                                          l_logBuffer );
        }

#endif

    EXIT_DOCUMENT:
        GLTF_t$document$unload( &l_document );

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
