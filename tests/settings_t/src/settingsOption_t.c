#include "settingsOption_t.h"

#include <SDL3/SDL_scancode.h>

#include "stdfloat16.h"
#include "test.h"
#include "vsync.h"

TEST( settingsOption_t$create ) {
    settingsOption_t l_settingsOption = settingsOption_t$create();

    bool l_returnValue = settingsOption_t$destroy( &l_settingsOption );

    ASSERT_TRUE( l_returnValue );
}

TEST( settingsOption_t$destroy ) {
    // NULL input - should return false
    ASSERT_FALSE( settingsOption_t$destroy( NULL ) );

    {
        settingsOption_t l_settingsOption = settingsOption_t$create();

        bool l_returnValue = settingsOption_t$destroy( &l_settingsOption );

        ASSERT_TRUE( l_returnValue );
    }
}

TEST( settingsOption_t$map ) {
    settingsOption_t l_settingsOption = settingsOption_t$create();

    // Valid
    {
        {
            char* l_storage = NULL;

            bool l_returnValue = settingsOption_t$map(
                &l_settingsOption, "window_name", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );

            free( l_storage );
        }

        {
            size_t l_storage = 0;

            bool l_returnValue = settingsOption_t$map(
                &l_settingsOption, "window_width", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }

        {
            float16_t l_storage = 0;

            bool l_returnValue = settingsOption_t$map(
                &l_settingsOption, "spawn_position_x", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }

        {
            bool l_storage = false;

            bool l_returnValue = settingsOption_t$map( &l_settingsOption,
                                                       "has_key", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }

        {
            SDL_Scancode l_storage = 0;

            bool l_returnValue = settingsOption_t$map( &l_settingsOption,
                                                       "scancode", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }

        {
            vsync_t l_storage = ( vsync_t )unknownVsync;

            bool l_returnValue =
                settingsOption_t$map( &l_settingsOption, "vsync", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }

        {
            settingsOption_t* l_storage = NULL;

            bool l_returnValue = settingsOption_t$map(
                &l_settingsOption, "unknown_option", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }
    }

    // Invalid
    {
        // NULL settings option
        {
            bool l_storage = false;

            bool l_returnValue =
                settingsOption_t$map( NULL, "has_key", &l_storage );

            ASSERT_FALSE( l_returnValue );
        }

        // NULL key
        {
            bool l_storage = false;

            bool l_returnValue =
                settingsOption_t$map( &l_settingsOption, NULL, &l_storage );

            ASSERT_FALSE( l_returnValue );
        }

        // NULL storage
        {
            bool l_returnValue =
                settingsOption_t$map( &l_settingsOption, "has_key", NULL );

            ASSERT_FALSE( l_returnValue );
        }
    }

    bool l_returnValue = settingsOption_t$destroy( &l_settingsOption );

    ASSERT_TRUE( l_returnValue );
}

TEST( settingsOption_t$unmap ) {
    settingsOption_t l_settingsOption = settingsOption_t$create();

    // Valid
    {
        {
            char* l_storage = NULL;

            bool l_returnValue = settingsOption_t$map(
                &l_settingsOption, "window_name", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }
    }

    // Invalid
    {
        // NULL settings option
        {
            bool l_returnValue = settingsOption_t$unmap( NULL );

            ASSERT_FALSE( l_returnValue );
        }
    }

    bool l_returnValue = settingsOption_t$destroy( &l_settingsOption );

    ASSERT_TRUE( l_returnValue );
}

TEST( settingsOption_t$bind ) {
    settingsOption_t l_settingsOption = settingsOption_t$create();

    // Valid
    {
        {
            char* l_storage = NULL;

            bool l_returnValue = settingsOption_t$map(
                &l_settingsOption, "window_name", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue = settingsOption_t$bind(
                &l_settingsOption, "window_name", "WindowName" );

            ASSERT_TRUE( l_returnValue );

            ASSERT_STRING_EQ( l_storage, "WindowName" );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );

            free( l_storage );
        }

        {
            size_t l_storage = 0;

            bool l_returnValue = settingsOption_t$map(
                &l_settingsOption, "window_width", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue = settingsOption_t$bind( &l_settingsOption,
                                                   "window_width", "510" );

            ASSERT_TRUE( l_returnValue );

            ASSERT_EQ( "%zu", l_storage, ( size_t )510 );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }

        {
            float16_t l_storage = 0.0f;

            bool l_returnValue = settingsOption_t$map(
                &l_settingsOption, "spawn_position_x", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue = settingsOption_t$bind(
                &l_settingsOption, "spawn_position_x", "9.765" );

            ASSERT_TRUE( l_returnValue );

            ASSERT_EQ( "%f", ( float )l_storage, ( float )( float16_t )9.765f );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }

        {
            bool l_storage = false;

            bool l_returnValue = settingsOption_t$map( &l_settingsOption,
                                                       "has_key", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue =
                settingsOption_t$bind( &l_settingsOption, "has_key", "true" );

            ASSERT_TRUE( l_returnValue );

            ASSERT_TRUE( l_storage );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }

        {
            SDL_Scancode l_storage = 0;

            bool l_returnValue = settingsOption_t$map( &l_settingsOption,
                                                       "scancode", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue =
                settingsOption_t$bind( &l_settingsOption, "scancode", "123" );

            ASSERT_TRUE( l_returnValue );

            ASSERT_EQ( "%u", l_storage, 0 );

            l_returnValue =
                settingsOption_t$bind( &l_settingsOption, "scancode", "R.Alt" );

            ASSERT_TRUE( l_returnValue );

            ASSERT_EQ( "%u", l_storage, SDL_SCANCODE_RALT );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }

        {
            vsync_t l_storage = ( vsync_t )unknownVsync;

            bool l_returnValue =
                settingsOption_t$map( &l_settingsOption, "vsync", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue =
                settingsOption_t$bind( &l_settingsOption, "vsync", "OFF" );

            ASSERT_TRUE( l_returnValue );

            ASSERT_EQ( "%u", l_storage, ( vsync_t )off );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }
    }

    // Invalid
    {
        // NULL settings option
        {
            char* l_storage = NULL;

            bool l_returnValue = settingsOption_t$map(
                &l_settingsOption, "window_name", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue =
                settingsOption_t$bind( NULL, "window_name", "WindowName" );

            ASSERT_FALSE( l_returnValue );

            ASSERT_EQ( "%p", l_storage, NULL );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }

        // NULL key
        {
            char* l_storage = NULL;

            bool l_returnValue = settingsOption_t$map(
                &l_settingsOption, "window_name", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue =
                settingsOption_t$bind( &l_settingsOption, NULL, "WindowName" );

            ASSERT_FALSE( l_returnValue );

            ASSERT_EQ( "%p", l_storage, NULL );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }

        // NULL value
        {
            char* l_storage = NULL;

            bool l_returnValue = settingsOption_t$map(
                &l_settingsOption, "window_name", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue =
                settingsOption_t$bind( &l_settingsOption, "window_name", NULL );

            ASSERT_EQ( "%p", l_storage, NULL );

            ASSERT_FALSE( l_returnValue );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }

        // Not mapped key
        {
            bool l_storage = false;

            bool l_returnValue = settingsOption_t$map( &l_settingsOption,
                                                       "has_key", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue =
                settingsOption_t$bind( &l_settingsOption, "wrong_key", "true" );

            ASSERT_FALSE( l_returnValue );

            // Old value
            ASSERT_FALSE( l_storage );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }

        // Unkown type
        {
            settingsOption_t* l_storage = NULL;

            bool l_returnValue = settingsOption_t$map(
                &l_settingsOption, "unknown_option", &l_storage );

            ASSERT_TRUE( l_returnValue );

            l_returnValue = settingsOption_t$bind( &l_settingsOption,
                                                   "unknown_option", "TEST" );

            ASSERT_FALSE( l_returnValue );

            ASSERT_EQ( "%p", l_storage, NULL );

            l_returnValue = settingsOption_t$unmap( &l_settingsOption );

            ASSERT_TRUE( l_returnValue );
        }
    }

    bool l_returnValue = settingsOption_t$destroy( &l_settingsOption );

    ASSERT_TRUE( l_returnValue );
}
