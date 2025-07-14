#include "HUD_t.h"

#include "log.h"
#include "stdfunc.h"

#if defined( HOT_RELOAD )

#include "applicationState_t.h"

#endif

HUD_t HUD_t$create( void ) {
    HUD_t l_returnValue = DEFAULT_HUD;

    {
        l_returnValue.guardBars = createArray( object_t* );
        l_returnValue.brokenGuardBars = createArray( object_t* );
        l_returnValue.guardGauges = createArray( object_t* );
        l_returnValue.restorableHealthBars = createArray( object_t* );
        l_returnValue.healthBars = createArray( object_t* );
        l_returnValue.healthGauges = createArray( object_t* );
        l_returnValue.logos = createArray( object_t* );
        l_returnValue.names = createArray( object_t* );
        l_returnValue.meterBars = createArray( object_t* );
        l_returnValue.meterGauges = createArray( object_t* );

        l_returnValue.timerBackground = object_t$create();
        l_returnValue.timer = object_t$create();

#if defined( DEBUG )

        l_returnValue.watches = createArray( watch_t* );

#endif

        l_returnValue.players = createArray( player_t* );
    }

    return ( l_returnValue );
}

bool HUD_t$destroy( HUD_t* restrict _HUD ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_HUD ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        FREE_ARRAY( _HUD->guardBars );
        _HUD->guardBars = NULL;

        FREE_ARRAY( _HUD->brokenGuardBars );
        _HUD->brokenGuardBars = NULL;

        FREE_ARRAY( _HUD->guardGauges );
        _HUD->guardGauges = NULL;

        FREE_ARRAY( _HUD->restorableHealthBars );
        _HUD->restorableHealthBars = NULL;

        FREE_ARRAY( _HUD->healthBars );
        _HUD->healthBars = NULL;

        FREE_ARRAY( _HUD->healthGauges );
        _HUD->healthGauges = NULL;

        FREE_ARRAY( _HUD->logos );
        _HUD->logos = NULL;

        FREE_ARRAY( _HUD->names );
        _HUD->names = NULL;

        FREE_ARRAY( _HUD->meterBars );
        _HUD->meterBars = NULL;

        FREE_ARRAY( _HUD->meterGauges );
        _HUD->meterGauges = NULL;

#define TRY_DESTROY_OR_EXIT( _field )                          \
    do {                                                       \
        l_returnValue = object_t$destroy( &( _HUD->_field ) ); \
        if ( UNLIKELY( !l_returnValue ) ) {                    \
            log$transaction$query( ( logLevel_t )error,        \
                                   "Destroying " #_field );    \
            goto EXIT;                                         \
        }                                                      \
    } while ( 0 )

        TRY_DESTROY_OR_EXIT( timerBackground );
        TRY_DESTROY_OR_EXIT( timer );

#undef TRY_DESTROY_OR_EXIT

        if ( LIKELY( _HUD->name ) ) {
            free( _HUD->name );
            _HUD->name = NULL;
        }

        if ( LIKELY( _HUD->folder ) ) {
            free( _HUD->folder );
            _HUD->folder = NULL;
        }

        if ( LIKELY( _HUD->extension ) ) {
            free( _HUD->extension );
            _HUD->extension = NULL;
        }

        FREE_ARRAY( _HUD->players );
        _HUD->players = NULL;

#if defined( DEBUG )

        FREE_ARRAY( _HUD->watches );

#endif

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool HUD_t$element$load$one(
    object_t* restrict _element,
    HUD_t* restrict _HUD,
    SDL_Renderer* _renderer,
    const char* restrict _fieldName ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_element ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_HUD ) || UNLIKELY( !_HUD->name ) ||
         UNLIKELY( !_HUD->folder ) || UNLIKELY( !_HUD->extension ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_fieldName ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        {
            char* l_folder = _HUD->folder;

            char* l_boxesGlbb = NULL;

            // Boxes
            // folder/folder.boxes
            {
                l_boxesGlbb = duplicateString( _fieldName );

                concatBeforeAndAfterString( &l_boxesGlbb, "/",
                                            "." BOXES_FILE_EXTENSION );
                concatBeforeAndAfterString( &l_boxesGlbb, l_folder, NULL );
            }

            char* l_animationGlob = NULL;

            // Animation
            // folder/folder*.extension
            {
                l_animationGlob = duplicateString( "_*." );

                concatBeforeAndAfterString( &l_animationGlob, _fieldName,
                                            _HUD->extension );
                concatBeforeAndAfterString( &l_animationGlob, "/", NULL );
                concatBeforeAndAfterString( &l_animationGlob, l_folder, NULL );
            }

            l_returnValue =
                object_t$state$add$fromGlob( _element, _renderer, l_boxesGlbb,
                                             l_animationGlob, "", false, true );

            free( l_boxesGlbb );
            free( l_animationGlob );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Adding HUD element state from glob" );

                goto EXIT;
            }
        }

        // Default to the first state
        _element->currentState = arrayFirstElement( _element->states );

        // World coordinates is the same as camera coordinates for HUD element
        // Define world/ camera coordinates
        {
            SDL_FRect** l_boxesKeyFrames = boxes_t$currentKeyFrames$get(
                &( _element->currentState->boxes ) );

            if ( UNLIKELY( !l_boxesKeyFrames ) ||
                 UNLIKELY( !arrayLength( l_boxesKeyFrames ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Assigning HUD element position" );

                goto EXIT_ASSIGN;
            }

            const SDL_FRect* l_boxesKeyFrame =
                arrayFirstElement( l_boxesKeyFrames );

            const SDL_FRect* l_elementRectangle =
                animation_t$currentTargetRectangle$get(
                    &( _element->currentState->animation ) );

            _element->worldX = l_boxesKeyFrame->x;
            _element->worldXMin = l_boxesKeyFrame->x;
            _element->worldXMax = ( _HUD->logicalWidth - l_elementRectangle->w -
                                    l_boxesKeyFrame->x );

            _element->worldY = l_boxesKeyFrame->y;
            _element->worldYMin = l_boxesKeyFrame->y;
            _element->worldYMax =
                ( _HUD->logicalHeight - l_elementRectangle->h -
                  l_boxesKeyFrame->y );

        EXIT_ASSIGN:
            FREE_ARRAY( l_boxesKeyFrames );

            if ( UNLIKELY( !l_returnValue ) ) {
                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool HUD_t$reload( void* restrict _context,
                                       const char* restrict _fileName,
                                       size_t _eventMask,
                                       uint32_t _cookie ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_context ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_fileName ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
#if defined( LOG_WATCH )

        log$transaction$query$format(
            ( logLevel_t )info, "HUD watch: file [ '%s' : '%#zx' : '%u' ]",
            _fileName, _eventMask, _cookie );

#else

        UNUSED( _eventMask );
        UNUSED( _cookie );

#endif

        HUD_t* l_HUD = ( HUD_t* )_context;

        bool l_isAnimationFrame = false;
        bool l_isBoxes = false;

        {
            const ssize_t l_fileExtensionStartIndex =
                findLastSymbolInString( _fileName, '.' );

            l_returnValue = ( l_fileExtensionStartIndex != -1 );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )warn,
                                       "No file extension" );

                l_returnValue = true;

                goto EXIT;
            }

            const char* l_fileExtension =
                ( _fileName + l_fileExtensionStartIndex + 1 );

            if ( __builtin_strcmp( l_fileExtension, l_HUD->extension ) == 0 ) {
                l_isAnimationFrame = true;

            } else if ( __builtin_strcmp( l_fileExtension,
                                          BOXES_FILE_EXTENSION ) == 0 ) {
                char l_filePath[ PATH_MAX ] = { '\0' };

                {
                    char* l_pointer = l_filePath;

                    l_pointer = __builtin_stpcpy( l_pointer, l_HUD->folder );
                    l_pointer = __builtin_stpcpy( l_pointer, "/" );
                    __builtin_stpcpy( l_pointer, _fileName );
                }

                if ( doesPathExist( l_filePath ) ) {
                    l_isBoxes = true;
                }
            }
        }

        if ( l_isAnimationFrame || l_isBoxes ) {
            if ( findSymbolInString( _fileName, '.' ) !=
                 findLastSymbolInString( _fileName, '.' ) ) {
                goto EXIT;
            }

#define TRY_LOAD_MANY_OR_EXIT( _field )                                        \
    do {                                                                       \
        const size_t l_##_field##Length = __builtin_strlen( #_field );         \
        if ( ( __builtin_strncmp( _fileName, #_field, l_##_field##Length ) ==  \
               0 ) &&                                                          \
             ( _fileName[ l_##_field##Length ] ==                              \
               ( ( l_isAnimationFrame ) ? ( '_' ) : ( '.' ) ) ) ) {            \
            FOR_ARRAY( object_t* const*, ( l_HUD->_field ) ) {                 \
                state_t* l_state = arrayFirstElement( ( *_element )->states ); \
                SDL_Renderer* l_renderer = l_state->renderer;                  \
                l_returnValue = object_t$states$remove( *_element );           \
                if ( UNLIKELY( !l_returnValue ) ) {                            \
                    log$transaction$query( ( logLevel_t )error,                \
                                           "Removing HUD " #_field             \
                                           " states" );                        \
                    goto EXIT;                                                 \
                }                                                              \
                l_returnValue = HUD_t$element$load$one( *_element, l_HUD,      \
                                                        l_renderer, #_field ); \
                if ( UNLIKELY( !l_returnValue ) ) {                            \
                    log$transaction$query( ( logLevel_t )error,                \
                                           "Loading HUD " #_field );           \
                    goto EXIT;                                                 \
                }                                                              \
            }                                                                  \
        }                                                                      \
    } while ( 0 )

            TRY_LOAD_MANY_OR_EXIT( guardBars );
            TRY_LOAD_MANY_OR_EXIT( brokenGuardBars );
            TRY_LOAD_MANY_OR_EXIT( guardGauges );
            TRY_LOAD_MANY_OR_EXIT( restorableHealthBars );
            TRY_LOAD_MANY_OR_EXIT( healthBars );
            TRY_LOAD_MANY_OR_EXIT( healthGauges );
            TRY_LOAD_MANY_OR_EXIT( logos );
            TRY_LOAD_MANY_OR_EXIT( names );
            TRY_LOAD_MANY_OR_EXIT( meterBars );
            TRY_LOAD_MANY_OR_EXIT( meterGauges );

#undef TRY_LOAD_MANY_OR_EXIT

#define TRY_LOAD_ONE_OR_EXIT( _field )                                         \
    do {                                                                       \
        const size_t l_##_field##Length = __builtin_strlen( #_field );         \
        if ( ( __builtin_strncmp( _fileName, #_field, l_##_field##Length ) ==  \
               0 ) &&                                                          \
             ( _fileName[ l_##_field##Length ] ==                              \
               ( ( l_isAnimationFrame ) ? ( '_' ) : ( '.' ) ) ) ) {            \
            state_t* l_state = arrayFirstElement( l_HUD->_field.states );      \
            SDL_Renderer* l_renderer = l_state->renderer;                      \
            l_returnValue = object_t$states$remove( &( l_HUD->_field ) );      \
            if ( UNLIKELY( !l_returnValue ) ) {                                \
                log$transaction$query( ( logLevel_t )error,                    \
                                       "Removing HUD " #_field " states" );    \
                goto EXIT;                                                     \
            }                                                                  \
            l_returnValue = HUD_t$element$load$one( &( l_HUD->_field ), l_HUD, \
                                                    l_renderer, #_field );     \
            if ( UNLIKELY( !l_returnValue ) ) {                                \
                log$transaction$query( ( logLevel_t )error,                    \
                                       "Loading HUD " #_field );               \
                goto EXIT;                                                     \
            }                                                                  \
        }                                                                      \
    } while ( 0 )

            TRY_LOAD_ONE_OR_EXIT( timer );
            TRY_LOAD_ONE_OR_EXIT( timerBackground );

#undef TRY_LOAD_ONE_OR_EXIT

            log$transaction$query$format(
                ( logLevel_t )debug, "Loaded HUD state %s from path: '%s'",
                ( ( l_isAnimationFrame ) ? ( "animation" ) : ( "boxes" ) ),
                _fileName );
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool HUD_t$load( HUD_t* restrict _HUD, SDL_Renderer* _renderer ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_HUD ) || UNLIKELY( !_HUD->name ) ||
         UNLIKELY( !_HUD->folder ) || UNLIKELY( !_HUD->extension ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !arrayLength( _HUD->players ) ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        log$transaction$query$format( ( logLevel_t )info, "Loading HUD: '%s'",
                                      _HUD->name );

#define TRY_LOAD_MANY_OR_EXIT( _field )                                   \
    do {                                                                  \
        FOR_RANGE( arrayLength_t, 0, arrayLength( _HUD->players ) ) {     \
            object_t l_element = object_t$create();                       \
            l_returnValue = HUD_t$element$load$one( &l_element, _HUD,     \
                                                    _renderer, #_field ); \
            if ( UNLIKELY( !l_returnValue ) ) {                           \
                log$transaction$query( ( logLevel_t )error,               \
                                       "Loading HUD " #_field );          \
                goto EXIT;                                                \
            }                                                             \
            insertIntoArray( &( _HUD->_field ), clone( &l_element ) );    \
        }                                                                 \
    } while ( 0 )

        TRY_LOAD_MANY_OR_EXIT( guardBars );
        TRY_LOAD_MANY_OR_EXIT( brokenGuardBars );
        TRY_LOAD_MANY_OR_EXIT( guardGauges );
        TRY_LOAD_MANY_OR_EXIT( restorableHealthBars );
        TRY_LOAD_MANY_OR_EXIT( healthBars );
        TRY_LOAD_MANY_OR_EXIT( healthGauges );
        TRY_LOAD_MANY_OR_EXIT( logos );
        TRY_LOAD_MANY_OR_EXIT( names );
        TRY_LOAD_MANY_OR_EXIT( meterBars );
        TRY_LOAD_MANY_OR_EXIT( meterGauges );

#undef TRY_LOAD_MANY_OR_EXIT

#define TRY_LOAD_ONE_OR_EXIT( _field )                                   \
    do {                                                                 \
        l_returnValue = HUD_t$element$load$one( &( _HUD->_field ), _HUD, \
                                                _renderer, #_field );    \
        if ( UNLIKELY( !l_returnValue ) ) {                              \
            log$transaction$query( ( logLevel_t )error,                  \
                                   "Loading HUD " #_field );             \
            goto EXIT;                                                   \
        }                                                                \
    } while ( 0 )

        TRY_LOAD_ONE_OR_EXIT( timerBackground );
        TRY_LOAD_ONE_OR_EXIT( timer );

#undef TRY_LOAD_ONE_OR_EXIT

#if defined( DEBUG )
        // Watch
        {
            watch_t l_watch = watch_t$create();

            l_returnValue = watch_t$add$toPath( &l_watch, _HUD->folder,
                                                HUD_t$reload, _HUD, true );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Adding HUD watch" );

                goto EXIT;
            }

            insertIntoArray( &( _HUD->watches ), clone( &l_watch ) );
        }
#endif

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool HUD_t$unload( HUD_t* restrict _HUD ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_HUD ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
#define REMOVE_STATES_AND_FREE_OR_EXIT( _field )                              \
    do {                                                                      \
        if ( UNLIKELY( !( _HUD->_field ) ) ) {                                \
            l_returnValue = false;                                            \
            log$transaction$query( ( logLevel_t )error, "Invalid argument" ); \
            goto EXIT;                                                        \
        }                                                                     \
        FOR_ARRAY( object_t* const*, ( _HUD->_field ) ) {                     \
            l_returnValue = object_t$states$remove( *_element );              \
            if ( UNLIKELY( !l_returnValue ) ) {                               \
                log$transaction$query( ( logLevel_t )error,                   \
                                       "Removing HUD " #_field " states" );   \
                goto EXIT;                                                    \
            }                                                                 \
            l_returnValue = object_t$destroy( *_element );                    \
            if ( UNLIKELY( !l_returnValue ) ) {                               \
                log$transaction$query( ( logLevel_t )error,                   \
                                       "Destroying HUD " #_field );           \
                goto EXIT;                                                    \
            }                                                                 \
        }                                                                     \
        FREE_ARRAY_ELEMENTS( _HUD->_field );                                  \
    } while ( 0 )

        REMOVE_STATES_AND_FREE_OR_EXIT( guardBars );
        REMOVE_STATES_AND_FREE_OR_EXIT( brokenGuardBars );
        REMOVE_STATES_AND_FREE_OR_EXIT( guardGauges );
        REMOVE_STATES_AND_FREE_OR_EXIT( restorableHealthBars );
        REMOVE_STATES_AND_FREE_OR_EXIT( healthBars );
        REMOVE_STATES_AND_FREE_OR_EXIT( healthGauges );
        REMOVE_STATES_AND_FREE_OR_EXIT( logos );
        REMOVE_STATES_AND_FREE_OR_EXIT( names );
        REMOVE_STATES_AND_FREE_OR_EXIT( meterBars );
        REMOVE_STATES_AND_FREE_OR_EXIT( meterGauges );

#undef REMOVE_STATES_AND_FREE_OR_EXIT

#define TRY_REMOVE_OR_EXIT( _field )                                 \
    do {                                                             \
        l_returnValue = object_t$states$remove( &( _HUD->_field ) ); \
        if ( UNLIKELY( !l_returnValue ) ) {                          \
            log$transaction$query( ( logLevel_t )error,              \
                                   "Removing " #_field " states" );  \
            goto EXIT;                                               \
        }                                                            \
    } while ( 0 )

        TRY_REMOVE_OR_EXIT( timerBackground );
        TRY_REMOVE_OR_EXIT( timer );

#undef TRY_REMOVE_OR_EXIT

#if defined( DEBUG )
        // Watch
        {
            FOR_RANGE( arrayLength_t, 0, arrayLength( _HUD->watches ) ) {
                watch_t* l_watch = _HUD->watches[ _index ];

                l_returnValue = watch_t$destroy( l_watch );

                if ( UNLIKELY( !l_returnValue ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Destroying background watch" );

                    goto EXIT;
                }

                free( l_watch );

                pluckArray( &( _HUD->watches ), l_watch );
            }
        }
#endif

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool HUD_t$step( HUD_t* restrict _HUD ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_HUD ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
#define STEP_OBJECTS_OR_EXIT( _field )                        \
    do {                                                      \
        FOR_ARRAY( object_t* const*, _HUD->_field ) {         \
            l_returnValue = object_t$step( *_element, 0, 0 ); \
            if ( UNLIKELY( !l_returnValue ) ) {               \
                log$transaction$query( ( logLevel_t )error,   \
                                       "Stepping " #_field ); \
                goto EXIT;                                    \
            }                                                 \
        }                                                     \
    } while ( 0 )

        STEP_OBJECTS_OR_EXIT( guardBars );
        STEP_OBJECTS_OR_EXIT( brokenGuardBars );
        STEP_OBJECTS_OR_EXIT( guardGauges );
        STEP_OBJECTS_OR_EXIT( restorableHealthBars );
        STEP_OBJECTS_OR_EXIT( healthBars );
        STEP_OBJECTS_OR_EXIT( healthGauges );
        STEP_OBJECTS_OR_EXIT( logos );
        STEP_OBJECTS_OR_EXIT( names );
        STEP_OBJECTS_OR_EXIT( meterBars );
        STEP_OBJECTS_OR_EXIT( meterGauges );

#undef STEP_OBJECTS_OR_EXIT

#define TRY_STEP_OR_EXIT( _field )                                             \
    do {                                                                       \
        l_returnValue = object_t$step( &( _HUD->_field ), 0, 0 );              \
        if ( UNLIKELY( !l_returnValue ) ) {                                    \
            log$transaction$query( ( logLevel_t )error, "Stepping " #_field ); \
            goto EXIT;                                                         \
        }                                                                      \
    } while ( 0 )

        TRY_STEP_OR_EXIT( timerBackground );
        TRY_STEP_OR_EXIT( timer );

#undef TRY_STEP_OR_EXIT

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool HUD_t$render( const HUD_t* restrict _HUD ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_HUD ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        const SDL_FRect l_cameraRectangle = {
            .x = 0,
            .y = 0,
            .w = 0,
            .h = 0,
        };
        const bool l_doDrawBoxes = false;

#define RENDER_BARS_OR_EXIT( _name )                                           \
    do {                                                                       \
        FOR_RANGE( arrayLength_t, 0, arrayLength( _HUD->_name##Bars ) ) {      \
            typeof( *( _HUD->_name##Bars ) ) l_element =                       \
                _HUD->_name##Bars[ _index ];                                   \
            player_t* l_player = _HUD->players[ _index ];                      \
            const float l_widthCurrent =                                       \
                ( animation_t$currentTargetRectangle$get(                      \
                      &( l_element->currentState->animation ) ) )              \
                    ->w;                                                       \
            ( animation_t$currentTargetRectangle$get(                          \
                  &( l_element->currentState->animation ) ) )                  \
                ->w = ( ( l_widthCurrent * *( l_player->_name##Points ) ) /    \
                        *( l_player->_name##PointsMax ) );                     \
            if ( _index % 2 ) {                                                \
                const float l_widthMissing =                                   \
                    ( l_widthCurrent -                                         \
                      ( animation_t$currentTargetRectangle$get(                \
                            &( l_element->currentState->animation ) ) )        \
                          ->w );                                               \
                const size_t l_worldXCurrent = l_element->worldX;              \
                l_element->worldX = ( l_element->worldXMax + l_widthMissing ); \
                l_returnValue = object_t$render$rotated(                       \
                    l_element, 180, SDL_FLIP_VERTICAL, &l_cameraRectangle,     \
                    l_doDrawBoxes );                                           \
                l_element->worldX = l_worldXCurrent;                           \
            } else {                                                           \
                l_returnValue = object_t$render(                               \
                    l_element, &l_cameraRectangle, l_doDrawBoxes );            \
            }                                                                  \
            ( animation_t$currentTargetRectangle$get(                          \
                  &( l_element->currentState->animation ) ) )                  \
                ->w = l_widthCurrent;                                          \
            if ( UNLIKELY( !l_returnValue ) ) {                                \
                log$transaction$query( ( logLevel_t )error,                    \
                                       "Rendering " #_name " bars" );          \
                goto EXIT;                                                     \
            }                                                                  \
        }                                                                      \
    } while ( 0 )

#define RENDER_OBJECTS_OR_EXIT( _field )                                    \
    do {                                                                    \
        FOR_RANGE( arrayLength_t, 0, arrayLength( _HUD->_field ) ) {        \
            typeof( *( _HUD->_field ) ) l_element = _HUD->_field[ _index ]; \
            if ( _index % 2 ) {                                             \
                const size_t l_worldXCurrent = l_element->worldX;           \
                l_element->worldX = l_element->worldXMax;                   \
                l_returnValue = object_t$render$rotated(                    \
                    l_element, 180, SDL_FLIP_VERTICAL, &l_cameraRectangle,  \
                    l_doDrawBoxes );                                        \
                l_element->worldX = l_worldXCurrent;                        \
            } else {                                                        \
                l_returnValue = object_t$render(                            \
                    l_element, &l_cameraRectangle, l_doDrawBoxes );         \
            }                                                               \
            if ( UNLIKELY( !l_returnValue ) ) {                             \
                log$transaction$query( ( logLevel_t )error,                 \
                                       "Rendering " #_field );              \
                goto EXIT;                                                  \
            }                                                               \
        }                                                                   \
    } while ( 0 )

        RENDER_BARS_OR_EXIT( guard );

        // Broken guard bars
        FOR_RANGE( arrayLength_t, 0, arrayLength( _HUD->brokenGuardBars ) ) {
            {
                const player_t* l_player = _HUD->players[ _index ];

                if ( !( *( l_player->isGuardBroken ) ) ) {
                    continue;
                }
            }

            object_t* l_element = _HUD->brokenGuardBars[ _index ];

            if ( _index % 2 ) {
                const size_t l_worldXCurrent = l_element->worldX;

                l_element->worldX = l_element->worldXMax;

                l_returnValue = object_t$render$rotated(
                    l_element, 180, SDL_FLIP_VERTICAL, &l_cameraRectangle,
                    l_doDrawBoxes );

                l_element->worldX = l_worldXCurrent;

            } else {
                l_returnValue = object_t$render( l_element, &l_cameraRectangle,
                                                 l_doDrawBoxes );
            }

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Rendering brokenGuard bars" );

                goto EXIT;
            }
        }

        RENDER_OBJECTS_OR_EXIT( guardGauges );
        RENDER_BARS_OR_EXIT( restorableHealth );
        RENDER_BARS_OR_EXIT( health );
        RENDER_OBJECTS_OR_EXIT( healthGauges );
        RENDER_OBJECTS_OR_EXIT( logos );
        RENDER_OBJECTS_OR_EXIT( names );
        RENDER_BARS_OR_EXIT( meter );
        RENDER_OBJECTS_OR_EXIT( meterGauges );

#undef RENDER_OBJECTS_OR_EXIT
#undef RENDER_BARS_OR_EXIT

#define TRY_RENDER_OR_EXIT( _field )                                          \
    do {                                                                      \
        l_returnValue = object_t$render( &( _HUD->_field ),                   \
                                         &l_cameraRectangle, l_doDrawBoxes ); \
        if ( UNLIKELY( !l_returnValue ) ) {                                   \
            log$transaction$query( ( logLevel_t )error,                       \
                                   "Rendering " #_field );                    \
            goto EXIT;                                                        \
        }                                                                     \
    } while ( 0 )

        TRY_RENDER_OR_EXIT( timerBackground );
        TRY_RENDER_OR_EXIT( timer );

#undef TRY_RENDER_OR_EXIT

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

#if defined( HOT_RELOAD )

bool hotReload$unload( void** restrict _state,
                       size_t* restrict _stateSize,
                       applicationState_t* restrict _applicationState ) {
    UNUSED( _state );
    UNUSED( _stateSize );

    if ( LIKELY( _applicationState->HUD ) &&
         LIKELY( _applicationState->HUD->watches ) ) {
        FOR_ARRAY( watch_t* const*, _applicationState->HUD->watches ) {
            watch_t* l_element = *_element;

            FOR_ARRAY( watchCallback_t*, l_element->watchCallbacks ) {
                *_element = NULL;
            }
        }
    }

    return ( true );
}

bool hotReload$load( void* restrict _state,
                     size_t _stateSize,
                     applicationState_t* restrict _applicationState ) {
    UNUSED( _state );
    UNUSED( _stateSize );

    if ( LIKELY( _applicationState->HUD ) &&
         LIKELY( _applicationState->HUD->watches ) ) {
        FOR_ARRAY( watch_t* const*, _applicationState->HUD->watches ) {
            watch_t* l_element = *_element;

            FOR_ARRAY( watchCallback_t*, l_element->watchCallbacks ) {
                *_element = HUD_t$reload;
            }
        }
    }

    return ( true );
}

#endif
