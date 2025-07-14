#include "control_t.h"

#include "log.h"
#include "stdfunc.h"

static const SDL_Scancode g_keyboardLayoutScancodes[] = {
    SDL_SCANCODE_ESCAPE,
    SDL_SCANCODE_F1,
    SDL_SCANCODE_F2,
    SDL_SCANCODE_F3,
    SDL_SCANCODE_F4,
    SDL_SCANCODE_F5,

    SDL_SCANCODE_F6,
    SDL_SCANCODE_F7,
    SDL_SCANCODE_F8,
    SDL_SCANCODE_F9,
    SDL_SCANCODE_F10,
    SDL_SCANCODE_F11,

    SDL_SCANCODE_F12,
    SDL_SCANCODE_PAUSE,
    SDL_SCANCODE_GRAVE,

    SDL_SCANCODE_1,
    SDL_SCANCODE_2,
    SDL_SCANCODE_3,
    SDL_SCANCODE_4,
    SDL_SCANCODE_5,
    SDL_SCANCODE_6,

    SDL_SCANCODE_7,
    SDL_SCANCODE_8,
    SDL_SCANCODE_9,
    SDL_SCANCODE_0,
    SDL_SCANCODE_MINUS,
    SDL_SCANCODE_EQUALS,

    SDL_SCANCODE_BACKSPACE,
    SDL_SCANCODE_INSERT,
    SDL_SCANCODE_HOME,
    SDL_SCANCODE_PAGEUP,
    SDL_SCANCODE_TAB,
    SDL_SCANCODE_Q,

    SDL_SCANCODE_W,
    SDL_SCANCODE_E,
    SDL_SCANCODE_R,
    SDL_SCANCODE_T,
    SDL_SCANCODE_Y,
    SDL_SCANCODE_U,

    SDL_SCANCODE_I,
    SDL_SCANCODE_O,
    SDL_SCANCODE_P,
    SDL_SCANCODE_LEFTBRACKET,
    SDL_SCANCODE_RIGHTBRACKET,
    SDL_SCANCODE_BACKSLASH,

    SDL_SCANCODE_DELETE,
    SDL_SCANCODE_END,
    SDL_SCANCODE_PAGEDOWN,
    SDL_SCANCODE_CAPSLOCK,
    SDL_SCANCODE_A,
    SDL_SCANCODE_S,

    SDL_SCANCODE_D,
    SDL_SCANCODE_F,
    SDL_SCANCODE_G,
    SDL_SCANCODE_H,
    SDL_SCANCODE_J,
    SDL_SCANCODE_K,

    SDL_SCANCODE_L,
    SDL_SCANCODE_SEMICOLON,
    SDL_SCANCODE_APOSTROPHE,
    SDL_SCANCODE_RETURN,
    SDL_SCANCODE_LSHIFT,
    SDL_SCANCODE_Z,

    SDL_SCANCODE_X,
    SDL_SCANCODE_C,
    SDL_SCANCODE_V,
    SDL_SCANCODE_B,
    SDL_SCANCODE_N,
    SDL_SCANCODE_M,

    SDL_SCANCODE_COMMA,
    SDL_SCANCODE_PERIOD,
    SDL_SCANCODE_SLASH,
    SDL_SCANCODE_RSHIFT,
    SDL_SCANCODE_UP,
    SDL_SCANCODE_LCTRL,

    SDL_SCANCODE_LALT,
    SDL_SCANCODE_SPACE,
    SDL_SCANCODE_RALT,
    SDL_SCANCODE_RCTRL,
    SDL_SCANCODE_LEFT,
    SDL_SCANCODE_DOWN,

    SDL_SCANCODE_RIGHT };

static const char* g_keyboardLayoutKeyNames[] = {
    "Esc",    "F1",     "F2",      "F3",   "F4",     "F5",      "F6",
    "F7",     "F8",     "F9",      "F10",  "F11",    "F12",     "Pause",
    "~",      "1",      "2",       "3",    "4",      "5",       "6",
    "7",      "8",      "9",       "0",    "-",      "+",       "Backspace",
    "Insert", "Home",   "PgUp",    "Tab",  "Q",      "W",       "E",
    "R",      "T",      "Y",       "U",    "I",      "O",       "P",
    "[",      "]",      "|",       "Del",  "End",    "PgDn",    "Caps Lock",
    "A",      "S",      "D",       "F",    "G",      "H",       "J",
    "K",      "L",      ";",       "\'",   "Enter",  "L.Shift", "Z",
    "X",      "C",      "V",       "B",    "N",      "M",       ",",
    ".",      "/",      "R.Shift", "Up",   "L.Ctrl", "L.Alt",   "Space",
    "R.Alt",  "R.Ctrl", "Left",    "Down", "Right" };

control_t control_t$create( void ) {
    control_t l_returnValue = DEFAULT_CONTROL;

    {
        l_returnValue.input = input_t$create();
    }

    return ( l_returnValue );
}

bool control_t$destroy( control_t* restrict _control ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_control ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        _control->scancode = SDL_SCANCODE_UNKNOWN;

        l_returnValue = input_t$destroy( &( _control->input ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Destroying input" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

SDL_Scancode control_t$scancode$convert$fromString( const char* _string ) {
    // Unknown
    SDL_Scancode l_returnValue = SDL_SCANCODE_UNKNOWN;

    if ( UNLIKELY( !_string ) || UNLIKELY( !__builtin_strlen( _string ) ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        FOR_RANGE( size_t, 0, arrayLengthNative( g_keyboardLayoutKeyNames ) ) {
            if ( __builtin_strcmp( g_keyboardLayoutKeyNames[ _index ],
                                   _string ) == 0 ) {
                l_returnValue = g_keyboardLayoutScancodes[ _index ];

                break;
            }
        }
    }

EXIT:
    return ( l_returnValue );
}

const char* control_t$scancode$convert$toStaticString(
    SDL_Scancode _scancode ) {
    FOR_RANGE( size_t, 0, arrayLengthNative( g_keyboardLayoutScancodes ) ) {
        if ( g_keyboardLayoutScancodes[ _index ] == _scancode ) {
            return ( g_keyboardLayoutKeyNames[ _index ] );
        }
    }

    return ( CONTROL_AS_STRING_UNKNOWN );
}
