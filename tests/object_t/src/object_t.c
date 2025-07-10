#include "object_t.h"

#include "stdfunc.h"
#include "test.h"

TEST( object_t$create ) {
    object_t l_object = object_t$create();

    bool l_returnValue = object_t$destroy( &l_object );

    ASSERT_TRUE( l_returnValue );
}

TEST( object_t$destroy ) {
    // NULL input - should return false
    ASSERT_FALSE( object_t$destroy( NULL ) );

    {
        object_t l_object = object_t$create();

        bool l_returnValue = object_t$destroy( &l_object );

        ASSERT_TRUE( l_returnValue );
    }
}

TEST( object_t$move ) {
    // NULL pointer must return false
    {
        bool l_returnValue = object_t$move( NULL, 1.0f, 1.0f );

        ASSERT_FALSE( l_returnValue );
    }

    // Set up a single object for all fuzz iterations
    object_t l_object = object_t$create();

    {
        l_object.worldXMin = -50.0f;
        l_object.worldXMax = 50.0f;

        l_object.worldYMin = -25.0f;
        l_object.worldYMax = 25.0f;
    }

    // Zero movement should succeed and leave position unchanged
    {
        float l_oldX = l_object.worldX;
        float l_oldY = l_object.worldY;

        ASSERT_TRUE( object_t$move( &l_object, 0.0f, 0.0f ) );

        ASSERT_EQ( "%f", l_oldX, l_object.worldX );
        ASSERT_EQ( "%f", l_oldY, l_object.worldY );
    }

    // Fuzz: 1000 random moves
    FOR_RANGE( size_t, 0, 1000 ) {
        // Random delta in [-100, +100)
        float l_destinationX =
            ( ( ( randomNumber() / ( float )RANDOM_NUMBER_MAX ) - 0.5f ) *
              200.0f );
        float l_destinationY =
            ( ( ( randomNumber() / ( float )RANDOM_NUMBER_MAX ) - 0.5f ) *
              200.0f );

        // Remember old
        float l_oldX = l_object.worldX;
        float l_oldY = l_object.worldY;

        // Compute expected, with clamp
        float l_expectedX = l_oldX + l_destinationX;

        if ( l_expectedX < l_object.worldXMin ) {
            l_expectedX = l_object.worldXMin;
        }

        if ( l_expectedX > l_object.worldXMax ) {
            l_expectedX = l_object.worldXMax;
        }

        float l_expectedY = ( l_oldY + l_destinationY );

        if ( l_expectedY < l_object.worldYMin ) {
            l_expectedY = l_object.worldYMin;
        }

        if ( l_expectedY > l_object.worldYMax ) {
            l_expectedY = l_object.worldYMax;
        }

        // Call and verify
        bool l_returnValue =
            object_t$move( &l_object, l_destinationX, l_destinationY );

        ASSERT_TRUE( l_returnValue );

        // Allow tiny float‐rounding slack
        ASSERT_TRUE( __builtin_fabsf( l_object.worldX - l_expectedX ) < 1e-6f );
        ASSERT_TRUE( __builtin_fabsf( l_object.worldY - l_expectedY ) < 1e-6f );
    }

    bool l_returnValue = object_t$destroy( &l_object );

    ASSERT_TRUE( l_returnValue );
}

TEST( object_t$step ) {
    // NULL pointer must return false
    {
        bool l_returnValue = object_t$step( NULL, 1.0f, 1.0f );

        ASSERT_FALSE( l_returnValue );
    }

    // Set up a single object for all fuzz iterations
    object_t l_object = object_t$create();

    {
        l_object.worldXMin = -50.0f;
        l_object.worldXMax = 50.0f;

        l_object.worldYMin = -25.0f;
        l_object.worldYMax = 25.0f;
    }

    // Zero movement should succeed and leave position unchanged
    {
        float l_oldX = l_object.worldX;
        float l_oldY = l_object.worldY;

        ASSERT_FALSE( object_t$step( &l_object, 0.0f, 0.0f ) );

        ASSERT_EQ( "%f", l_oldX, l_object.worldX );
        ASSERT_EQ( "%f", l_oldY, l_object.worldY );
    }

    // Fuzz: 1000 random moves - should fail
    FOR_RANGE( size_t, 0, 1000 ) {
        // Random delta in [-100, +100)
        float l_destinationX =
            ( ( ( randomNumber() / ( float )RANDOM_NUMBER_MAX ) - 0.5f ) *
              200.0f );
        float l_destinationY =
            ( ( ( randomNumber() / ( float )RANDOM_NUMBER_MAX ) - 0.5f ) *
              200.0f );

        // Remember old
        float l_oldX = l_object.worldX;
        float l_oldY = l_object.worldY;

        // Compute expected
        float l_velocityX = ( l_oldX + l_destinationX );
        float l_velocityY = ( l_oldY + l_destinationY );

        // Call and verify
        bool l_returnValue =
            object_t$step( &l_object, l_velocityX, l_velocityY );

        ASSERT_FALSE( l_returnValue );

        // Allow tiny float‐rounding slack
        ASSERT_EQ( "%f", l_object.worldX, l_oldX );
        ASSERT_EQ( "%f", l_object.worldY, l_oldY );
    }

    bool l_returnValue = object_t$destroy( &l_object );

    ASSERT_TRUE( l_returnValue );
}

TEST( object_t$render ) {
    object_t l_object = object_t$create();

    // Invalid
    {
        // No draw boxes
        {
            // NULL object
            {
                const SDL_FRect l_cameraRectangle = { 0, 0, 1, 1 };

                bool l_returnValue =
                    object_t$render( NULL, &l_cameraRectangle, false );

                ASSERT_FALSE( l_returnValue );
            }

            // NULL camera
            {
                bool l_returnValue = object_t$render( &l_object, NULL, false );

                ASSERT_FALSE( l_returnValue );
            }
        }

        // Do draw boxes
        {
            // NULL object
            {
                const SDL_FRect l_cameraRectangle = { 0, 0, 1, 1 };

                bool l_returnValue =
                    object_t$render( NULL, &l_cameraRectangle, true );

                ASSERT_FALSE( l_returnValue );
            }

            // NULL camera
            {
                bool l_returnValue = object_t$render( &l_object, NULL, true );

                ASSERT_FALSE( l_returnValue );
            }
        }
    }

    bool l_returnValue = object_t$destroy( &l_object );

    ASSERT_TRUE( l_returnValue );
}
