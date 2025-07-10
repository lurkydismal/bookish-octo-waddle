#include "init.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <argp.h>
#include <stdlib.h>

#include "FPS.h"
#include "asset_t.h"
#include "config_t.h"
#include "log.h"
#include "settings_t.h"
#include "stdfunc.h"
#include "vsync.h"

#define LOG_FILE_NAME_DEFAULT "log"
#define LOG_FILE_EXTENSION_DEFAULT "txt"

#define ASSETS_DIRECTORY "assets"

#define SETTINGS_FILE_NAME "settings"
#define SETTINGS_FILE_EXTENSION "ini"

#define CONFIG_FILE_NAME "config"
#define CONFIG_FILE_EXTENSION "ini"

#define PRINT_CONFIG_FORMAT_STRING \
    "\nConfig options:\n"          \
    "%s" ASCII_COLOR_RESET "\n"

// Keys should match keys for settings options
#define SETTINGS_FORMAT_STRING \
    "window_width = %zu\n"     \
    "window_height = %zu\n"    \
    "up = %s\n"                \
    "down = %s\n"              \
    "left = %s\n"              \
    "right = %s\n"             \
    "light_attack = %s\n"      \
    "medium_attack = %s\n"     \
    "heavy_attack = %s\n"      \
    "shield = %s\n"            \
    "background_index = %zu\n" \
    "HUD_index = %zu\n"        \
    "character_index = %zu\n"

#define SETTINGS_FORMAT_ARGUMENTS( _settings )                 \
    ( _settings ).window.width, ( _settings ).window.height,   \
        control_t$scancode$convert$toStaticString(             \
            ( _settings ).controls.up.scancode ),              \
        control_t$scancode$convert$toStaticString(             \
            ( _settings ).controls.down.scancode ),            \
        control_t$scancode$convert$toStaticString(             \
            ( _settings ).controls.left.scancode ),            \
        control_t$scancode$convert$toStaticString(             \
            ( _settings ).controls.right.scancode ),           \
        control_t$scancode$convert$toStaticString(             \
            ( _settings ).controls.A.scancode ),               \
        control_t$scancode$convert$toStaticString(             \
            ( _settings ).controls.B.scancode ),               \
        control_t$scancode$convert$toStaticString(             \
            ( _settings ).controls.C.scancode ),               \
        control_t$scancode$convert$toStaticString(             \
            ( _settings ).controls.D.scancode ),               \
        ( _settings ).backgroundIndex, ( _settings ).HUDIndex, \
        ( _settings ).characterIndex

const char* argp_program_version;
const char* argp_program_bug_address;

