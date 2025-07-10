#include "camera_t.h"

#include "test.h"

TEST( camera_t$create ) {
    camera_t l_camera = camera_t$create();

    bool l_returnValue = camera_t$destroy( &l_camera );

    ASSERT_TRUE( l_returnValue );
}

TEST( camera_t$destroy ) {
    // NULL input - should return false
    ASSERT_FALSE( camera_t$destroy( NULL ) );

    {
        camera_t l_camera = camera_t$create();

        bool l_returnValue = camera_t$destroy( &l_camera );

        ASSERT_TRUE( l_returnValue );
    }
}

TEST( camera_t$update ) {
    camera_t l_camera = camera_t$create();

    // Valid
    {
        player_t l_player = player_t$create();

        bool l_returnValue = player_t$destroy( &l_player );

        ASSERT_TRUE( l_returnValue );
    }

    // Camera NULL input - should return false
    {
        player_t l_player = player_t$create();

        bool l_returnValue = camera_t$update( NULL, &l_player );

        ASSERT_FALSE( l_returnValue );

        l_returnValue = player_t$destroy( &l_player );

        ASSERT_TRUE( l_returnValue );
    }

    // Player NULL input - should return false
    {
        bool l_returnValue = camera_t$update( &l_camera, NULL );

        ASSERT_FALSE( l_returnValue );
    }

    bool l_returnValue = camera_t$destroy( &l_camera );

    ASSERT_TRUE( l_returnValue );
}
