#include "input_t.h"

#include "test.h"

TEST( input_t$convert$toStaticString ) {
    // clang-format off
    direction_t l_directions[] = {
        UP, DOWN, LEFT, RIGHT,
    };

    button_t l_buttons[] = {
        A, B, C, D,
    };

    const char l_directionsConverted[] = {
        '8',  // UP
        '2',  // DOWN
        '4',  // LEFT
        '6',  // RIGHT
    };

    const char l_buttonsConverted[] = {
        'A', 'B', 'C', 'D',
    };
    // clang-format on

    {
        const input_t l_input = MAKE_INPUT( ( UP | DOWN ), ( A | B | C ) );

        ASSERT_STRING_EQ( input_t$convert$toStaticString( &l_input ), "82ABC" );
    }

    {
        const input_t l_input = MAKE_INPUT( ( LEFT | RIGHT ), ( A | B | D ) );

        ASSERT_STRING_EQ( input_t$convert$toStaticString( &l_input ), "46ABD" );
    }

    {
        const input_t l_input = MAKE_INPUT( ( DOWN | RIGHT ), ( A ) );

        ASSERT_STRING_EQ( input_t$convert$toStaticString( &l_input ), "3A" );
    }

    {
        char l_expected[ 3 ];

        l_expected[ 2 ] = '\0';

        FOR_RANGE( size_t, 0, arrayLengthNative( l_directions ) ) {
            const size_t l_directionIndex = _index;

            FOR_RANGE( size_t, 0, arrayLengthNative( l_buttons ) ) {
                const size_t l_buttonIndex = _index;

                l_expected[ 0 ] = l_directionsConverted[ l_directionIndex ];
                l_expected[ 1 ] = l_buttonsConverted[ l_buttonIndex ];

                const input_t l_input =
                    MAKE_INPUT( l_directions[ l_directionIndex ],
                                l_buttons[ l_buttonIndex ] );

                const char* l_actual =
                    input_t$convert$toStaticString( &l_input );

                ASSERT_STRING_EQ( l_actual, l_expected );
            }
        }
    }
}
