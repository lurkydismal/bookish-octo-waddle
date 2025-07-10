#include "settings_t.h"

#include "test.h"

TEST( settings_t$create ) {
    settings_t l_settings = settings_t$create();

    bool l_returnValue = settings_t$destroy( &l_settings );

    ASSERT_TRUE( l_returnValue );
}

TEST( settings_t$destroy ) {
    // NULL input - should return false
    ASSERT_FALSE( settings_t$destroy( NULL ) );

    {
        settings_t l_settings = settings_t$create();

        bool l_returnValue = settings_t$destroy( &l_settings );

        ASSERT_TRUE( l_returnValue );
    }
}
