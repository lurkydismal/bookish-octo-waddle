#include "color_t.h"

#include "test.h"

TEST( color_t$convert$fromString ) {
    const color_t l_colorDefault = DEFAULT_COLOR;

    // Valid 6-digit hex ( alpha stays DEFAULT_COLOR.alpha )
    {
        const color_t l_color = color_t$convert$fromString( "ABCDEF" );

        ASSERT_EQ( "%x", l_color.red, 0xAB );
        ASSERT_EQ( "%x", l_color.green, 0xCD );
        ASSERT_EQ( "%x", l_color.blue, 0xEF );
        ASSERT_EQ( "%x", l_color.alpha, l_colorDefault.alpha );
    }

    // Empty string - all channels zero ( alpha stays DEFAULT_COLOR.alpha )
    {
        const color_t l_color = color_t$convert$fromString( "" );

        ASSERT_EQ( "%x", l_color.red, l_colorDefault.red );
        ASSERT_EQ( "%x", l_color.green, l_colorDefault.green );
        ASSERT_EQ( "%x", l_color.blue, l_colorDefault.blue );
        ASSERT_EQ( "%x", l_color.alpha, l_colorDefault.alpha );
    }

    // Invalid hex - all channels zero ( alpha stays DEFAULT_COLOR.alpha )
    {
        const color_t l_color = color_t$convert$fromString( "ZZZZ" );

        ASSERT_EQ( "%x", l_color.red, l_colorDefault.red );
        ASSERT_EQ( "%x", l_color.green, l_colorDefault.green );
        ASSERT_EQ( "%x", l_color.blue, l_colorDefault.blue );
        ASSERT_EQ( "%x", l_color.alpha, l_colorDefault.alpha );
    }

    // NULL input - DEFAULT_COLOR entirely
    {
        const color_t l_color = color_t$convert$fromString( NULL );

        ASSERT_EQ( "%x", l_color.red, l_colorDefault.red );
        ASSERT_EQ( "%x", l_color.green, l_colorDefault.green );
        ASSERT_EQ( "%x", l_color.blue, l_colorDefault.blue );
        ASSERT_EQ( "%x", l_color.alpha, l_colorDefault.alpha );
    }
}

TEST( color_t$convert$toStaticString ) {
    // Valid color - 8-digit hex string
    {
        const color_t l_color = { 0xAB, 0xCD, 0xEF, 0x12 };

        const char* l_colorAsString =
            color_t$convert$toStaticString( &l_color );

        ASSERT_NOT_EQ( "%p", l_colorAsString, NULL );
        ASSERT_STRING_EQ( l_colorAsString, "ABCDEF12" );
    }

    // NULL input - returns NULL
    {
        const char* l_colorAsString = color_t$convert$toStaticString( NULL );

        ASSERT_STRING_EQ( l_colorAsString, "" );
    }
}
