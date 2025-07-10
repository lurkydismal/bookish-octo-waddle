#include "window_t.h"

#include "test.h"

TEST( window_t$create ) {
    window_t l_window = window_t$create();

    bool l_returnValue = window_t$destroy( &l_window );

    ASSERT_TRUE( l_returnValue );
}

TEST( window_t$destroy ) {
    // Invalid - NULL input
    {
        bool l_returnValue = window_t$destroy( NULL );

        ASSERT_FALSE( l_returnValue );
    }

    // Valid
    {
        window_t l_window = window_t$create();

        bool l_returnValue = window_t$destroy( &l_window );

        ASSERT_TRUE( l_returnValue );
    }
}
