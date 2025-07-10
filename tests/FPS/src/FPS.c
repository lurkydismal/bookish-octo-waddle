#include "FPS.h"

#include "test.h"

TEST( FPS$init ) {
    size_t l_totalFrames = 0;

    {
        bool l_returnValue = FPS$init( &l_totalFrames );

        ASSERT_TRUE( l_returnValue );

        // Clean up
        l_returnValue = FPS$quit();

        ASSERT_TRUE( l_returnValue );
    }

    // Init without quitting should fail
    {
        bool l_returnValue = FPS$init( &l_totalFrames );

        ASSERT_TRUE( l_returnValue );

        l_returnValue = FPS$init( &l_totalFrames );

        ASSERT_FALSE( l_returnValue );

        l_returnValue = FPS$quit();

        ASSERT_TRUE( l_returnValue );
    }
}
