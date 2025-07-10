#include "animation_t.h"

#include "stdfunc.h"
#include "test.h"

TEST( animation_t$create ) {
    animation_t l_animation = animation_t$create();

    // Newly created boxes should have two empty arrays:
    ASSERT_EQ( "%zu", arrayLength( l_animation.keyFrames ), ( size_t )0 );
    ASSERT_EQ( "%zu", arrayLength( l_animation.frames ), ( size_t )0 );

    bool l_returnValue = animation_t$destroy( &l_animation );

    ASSERT_TRUE( l_returnValue );
}

TEST( animation_t$destroy ) {
    // NULL input - should return false
    ASSERT_FALSE( animation_t$destroy( NULL ) );

    {
        animation_t l_animation = animation_t$create();

        // Newly created boxes should have two empty arrays:
        ASSERT_EQ( "%zu", arrayLength( l_animation.keyFrames ), ( size_t )0 );
        ASSERT_EQ( "%zu", arrayLength( l_animation.frames ), ( size_t )0 );

        bool l_returnValue = animation_t$destroy( &l_animation );

        ASSERT_TRUE( l_returnValue );
    }
}
