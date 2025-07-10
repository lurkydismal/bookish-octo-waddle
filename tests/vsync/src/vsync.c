#include "vsync.h"

#include "test.h"

TEST( vsync$convert$toStaticString ) {
    // off - "OFF"
    {
        const char* l_vsyncAsString =
            vsync$convert$toStaticString( ( vsync_t )off );

        ASSERT_NOT_EQ( "%p", l_vsyncAsString, NULL );
        ASSERT_STRING_EQ( l_vsyncAsString, VSYNC_TYPE_AS_STRING_OFF );
    }

    // out-of-range ( negative ) - "UNKNOWN"
    {
        const char* l_vsyncAsString =
            vsync$convert$toStaticString( ( vsync_t )( -1 ) );

        ASSERT_NOT_EQ( "%p", l_vsyncAsString, NULL );
        ASSERT_STRING_EQ( l_vsyncAsString, VSYNC_TYPE_AS_STRING_UNKNOWN );
    }

    // out-of-range ( arbitrary ) - "UNKNOWN"
    {
        const char* l_vsyncAsString =
            vsync$convert$toStaticString( ( vsync_t )42 );

        ASSERT_NOT_EQ( "%p", l_vsyncAsString, NULL );
        ASSERT_STRING_EQ( l_vsyncAsString, VSYNC_TYPE_AS_STRING_UNKNOWN );
    }
}

TEST( vsync_t$convert$fromString ) {
    // NULL input - unknownVsync
    {
        const vsync_t l_vsync = vsync_t$convert$fromString( NULL );

        ASSERT_EQ( "%d", l_vsync, ( vsync_t )unknownVsync );
    }

    // "OFF" - off
    {
        const vsync_t l_vsync =
            vsync_t$convert$fromString( VSYNC_TYPE_AS_STRING_OFF );

        ASSERT_EQ( "%d", l_vsync, ( vsync_t )off );
    }

    // arbitrary string - unknownVsync
    {
        const vsync_t l_vsync = vsync_t$convert$fromString( "FOO" );

        ASSERT_EQ( "%d", l_vsync, ( vsync_t )unknownVsync );
    }
}

TEST( vsync$init ) {
    // NULL renderer - fail
    {
        bool l_returnValue = vsync$init( ( vsync_t )off, 60, NULL );

        ASSERT_EQ( "%d", l_returnValue, false );
    }
}
