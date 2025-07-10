#include "player_t.h"

#include "test.h"

TEST( player_t$create ) {
    player_t l_player = player_t$create();

    bool l_returnValue = player_t$destroy( &l_player );

    ASSERT_TRUE( l_returnValue );
}

TEST( player_t$destroy ) {
    // Invalid - NULL input
    {
        bool l_returnValue = player_t$destroy( NULL );

        ASSERT_FALSE( l_returnValue );
    }

    // Valid
    {
        player_t l_player = player_t$create();

        bool l_returnValue = player_t$destroy( &l_player );

        ASSERT_TRUE( l_returnValue );
    }
}

TEST( player_t$step ) {
    // NULL pointer must return false
    {
        bool l_returnValue = player_t$step( NULL, 1.0f, 1.0f );

        ASSERT_FALSE( l_returnValue );
    }

    // Set up a single object for all fuzz iterations
    player_t l_player = player_t$create();

    {
        l_player.object.worldXMin = -50.0f;
        l_player.object.worldXMax = 50.0f;

        l_player.object.worldYMin = -25.0f;
        l_player.object.worldYMax = 25.0f;
    }

    // Zero movement should succeed and leave position unchanged
    {
        float l_oldX = l_player.object.worldX;
        float l_oldY = l_player.object.worldY;

        ASSERT_FALSE( player_t$step( &l_player, 0.0f, 0.0f ) );

        ASSERT_EQ( "%f", l_oldX, l_player.object.worldX );
        ASSERT_EQ( "%f", l_oldY, l_player.object.worldY );
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
        float l_oldX = l_player.object.worldX;
        float l_oldY = l_player.object.worldY;

        // Compute expected
        float l_velocityX = ( l_oldX + l_destinationX );
        float l_velocityY = ( l_oldY + l_destinationY );

        // Call and verify
        bool l_returnValue =
            player_t$step( &l_player, l_velocityX, l_velocityY );

        ASSERT_FALSE( l_returnValue );

        // Allow tiny float‐rounding slack
        ASSERT_EQ( "%f", l_player.object.worldX, l_oldX );
        ASSERT_EQ( "%f", l_player.object.worldY, l_oldY );
    }

    bool l_returnValue = player_t$destroy( &l_player );

    ASSERT_TRUE( l_returnValue );
}

TEST( player_t$render ) {
    player_t l_player = player_t$create();

    // Invalid
    {
        // No draw boxes
        {
            // NULL player
            {
                const SDL_FRect l_cameraRectangle = { 0, 0, 1, 1 };

                bool l_returnValue =
                    player_t$render( NULL, &l_cameraRectangle, false );

                ASSERT_FALSE( l_returnValue );
            }

            // NULL camera
            {
                bool l_returnValue = player_t$render( &l_player, NULL, false );

                ASSERT_FALSE( l_returnValue );
            }
        }

        // Do draw boxes
        {
            // NULL player
            {
                const SDL_FRect l_cameraRectangle = { 0, 0, 1, 1 };

                bool l_returnValue =
                    player_t$render( NULL, &l_cameraRectangle, true );

                ASSERT_FALSE( l_returnValue );
            }

            // NULL camera
            {
                bool l_returnValue = player_t$render( &l_player, NULL, true );

                ASSERT_FALSE( l_returnValue );
            }
        }
    }

    bool l_returnValue = player_t$destroy( &l_player );

    ASSERT_TRUE( l_returnValue );
}
