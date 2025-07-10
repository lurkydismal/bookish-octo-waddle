#include "controls_t.h"

#include "test.h"

TEST( controls_t$create ) {
    controls_t l_controls = controls_t$create();

    bool l_returnValue = controls_t$destroy( &l_controls );

    ASSERT_TRUE( l_returnValue );
}

TEST( controls_t$destroy ) {
    // Invalid - NULL input
    {
        bool l_returnValue = controls_t$destroy( NULL );

        ASSERT_FALSE( l_returnValue );
    }

    // Valid
    {
        controls_t l_controls = controls_t$create();

        bool l_returnValue = controls_t$destroy( &l_controls );

        ASSERT_TRUE( l_returnValue );
    }
}

TEST( controls_t$control_t$convert$fromScancode ) {
    controls_t l_controls = controls_t$create();

    l_controls.up.scancode = SDL_SCANCODE_W;
    l_controls.down.scancode = SDL_SCANCODE_S;
    l_controls.left.scancode = SDL_SCANCODE_A;
    l_controls.right.scancode = SDL_SCANCODE_D;
    l_controls.A.scancode = SDL_SCANCODE_J;
    l_controls.B.scancode = SDL_SCANCODE_K;
    l_controls.C.scancode = SDL_SCANCODE_L;
    l_controls.D.scancode = SDL_SCANCODE_SEMICOLON;

    const SDL_Scancode l_scancodes[] = {
        SDL_SCANCODE_W,      SDL_SCANCODE_S,         SDL_SCANCODE_A,
        SDL_SCANCODE_D,      SDL_SCANCODE_J,         SDL_SCANCODE_K,
        SDL_SCANCODE_L,      SDL_SCANCODE_SEMICOLON,
        SDL_SCANCODE_Z,      // Not mapped
        SDL_SCANCODE_UNKNOWN // Invalid scancode
    };

    const control_t* l_expectedControl[] = {
        &l_controls.up,
        &l_controls.down,
        &l_controls.left,
        &l_controls.right,
        &l_controls.A,
        &l_controls.B,
        &l_controls.C,
        &l_controls.D,
        NULL, // Not mapped
        NULL  // Invalid scancode
    };

    FOR_RANGE( size_t, 0, arrayLengthNative( l_scancodes ) ) {
        const control_t* l_actual = controls_t$control_t$convert$fromScancode(
            &l_controls, l_scancodes[ _index ] );
        const control_t* l_expected = l_expectedControl[ _index ];

        ASSERT_EQ( "%p", l_actual, l_expected );
    }

    bool l_returnValue = controls_t$destroy( &l_controls );

    ASSERT_TRUE( l_returnValue );
}

TEST( controls_t$control_t$convert$fromInput ) {
    controls_t l_controls = controls_t$create();

    const input_t l_inputs[] = {
        MAKE_INPUT( UP, 0 ),       MAKE_INPUT( DOWN, 0 ),
        MAKE_INPUT( LEFT, 0 ),     MAKE_INPUT( RIGHT, 0 ),
        MAKE_INPUT( 0, A ),        MAKE_INPUT( 0, B ),
        MAKE_INPUT( 0, C ),        MAKE_INPUT( 0, D ),
        MAKE_INPUT( UP_RIGHT, 0 ), // Not mapped
        MAKE_INPUT( 0, 0 )         // Invalid input
    };

    const control_t* l_expectedControl[] = {
        &l_controls.up,
        &l_controls.down,
        &l_controls.left,
        &l_controls.right,
        &l_controls.A,
        &l_controls.B,
        &l_controls.C,
        &l_controls.D,
        NULL, // Not mapped
        NULL  // Invalid input
    };

    FOR_RANGE( size_t, 0, arrayLengthNative( l_inputs ) ) {
        const control_t* l_actual = controls_t$control_t$convert$fromInput(
            &l_controls, &( l_inputs[ _index ] ) );
        const control_t* l_expected = l_expectedControl[ _index ];

        ASSERT_EQ( "%p", l_actual, l_expected );
    }

    bool l_returnValue = controls_t$destroy( &l_controls );

    ASSERT_TRUE( l_returnValue );
}
