#include "direction_t.h"

#include "test.h"

TEST( direction_t$convert$toStaticString ) {
    // clang-format off
    direction_t l_directions[] = {
        UP, DOWN, LEFT, RIGHT,
        UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT,
        (UP | DOWN),
        (LEFT | RIGHT),
        (UP | DOWN | LEFT),
        (UP | DOWN | RIGHT),
        (LEFT | RIGHT | UP),
        (LEFT | RIGHT | DOWN),
        (UP | DOWN | LEFT | RIGHT),
        0
    };
    // clang-format on

    // clang-format off
    const char* l_directionsConverted[] = {
        "8",  // UP
        "2",  // DOWN
        "4",  // LEFT
        "6",  // RIGHT
        "7",  // UP_LEFT
        "9",  // UP_RIGHT
        "1",  // DOWN_LEFT
        "3",  // DOWN_RIGHT
        "82", // UP | DOWN
        "46", // LEFT | RIGHT
        "72", // UP | DOWN | LEFT
        "92", // UP | DOWN | RIGHT
        "76", // LEFT | RIGHT | UP
        "16", // LEFT | RIGHT | DOWN
        "73", // UP | DOWN | LEFT | RIGHT
        "5"   // No direction
    };
    // clang-format on

    FOR_RANGE( size_t, 0, arrayLengthNative( l_directions ) ) {
        ASSERT_STRING_EQ(
            direction_t$convert$toStaticString( l_directions[ _index ] ),
            l_directionsConverted[ _index ] );
    }
}
