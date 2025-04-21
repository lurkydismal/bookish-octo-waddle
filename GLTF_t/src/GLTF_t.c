#if defined( __clang__ )

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wimplicit-function-declaration"

#endif

#include "GLTF_t.h"

#include <stdarg.h>

#include "log.h"
#include "stdfunc.h"
#include "yyjson.h"

struct bind {
    const char* fieldName;
    void* storage;
};

/**
 * |----------------|------|
 * | C Type         | Code |
 * |----------------|------|
 * | `float16_t`    | `f2` |
 * | `float`        | `f4` |
 * | `double`       | `f8` |
 * | `int8_t`       | `i1` |
 * | `uint8_t`      | `u1` |
 * | `int16_t`      | `i2` |
 * | `uint16_t`     | `u2` |
 * | `int32_t`      | `i4` |
 * | `uint32_t`     | `u4` |
 * | `int64_t`      | `i8` |
 * | `uint64_t`     | `u8` |
 * | `int128_t`     | `iA` |
 * | `uint128_t`    | `uA` |
 * | `size_t`       | `sz` |
 * | `ssize_t`      | `ss` |
 * | `char*`        | `s0` |
 * | `bool`         | `bl` | Not yet supported
 * |----------------|------|
 **/
// TODO: Error messages
static SENTINEL void GLTF_t$bind( yyjson_val* restrict _root,
                                  const char* restrict _rootFieldName,
                                  const char* restrict _format,
                                  ... ) {
    if ( UNLIKELY( !_root ) ) {
        return;
    }

    if ( UNLIKELY( !_rootFieldName ) ) {
        return;
    }

    if ( UNLIKELY( !_format ) ) {
        return;
    }

    {
        yyjson_val* l_rootField = yyjson_obj_get( _root, _rootFieldName );

        if ( UNLIKELY( !l_rootField ) ) {
            log$transaction$query$format(
                ( logLevel_t )info,
                "Root field '%s' not found in JSON object\n", _rootFieldName );

            return;
        }

        {
            va_list l_binds;

            va_start( l_binds, _format );

            const char* l_bindFormat = _format;

            while ( *l_bindFormat ) {
                // Skip spaces
                if ( UNLIKELY( __builtin_isspace( *l_bindFormat ) ) ) {
                    l_bindFormat++;

                    continue;
                }

                // Ensure we have two pairs
                FOR_RANGE( size_t, 0, 4 ) {
                    if ( UNLIKELY( !( l_bindFormat[ _index ] ) ) ) {
                        log$transaction$query$format(
                            ( logLevel_t )error,
                            "Incomplete format string near '%s'. Expected 4 "
                            "characters (2 type codes)\n",
                            l_bindFormat );

                        l_bindFormat = NULL;

                        break;
                    }
                }

                if ( UNLIKELY( !( l_bindFormat ) ) ) {
                    break;
                }

                {
                    char l_sourceType[ 2 + 1 ] = { l_bindFormat[ 0 ],
                                                   l_bindFormat[ 1 ], '\0' };
                    char l_storageType[ 2 + 1 ] = { l_bindFormat[ 2 ],
                                                    l_bindFormat[ 3 ], '\0' };

                    l_bindFormat += 4;

                    struct bind* l_bind = va_arg( l_binds, struct bind* );

                    if ( UNLIKELY( !l_bind ) ) {
                        break;
                    }

                    yyjson_val* l_field =
                        yyjson_obj_get( l_rootField, l_bind->fieldName );

                    if ( UNLIKELY( !l_field ) ) {
                        log$transaction$query$format(
                            ( logLevel_t )info,
                            "Field '%s' not found under '%s'\n",
                            l_bind->fieldName, _rootFieldName );

                        continue;
                    }

#define COMPARE_TYPES( _a, _b ) ( __builtin_strncmp( _a, _b, 2 ) == 0 )

                    if ( COMPARE_TYPES( l_sourceType, "f2" ) &&
                         COMPARE_TYPES( l_storageType, "u1" ) ) {
                        const char* l_value = yyjson_get_str( l_field );

                        if ( !l_value ) {
                            log$transaction$query$format(
                                ( logLevel_t )error,
                                "Field '%s' is not valid\n",
                                l_bind->fieldName );

                            continue;
                        }

                        const float16_t l_version = strtof( l_value, NULL );

                        const uint8_t l_versionAsUint8 =
                            convertFloat16ToDecimal( l_version );

                        *( ( uint8_t* )( l_bind->storage ) ) = l_versionAsUint8;

                    } else if ( COMPARE_TYPES( l_sourceType, "s0" ) &&
                                COMPARE_TYPES( l_storageType, "s0" ) ) {
                        const char* l_value = yyjson_get_str( l_field );

                        if ( !l_value ) {
                            log$transaction$query$format(
                                ( logLevel_t )error,
                                "Field '%s' is not valid\n",
                                l_bind->fieldName );

                            continue;
                        }

                        *( ( char** )( l_bind->storage ) ) =
                            duplicateString( l_value );

                    } else {
                        log$transaction$query$format(
                            ( logLevel_t )error,
                            "Unsupported type pair: '%s' -> '%s' for field "
                            "'%s'\n",
                            l_sourceType, l_storageType, l_bind->fieldName );
                    }

#undef COMPARE_TYPES
                }
            }

            va_end( l_binds );
        }
    }
}

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

            // Bind asset
            {
                struct bind l_versionBind = {
                    .fieldName = "version",
                    .storage = &( _GLTF->asset.version ) };
                struct bind l_generatorBind = {
                    .fieldName = "generator",
                    .storage = &( _GLTF->asset.generator ) };
                struct bind l_copyrightBind = {
                    .fieldName = "copyright",
                    .storage = &( _GLTF->asset.copyright ) };

                GLTF_t$bind( l_root, "asset", "f2u1 s0s0 s0s0", &l_versionBind,
                             &l_generatorBind, &l_copyrightBind, NULL );

                log$transaction$query$format(
                    ( logLevel_t )error, "T %u %s %s\n", _GLTF->asset.version,
                    _GLTF->asset.generator, _GLTF->asset.copyright );
            }
        }

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