static error_t parserForOption( int _key,
                                char* _value,
                                struct argp_state* _state ) {
    error_t l_returnValue = 0;

    applicationState_t* l_applicationState =
        ( applicationState_t* )( _state->input );

    switch ( _key ) {
        // Verbose
        case 'v': {
            const logLevel_t l_logLevel = info;

            if ( UNLIKELY( !log$level$set( l_logLevel ) ) ) {
                log$transaction$query$format(
                    ( logLevel_t )error, "Setting log level to %s",
                    log$level$convert$toStaticString( l_logLevel ) );

                l_returnValue = EPERM;

                goto EXIT;
            }

            break;
        }

            // Quiet
        case 'q': {
            const logLevel_t l_logLevel = unknownLogLevel;

            if ( UNLIKELY( !log$level$set( l_logLevel ) ) ) {
                log$transaction$query$format(
                    ( logLevel_t )error, "Setting log level to %s",
                    log$level$convert$toStaticString( l_logLevel ) );

                l_returnValue = EPERM;

                goto EXIT;
            }

            break;
        }

            // Background
        case 'b': {
            const size_t l_backgroundIndex = strtoul( _value, NULL, 10 );

            if ( UNLIKELY(
                     l_backgroundIndex >=
                     arrayLength( l_applicationState->config.backgrounds ) ) ) {
                log$transaction$query$format( ( logLevel_t )error,
                                              "Background index: %s", _value );

                argp_error( _state, "Background index '%s' is out of range",
                            _value );

                break;
            }

            l_applicationState->settings.backgroundIndex = l_backgroundIndex;

            l_applicationState->background =
                l_applicationState->config.backgrounds[ l_backgroundIndex ];

            break;
        }

            // HUD
        case 'h': {
            const size_t l_HUDIndex = strtoul( _value, NULL, 10 );

            if ( UNLIKELY( l_HUDIndex >=
                           arrayLength( l_applicationState->config.HUDs ) ) ) {
                log$transaction$query$format( ( logLevel_t )error,
                                              "HUD index: %s", _value );

                argp_error( _state, "HUD index '%s' is out of range", _value );

                break;
            }

            l_applicationState->settings.HUDIndex = l_HUDIndex;

            l_applicationState->HUD =
                l_applicationState->config.HUDs[ l_HUDIndex ];

            break;
        }

            // Character
        case 'c': {
            const size_t l_characterIndex = strtoul( _value, NULL, 10 );

            if ( UNLIKELY(
                     l_characterIndex >=
                     arrayLength( l_applicationState->config.characters ) ) ) {
                log$transaction$query$format( ( logLevel_t )error,
                                              "Character index: %s", _value );

                argp_error( _state, "Character index '%s' is out of range",
                            _value );

                break;
            }

            l_applicationState->settings.characterIndex = l_characterIndex;

            l_applicationState->character =
                l_applicationState->config.characters[ l_characterIndex ];

            break;
        }

            // Print
        case 'p': {
            char l_configAsString[ PATH_MAX ] = { '\0' };

#define configFieldHandler( _field, _fieldName, _configAsString )            \
    do {                                                                     \
        static size_t l_index = 0;                                           \
        const char* l_name = ( _field )->name;                               \
        const char* l_type = ( _fieldName );                                 \
        const size_t l_configAsStringLength =                                \
            __builtin_strlen( _configAsString );                             \
        const size_t l_configAsStringRemainingLength =                       \
            ( PATH_MAX - l_configAsStringLength );                           \
        snprintf( ( ( _configAsString ) + l_configAsStringLength ),          \
                  l_configAsStringRemainingLength,                           \
                  ASCII_COLOR_YELLOW                                         \
                  "[" ASCII_COLOR_RESET_FOREGROUND " '" ASCII_COLOR_GREEN    \
                  "%s" ASCII_COLOR_RESET_FOREGROUND "' = '" ASCII_COLOR_RED  \
                  "%zu" ASCII_COLOR_RESET_FOREGROUND "' " ASCII_COLOR_YELLOW \
                  "]" ASCII_COLOR_RESET_FOREGROUND                           \
                  ": '" ASCII_COLOR_PURPLE_LIGHT                             \
                  "%s" ASCII_COLOR_RESET_FOREGROUND "'\n",                   \
                  l_name, l_index, l_type );                                 \
        l_index++;                                                           \
    } while ( 0 )

#define HANDLE_CONFIG_FIELD( _field )                                   \
    do {                                                                \
        FOR_ARRAY( _field##_t* const*,                                  \
                   l_applicationState->config._field##s ) {             \
            configFieldHandler( *_element, #_field, l_configAsString ); \
        }                                                               \
    } while ( 0 )

            HANDLE_CONFIG_FIELD( background );
            HANDLE_CONFIG_FIELD( HUD );
            HANDLE_CONFIG_FIELD( character );

#undef HANDLE_CONFIG_FIELD

#undef configFieldHandler

            printf( PRINT_CONFIG_FORMAT_STRING, l_configAsString );

            _exit( 0 );
        }

            // Save
        case 's': {
            // Will not be cleaned or free'd
            asset_t l_settingsAsAsset = asset_t$create();

            // Generate settings asset
            {
                size_t l_length = 0;

                // Generate settings as string
                l_length = snprintf(
                    NULL, 0, SETTINGS_FORMAT_STRING,
                    SETTINGS_FORMAT_ARGUMENTS( l_applicationState->settings ) );

                // Allocate asset
                {
                    l_length++;

                    l_settingsAsAsset.size = ( l_length * sizeof( uint8_t ) );
                    l_settingsAsAsset.data =
                        ( uint8_t* )malloc( l_settingsAsAsset.size );
                }

                // Fill asset
                {
                    l_length = snprintf( ( char* )( l_settingsAsAsset.data ),
                                         l_settingsAsAsset.size,
                                         SETTINGS_FORMAT_STRING,
                                         SETTINGS_FORMAT_ARGUMENTS(
                                             l_applicationState->settings ) );

                    if ( l_length != ( l_settingsAsAsset.size - 1 ) ) {
                        log$transaction$query$format(
                            ( logLevel_t )error, "Generating settings %zu %zu",
                            l_length, l_settingsAsAsset.size );

                        argp_error( _state, "Failed to generate settings" );
                    }

                    // Exclude NULL
                    l_settingsAsAsset.size--;
                }
            }

            // Save to path
            {
                const char* l_filePath =
                    ( SETTINGS_FILE_NAME "." SETTINGS_FILE_EXTENSION );

                const bool l_result = asset_t$save$sync$toPath(
                    &l_settingsAsAsset, l_filePath, true // Need truncate
                );

                if ( UNLIKELY( !l_result ) ) {
                    log$transaction$query$format( ( logLevel_t )error,
                                                  "Saving settings file: '%s'",
                                                  l_filePath );

                    argp_error( _state, "Failed to save settings file: '%s'",
                                l_filePath );
                }

                log$transaction$query$format( ( logLevel_t )info,
                                              "Saved settings file: '%s'",
                                              l_filePath );

                log$transaction$commit();
            }

            // Exit process
            _exit( l_returnValue );
        }

        case ARGP_KEY_END: {
#define PICK_RANDOM_IF_NULL( _field )                                       \
    do {                                                                    \
        if ( !( l_applicationState->_field ) ) {                            \
            _field##_t** const* l_##_field##s =                             \
                &( l_applicationState->config._field##s );                  \
            if ( LIKELY( *l_##_field##s ) ) {                               \
                if ( LIKELY( arrayLength( *l_##_field##s ) ) ) {            \
                    size_t l_index =                                        \
                        l_applicationState->settings._field##Index;         \
                    const size_t l_##_field##sAmount =                      \
                        arrayLength( *l_##_field##s );                      \
                    if ( l_index >= l_##_field##sAmount ) {                 \
                        l_index = ( randomNumber() % l_##_field##sAmount ); \
                        log$transaction$query$format(                       \
                            ( logLevel_t )info,                             \
                            "Selecting random " #_field ": [ %zu ]",        \
                            l_index );                                      \
                    }                                                       \
                    l_applicationState->_field =                            \
                        ( *l_##_field##s )[ l_index ];                      \
                }                                                           \
            }                                                               \
        }                                                                   \
    } while ( 0 )

            PICK_RANDOM_IF_NULL( background );
            PICK_RANDOM_IF_NULL( HUD );
            PICK_RANDOM_IF_NULL( character );

#undef PICK_RANDOM_IF_NULL

            break;
        }

        default: {
            l_returnValue = ARGP_ERR_UNKNOWN;
        }
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Improve
static FORCE_INLINE bool parseArguments(
    applicationState_t* restrict _applicationState,
    int _argumentCount,
    char** restrict _argumentVector ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_argumentVector ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        // Application name and version
        {
            char* l_nameAndVersion = duplicateString( " " );

            concatBeforeAndAfterString( &l_nameAndVersion,
                                        _applicationState->settings.identifier,
                                        _applicationState->settings.version );

            argp_program_version = l_nameAndVersion;
        }

        argp_program_bug_address = _applicationState->settings.contactAddress;

        {
            char* l_description = NULL;

            {
                l_description = duplicateString( " - " );

                concatBeforeAndAfterString(
                    &l_description, _applicationState->settings.identifier,
                    _applicationState->settings.description );
            }

            struct argp_option l_options[] = {
                { "verbose", 'v', 0, 0, "Produce verbose output", 0 },
                { "quiet", 'q', 0, 0, "Do not produce any output", 0 },
                { "background", 'b', "INDEX", 0, "Select background by index",
                  0 },
                { "HUD", 'h', "INDEX", 0, "Select HUD by index", 0 },
                { "character", 'c', "INDEX", 0, "Select character by index",
                  0 },
                { "print", 'p', 0, 0, "Print available configuration", 0 },
                { "save", 's', 0, 0, "Save without running", 0 },
                { 0 } };

            // [NAME] - optional
            // NAME - required
            // NAME... - at least one and more
            const char l_arguments[] = "";

            struct argp l_argumentParser = {
                l_options, parserForOption, l_arguments, l_description, 0, 0,
                0 };

            l_returnValue =
                argp_parse( &l_argumentParser, _argumentCount, _argumentVector,
                            0, 0, _applicationState );

            free( l_description );
        }

        if ( UNLIKELY( !l_returnValue ) ) {
            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool init( applicationState_t* restrict _applicationState,
           int _argumentCount,
           char** _argumentVector ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_applicationState ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_argumentVector ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        // Log
        {
            assert(
                log$init( LOG_FILE_NAME_DEFAULT, LOG_FILE_EXTENSION_DEFAULT ),
                "Log initialization" );

#if defined( DEBUG )

            const logLevel_t l_logLevel = debug;

#elif defined( PROFILE )

            const logLevel_t l_logLevel = info;

#endif

#if ( defined( DEBUG ) || defined( PROFILE ) )

            if ( UNLIKELY( !log$level$set( l_logLevel ) ) ) {
                log$transaction$query$format(
                    ( logLevel_t )error, "Setting log level to %s",
                    log$level$convert$toStaticString( l_logLevel ) );

                goto EXIT;
            }

#endif
        }

        // Asset loader
        {
            if ( UNLIKELY( !asset_t$loader$init( ASSETS_DIRECTORY ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Initializing asset loader" );

                goto EXIT;
            }
        }

        // Generate application state
        {
            *_applicationState = applicationState_t$create();

            // Settings
            {
                if ( UNLIKELY( !settings_t$load$fromPath(
                         &( _applicationState->settings ), SETTINGS_FILE_NAME,
                         SETTINGS_FILE_EXTENSION ) ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Loading settings" );

                    log$transaction$query( ( logLevel_t )info,
                                           "Loading default settings" );

                    log$transaction$commit();

                    _applicationState->settings = settings_t$create();
                }
            }

            // Metadata
            {
                log$transaction$query$format(
                    ( logLevel_t )info,
                    "Window name: '%s', Version: '%s', Identifier: '%s'",
                    _applicationState->settings.window.name,
                    _applicationState->settings.version,
                    _applicationState->settings.identifier );

                log$transaction$commit();

                if ( UNLIKELY( !SDL_SetAppMetadata(
                         _applicationState->settings.window.name,
                         _applicationState->settings.version,
                         _applicationState->settings.identifier ) ) ) {
                    log$transaction$query$format( ( logLevel_t )error,
                                                  "Setting render scale: '%s'",
                                                  SDL_GetError() );

                    goto EXIT;
                }
            }

            // Configuration
            {
                // Backgrounds
                // UI
                // TODO: Implement
                // Characters
                if ( UNLIKELY( !config_t$load$fromPath(
                         &( _applicationState->config ), CONFIG_FILE_NAME,
                         CONFIG_FILE_EXTENSION ) ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Loading config" );

                    goto EXIT;
                }
            }

            // Application arguments
            // Setup recources to load
            {
                if ( UNLIKELY( parseArguments( _applicationState,
                                               _argumentCount,
                                               _argumentVector ) ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Parsing arguments" );

                    goto EXIT;
                }
            }

            // Init SDL sub-systems
            {
                SDL_Init( SDL_INIT_VIDEO );
            }

            // Window and Renderer
            {
                if ( UNLIKELY( !SDL_CreateWindowAndRenderer(
                         _applicationState->settings.window.name,
                         _applicationState->settings.window.width,
                         _applicationState->settings.window.height,
                         ( SDL_WINDOW_INPUT_FOCUS ),
                         &( _applicationState->window ),
                         &( _applicationState->renderer ) ) ) ) {
                    log$transaction$query$format(
                        ( logLevel_t )error,
                        "Window or Renderer creation: '%s'", SDL_GetError() );

                    goto EXIT;
                }
            }

            // TODO: Improve
            // Default scale mode
            {
                if ( UNLIKELY( !SDL_SetDefaultTextureScaleMode(
                         _applicationState->renderer,
                         SDL_SCALEMODE_PIXELART ) ) ) {
                    log$transaction$query$format(
                        ( logLevel_t )error,
                        "Setting render pixel scale mode: '%s'",
                        SDL_GetError() );

                    if ( UNLIKELY( !SDL_SetDefaultTextureScaleMode(
                             _applicationState->renderer,
                             SDL_SCALEMODE_NEAREST ) ) ) {
                        log$transaction$query$format(
                            ( logLevel_t )error,
                            "Setting render nearest scale mode: '%s'",
                            SDL_GetError() );

                        goto EXIT;
                    }
                }
            }

            // Scaling
            {
                const float l_scaleX =
                    ( ( float )( _applicationState->settings.window.width ) /
                      ( float )( _applicationState->logicalWidth ) );
                const float l_scaleY =
                    ( ( float )( _applicationState->settings.window.height ) /
                      ( float )( _applicationState->logicalHeight ) );

                if ( !SDL_SetRenderScale( _applicationState->renderer, l_scaleX,
                                          l_scaleY ) ) {
                    log$transaction$query$format( ( logLevel_t )error,
                                                  "Setting render scale: '%s'",
                                                  SDL_GetError() );

                    goto EXIT;
                }
            }

            // Load resources
            {
                if ( UNLIKELY(
                         !applicationState_t$load( _applicationState ) ) ) {
                    log$transaction$query( ( logLevel_t )error,
                                           "Loading application state" );

                    goto EXIT;
                }
            }
        }

        // Vsync
        {
            if ( UNLIKELY(
                     !vsync$init( _applicationState->settings.window.vsync,
                                  _applicationState->settings.window.desiredFPS,
                                  _applicationState->renderer ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Initializing Vsync" );

                goto EXIT;
            }
        }

        // FPS
        {
            if ( UNLIKELY( !FPS$init(
                     &( _applicationState->totalFramesRendered ) ) ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Initializing FPS" );

                goto EXIT;
            }
        }

        // Gamepad
        {
            if ( !!( SDL_HasGamepad() ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Initializing Gamepad" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
