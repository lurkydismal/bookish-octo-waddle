#include "controls_t.h"
#include "test.h"

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

TEST( control_t$create ) {
    control_t l_control = control_t$create();

    bool l_returnValue = control_t$destroy( &l_control );

    ASSERT_TRUE( l_returnValue );
}

TEST( control_t$destroy ) {
    // Invalid - NULL input
    {
        bool l_returnValue = control_t$destroy( NULL );

        ASSERT_FALSE( l_returnValue );
    }

    // Valid
    {
        control_t l_control = control_t$create();

        bool l_returnValue = control_t$destroy( &l_control );

        ASSERT_TRUE( l_returnValue );
    }
}

TEST( control_t$scancode$convert$fromString ) {
    // Invalid - NULL input
    {
        const SDL_Scancode l_scancode =
            control_t$scancode$convert$fromString( NULL );

        ASSERT_EQ( "%u", l_scancode, SDL_SCANCODE_UNKNOWN );
    }

    // Valid
    {
        FOR_RANGE( size_t, 0, arrayLengthNative( g_keyboardLayoutKeyNames ) ) {
            const SDL_Scancode l_actual = control_t$scancode$convert$fromString(
                g_keyboardLayoutKeyNames[ _index ] );
            const SDL_Scancode l_expected = g_keyboardLayoutScancodes[ _index ];

            ASSERT_EQ( "%u", l_actual, l_expected );
        }
    }
}

TEST( control_t$scancode$convert$toStaticString ) {
    // Invalid - NULL input
    {
        const char* l_scancode =
            control_t$scancode$convert$toStaticString( SDL_SCANCODE_UNKNOWN );

        ASSERT_STRING_EQ( l_scancode, CONTROL_AS_STRING_UNKNOWN );
    }

    // Valid
    {
        FOR_RANGE( size_t, 0, arrayLengthNative( g_keyboardLayoutKeyNames ) ) {
            const char* l_actual = control_t$scancode$convert$toStaticString(
                g_keyboardLayoutScancodes[ _index ] );
            const char* l_expected = g_keyboardLayoutKeyNames[ _index ];

            ASSERT_STRING_EQ( l_actual, l_expected );
        }
    }
}
