#include "asset_t.h"

#include "test.h"

TEST( asset_t$loader$init ) {
    // Valid string
    {
        bool l_returnValue = asset_t$loader$init( "WXEQWEXQWEWQEXQEW" );

        ASSERT_TRUE( l_returnValue );

        l_returnValue = asset_t$loader$quit();

        ASSERT_TRUE( l_returnValue );
    }

    // Duplicate init
    {
        bool l_returnValue = asset_t$loader$init( "ASDXAXDQWEWADACSDASX" );

        ASSERT_TRUE( l_returnValue );

        l_returnValue = asset_t$loader$init( "ASDQWXQWEQRVEWCRWER" );

        ASSERT_FALSE( l_returnValue );

        l_returnValue = asset_t$loader$quit();

        ASSERT_TRUE( l_returnValue );
    }

    // NULL input - should fail
    {
        bool l_returnValue = asset_t$loader$init( NULL );

        ASSERT_FALSE( l_returnValue );
    }
}
