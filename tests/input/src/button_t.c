#include "button_t.h"

#include "test.h"

TEST( button_t$convert$toStaticString ) {
    // clang-format off
    button_t l_buttons[] = {
        A, B, C, D,
        (A|B), (A|C), (A|D),
        (B|C), (B|D),
        (C|D),
        (A|B|C), (A|C|D), (B|C|D),
        (A|B|C|D)
    };
    // clang-format on

    // clang-format off
    const char* l_buttonsConverted[] = {
        "A", "B", "C", "D",
        "AB", "AC", "AD",
        "BC", "BD",
        "CD",
        "ABC", "ACD", "BCD",
        "ABCD"
    };
    // clang-format on

    FOR_RANGE( size_t, 0, arrayLengthNative( l_buttons ) ) {
        ASSERT_STRING_EQ(
            button_t$convert$toStaticString( l_buttons[ _index ] ),
            l_buttonsConverted[ _index ] );
    }
}
