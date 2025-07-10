#include "state_t.h"

#include "test.h"

TEST( state_t$create ) {
    state_t l_state = state_t$create();

    ASSERT_EQ( "%p", l_state.renderer, NULL );

    bool l_returnValue = state_t$destroy( &l_state );

    ASSERT_TRUE( l_returnValue );
}

TEST( state_t$destroy ) {
    // NULL input - should return false
    ASSERT_FALSE( state_t$destroy( NULL ) );

    {
        state_t l_state = state_t$create();

        ASSERT_EQ( "%p", l_state.renderer, NULL );

        bool l_returnValue = state_t$destroy( &l_state );

        ASSERT_TRUE( l_returnValue );
    }
}
