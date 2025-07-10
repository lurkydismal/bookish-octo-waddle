#include "boxes_t.h"

#include "stdfunc.h"
#include "test.h"

TEST( boxes_t$create ) {
    boxes_t l_boxes = boxes_t$create();

    // Newly created boxes should have two empty arrays:
    ASSERT_EQ( "%zu", arrayLength( l_boxes.keyFrames ), ( size_t )0 );
    ASSERT_EQ( "%zu", arrayLength( l_boxes.frames ), ( size_t )0 );

    bool l_returnValue = boxes_t$destroy( &l_boxes );

    ASSERT_TRUE( l_returnValue );
}

TEST( boxes_t$destroy ) {
    // NULL input - should return false
    ASSERT_FALSE( boxes_t$destroy( NULL ) );

    {
        boxes_t l_boxes = boxes_t$create();

        // Newly created boxes should have two empty arrays:
        ASSERT_EQ( "%zu", arrayLength( l_boxes.keyFrames ), ( size_t )0 );
        ASSERT_EQ( "%zu", arrayLength( l_boxes.frames ), ( size_t )0 );

        bool l_returnValue = boxes_t$destroy( &l_boxes );

        ASSERT_TRUE( l_returnValue );
    }
}

TEST( boxes_t$load$one ) {
    boxes_t l_boxes = boxes_t$create();
    const SDL_FRect l_targetRectangle = {
        .x = 1.0f, .y = 2.0f, .w = 3.0f, .h = 4.0f };

    // Invalid
    {
        // NULL boxes pointer - false
        ASSERT_FALSE( boxes_t$load$one( NULL, &l_targetRectangle, 50, 200 ) );

        // NULL rectangle - false
        ASSERT_FALSE( boxes_t$load$one( &l_boxes, NULL, 100, 200 ) );

        // startIndex > endIndex - false
        ASSERT_FALSE( boxes_t$load$one( &l_boxes, &l_targetRectangle, 2, 1 ) );

        // startIndex == 0 - false
        ASSERT_FALSE( boxes_t$load$one( &l_boxes, &l_targetRectangle, 0, 2 ) );

        // endIndex == 0 - false
        ASSERT_FALSE( boxes_t$load$one( &l_boxes, &l_targetRectangle, 1, 0 ) );
    }

    // Valid
    {
        // First box
        {
            // Load frames from 1 up to 3
            bool l_returnValue =
                boxes_t$load$one( &l_boxes, &l_targetRectangle, 1, 3 );

            ASSERT_TRUE( l_returnValue );

            // Should have inserted exactly one keyFrame
            ASSERT_EQ( "%zu", arrayLength( l_boxes.keyFrames ), ( size_t )1 );

            // The stored SDL_FRect should match the input l_targetRectangle
            {
                const SDL_FRect* l_targetRectangleStored =
                    l_boxes.keyFrames[ 0 ];

                ASSERT_EQ( "%f", l_targetRectangleStored->x,
                           l_targetRectangle.x );
                ASSERT_EQ( "%f", l_targetRectangleStored->y,
                           l_targetRectangle.y );
                ASSERT_EQ( "%f", l_targetRectangleStored->w,
                           l_targetRectangle.w );
                ASSERT_EQ( "%f", l_targetRectangleStored->h,
                           l_targetRectangle.h );
            }

            // The frames array should have indices 1 and 2
            ASSERT_EQ( "%zu", arrayLength( l_boxes.frames ), ( size_t )3 );

            // For each index from 1 up to 3 we should have recorded a frame
            // index of 0
            FOR_ARRAY( size_t* const*, l_boxes.frames ) {
                // Each l_boxes.frames[ _index ] is itself an array of size_t
                ASSERT_EQ( "%zu", arrayLength( *_element ), ( size_t )1 );

                const size_t l_frameIndex = ( *_element )[ 0 ];

                ASSERT_EQ( "%zu", l_frameIndex, ( size_t )0 );
            }
        }

        // Second box
        // No overlapping
        {
            const SDL_FRect l_targetRectangle = {
                .x = 5.0f, .y = 6.0f, .w = 7.0f, .h = 8.0f };

            // Load frames from 3 up to 5
            bool l_returnValue =
                boxes_t$load$one( &l_boxes, &l_targetRectangle, 3, 5 );

            ASSERT_TRUE( l_returnValue );

            // Should have inserted exactly two keyFrames
            ASSERT_EQ( "%zu", arrayLength( l_boxes.keyFrames ), ( size_t )2 );

            // The stored SDL_FRect should match the input l_targetRectangle
            {
                const SDL_FRect* l_targetRectangleStored =
                    l_boxes.keyFrames[ 1 ];

                ASSERT_EQ( "%f", l_targetRectangleStored->x,
                           l_targetRectangle.x );
                ASSERT_EQ( "%f", l_targetRectangleStored->y,
                           l_targetRectangle.y );
                ASSERT_EQ( "%f", l_targetRectangleStored->w,
                           l_targetRectangle.w );
                ASSERT_EQ( "%f", l_targetRectangleStored->h,
                           l_targetRectangle.h );
            }

            // The frames array should have indices 1, 2, 3, 4 and 5
            ASSERT_EQ( "%zu", arrayLength( l_boxes.frames ), ( size_t )5 );

            // For each index from 3 to 5 we should have recorded a frame index
            // of 0
            {
                size_t* const* l_frames = l_boxes.frames;

                l_frames++;
                l_frames++;

                // Index 3
                {
                    // Each l_boxes.frames[ _index ] is itself an array of
                    // size_t
                    ASSERT_EQ( "%zu", arrayLength( *l_frames ), ( size_t )2 );

                    size_t l_frameIndex = ( *l_frames )[ 0 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )0 );

                    l_frameIndex = ( *l_frames )[ 1 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )1 );
                }

                l_frames++;

                // Index 4
                {
                    // Each l_boxes.frames[ _index ] is itself an array of
                    // size_t
                    ASSERT_EQ( "%zu", arrayLength( *l_frames ), ( size_t )1 );

                    const size_t l_frameIndex = ( *l_frames )[ 0 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )1 );
                }

                l_frames++;

                // Index 5
                {
                    // Each l_boxes.frames[ _index ] is itself an array of
                    // size_t
                    ASSERT_EQ( "%zu", arrayLength( *l_frames ), ( size_t )1 );

                    const size_t l_frameIndex = ( *l_frames )[ 0 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )1 );
                }
            }
        }

        // Third box
        // Overlapping
        {
            const SDL_FRect l_targetRectangle = {
                .x = 9.0f, .y = 10.0f, .w = 11.0f, .h = -1.0f };

            // Load frames from 2 up to 4
            bool l_returnValue =
                boxes_t$load$one( &l_boxes, &l_targetRectangle, 2, 4 );

            ASSERT_TRUE( l_returnValue );

            // Should have inserted exactly three keyFrames
            ASSERT_EQ( "%zu", arrayLength( l_boxes.keyFrames ), ( size_t )3 );

            // The stored SDL_FRect should match the input l_targetRectangle
            {
                const SDL_FRect* l_targetRectangleStored =
                    l_boxes.keyFrames[ 2 ];

                ASSERT_EQ( "%f", l_targetRectangleStored->x,
                           l_targetRectangle.x );
                ASSERT_EQ( "%f", l_targetRectangleStored->y,
                           l_targetRectangle.y );
                ASSERT_EQ( "%f", l_targetRectangleStored->w,
                           l_targetRectangle.w );
                ASSERT_EQ( "%f", l_targetRectangleStored->h,
                           l_targetRectangle.h );
            }

            // The frames array should have indices 1, 2, 3, 4 and 5
            ASSERT_EQ( "%zu", arrayLength( l_boxes.frames ), ( size_t )5 );

            // For each index from 2 to 4 we should have recorded a frame index
            // of 0
            {
                size_t* const* l_frames = l_boxes.frames;

                l_frames++;

                // Index 2
                {
                    // Each l_boxes.frames[ _index ] is itself an array of
                    // size_t
                    ASSERT_EQ( "%zu", arrayLength( *l_frames ), ( size_t )2 );

                    size_t l_frameIndex = ( *l_frames )[ 0 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )0 );

                    l_frameIndex = ( *l_frames )[ 1 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )2 );
                }

                l_frames++;

                // Index 3
                {
                    // Each l_boxes.frames[ _index ] is itself an array of
                    // size_t
                    ASSERT_EQ( "%zu", arrayLength( *l_frames ), ( size_t )3 );

                    size_t l_frameIndex = ( *l_frames )[ 0 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )0 );

                    l_frameIndex = ( *l_frames )[ 1 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )1 );

                    l_frameIndex = ( *l_frames )[ 2 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )2 );
                }
            }
        }
    }

    bool l_returnValue = boxes_t$unload( &l_boxes );

    ASSERT_TRUE( l_returnValue );

    l_returnValue = boxes_t$destroy( &l_boxes );

    ASSERT_TRUE( l_returnValue );
}

TEST( boxes_t$load$one$fromString ) {
    boxes_t l_boxes = boxes_t$create();

    // Invalid
    {
        // NULL boxes pointer - false
        ASSERT_FALSE( boxes_t$load$one$fromString( NULL, "1 2 3 4 50-200" ) );

        // No rectangle - false
        ASSERT_FALSE( boxes_t$load$one$fromString( &l_boxes, " 100-200" ) );

        // startIndex > endIndex - false
        ASSERT_FALSE( boxes_t$load$one$fromString( &l_boxes, "1 2 3 4 2-1" ) );

        // startIndex == 0 - false
        ASSERT_FALSE( boxes_t$load$one$fromString( &l_boxes, "1 2 3 4 0-2" ) );

        // endIndex == 0 - false
        ASSERT_FALSE( boxes_t$load$one$fromString( &l_boxes, "1 2 3 4 1-0" ) );
    }

    // Valid
    {
        // First box
        {
            // Load frames from 1 up to 3
            bool l_returnValue =
                boxes_t$load$one$fromString( &l_boxes, "1 2 3 4 1-3" );

            ASSERT_TRUE( l_returnValue );

            // Should have inserted exactly one keyFrame
            ASSERT_EQ( "%zu", arrayLength( l_boxes.keyFrames ), ( size_t )1 );

            // The stored SDL_FRect should match the input target rectangle
            {
                const SDL_FRect* l_targetRectangleStored =
                    l_boxes.keyFrames[ 0 ];

                ASSERT_EQ( "%f", l_targetRectangleStored->x, 1.0f );
                ASSERT_EQ( "%f", l_targetRectangleStored->y, 2.0f );
                ASSERT_EQ( "%f", l_targetRectangleStored->w, 3.0f );
                ASSERT_EQ( "%f", l_targetRectangleStored->h, 4.0f );
            }

            // The frames array should have indices 1 2 and 3
            ASSERT_EQ( "%zu", arrayLength( l_boxes.frames ), ( size_t )3 );

            // For each index from 1 to 2 we should have recorded a frame index
            // of 0
            FOR_ARRAY( size_t* const*, l_boxes.frames ) {
                // Each l_boxes.frames[ _index ] is itself an array of size_t
                ASSERT_EQ( "%zu", arrayLength( *_element ), ( size_t )1 );

                const size_t l_frameIndex = ( *_element )[ 0 ];

                ASSERT_EQ( "%zu", l_frameIndex, ( size_t )0 );
            }
        }

        // Second box
        // No overlapping
        {
            // Load frames from 3 up to 5
            bool l_returnValue =
                boxes_t$load$one$fromString( &l_boxes, "5 6 7 8 3-5" );

            ASSERT_TRUE( l_returnValue );

            // Should have inserted exactly two keyFrames
            ASSERT_EQ( "%zu", arrayLength( l_boxes.keyFrames ), ( size_t )2 );

            // The stored SDL_FRect should match the input target rectangle
            {
                const SDL_FRect* l_targetRectangleStored =
                    l_boxes.keyFrames[ 1 ];

                ASSERT_EQ( "%f", l_targetRectangleStored->x, 5.0f );
                ASSERT_EQ( "%f", l_targetRectangleStored->y, 6.0f );
                ASSERT_EQ( "%f", l_targetRectangleStored->w, 7.0f );
                ASSERT_EQ( "%f", l_targetRectangleStored->h, 8.0f );
            }

            // The frames array should have indices 1, 2, 3, 4 and 5
            ASSERT_EQ( "%zu", arrayLength( l_boxes.frames ), ( size_t )5 );

            // For each index from 3 to 4 we should have recorded a frame index
            // of 0
            {
                size_t* const* l_frames = l_boxes.frames;

                l_frames++;
                l_frames++;

                // Index 3
                {
                    // Each l_boxes.frames[ _index ] is itself an array of
                    // size_t
                    ASSERT_EQ( "%zu", arrayLength( *l_frames ), ( size_t )2 );

                    size_t l_frameIndex = ( *l_frames )[ 0 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )0 );

                    l_frameIndex = ( *l_frames )[ 1 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )1 );
                }

                l_frames++;

                // Index 4
                {
                    // Each l_boxes.frames[ _index ] is itself an array of
                    // size_t
                    ASSERT_EQ( "%zu", arrayLength( *l_frames ), ( size_t )1 );

                    const size_t l_frameIndex = ( *l_frames )[ 0 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )1 );
                }
            }
        }

        // Third box
        // Overlapping
        {
            // Load frames from 2 up to 4
            bool l_returnValue =
                boxes_t$load$one$fromString( &l_boxes, "9 10 11 -1 2-4" );

            ASSERT_TRUE( l_returnValue );

            // Should have inserted exactly three keyFrames
            ASSERT_EQ( "%zu", arrayLength( l_boxes.keyFrames ), ( size_t )3 );

            // The stored SDL_FRect should match the input target rectangle
            {
                const SDL_FRect* l_targetRectangleStored =
                    l_boxes.keyFrames[ 2 ];

                ASSERT_EQ( "%f", l_targetRectangleStored->x, 9.0f );
                ASSERT_EQ( "%f", l_targetRectangleStored->y, 10.0f );
                ASSERT_EQ( "%f", l_targetRectangleStored->w, 11.0f );
                ASSERT_EQ( "%f", l_targetRectangleStored->h, -1.0f );
            }

            // The frames array should have indices 1, 2, 3, 4 and 5
            ASSERT_EQ( "%zu", arrayLength( l_boxes.frames ), ( size_t )5 );

            // For each index from 2 to 4 we should have recorded a frame index
            // of 0
            {
                size_t* const* l_frames = l_boxes.frames;

                l_frames++;

                // Index 2
                {
                    // Each l_boxes.frames[ _index ] is itself an array of
                    // size_t
                    ASSERT_EQ( "%zu", arrayLength( *l_frames ), ( size_t )2 );

                    size_t l_frameIndex = ( *l_frames )[ 0 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )0 );

                    l_frameIndex = ( *l_frames )[ 1 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )2 );
                }

                l_frames++;

                // Index 3
                {
                    // Each l_boxes.frames[ _index ] is itself an array of
                    // size_t
                    ASSERT_EQ( "%zu", arrayLength( *l_frames ), ( size_t )3 );

                    size_t l_frameIndex = ( *l_frames )[ 0 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )0 );

                    l_frameIndex = ( *l_frames )[ 1 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )1 );

                    l_frameIndex = ( *l_frames )[ 2 ];

                    ASSERT_EQ( "%zu", l_frameIndex, ( size_t )2 );
                }
            }
        }
    }

    bool l_returnValue = boxes_t$unload( &l_boxes );

    ASSERT_TRUE( l_returnValue );

    l_returnValue = boxes_t$destroy( &l_boxes );

    ASSERT_TRUE( l_returnValue );
}

TEST( boxes_t$unload ) {
    boxes_t l_boxes = boxes_t$create();

    bool l_returnValue = boxes_t$load$one$fromString( &l_boxes, "1 2 3 4 1-3" );

    ASSERT_TRUE( l_returnValue );

    l_returnValue = boxes_t$unload( &l_boxes );

    ASSERT_TRUE( l_returnValue );

    l_returnValue = boxes_t$destroy( &l_boxes );

    ASSERT_TRUE( l_returnValue );
}

TEST( boxes_t$step ) {
    boxes_t l_boxes = boxes_t$create();
    const SDL_FRect l_targetRectangle = {
        .x = 1.0f, .y = 2.0f, .w = 3.0f, .h = 4.0f };

    // Load frames from 1 up to ( but not including ) 3
    bool l_returnValue = boxes_t$load$one( &l_boxes, &l_targetRectangle, 1, 3 );

    ASSERT_TRUE( l_returnValue );

    ASSERT_EQ( "%zu", l_boxes.currentFrame, ( size_t )0 );

    {
        l_returnValue = boxes_t$step( &l_boxes, false );

        ASSERT_TRUE( l_returnValue );

        ASSERT_EQ( "%zu", l_boxes.currentFrame, ( size_t )1 );

        l_returnValue = boxes_t$step( &l_boxes, false );

        ASSERT_TRUE( l_returnValue );

        ASSERT_EQ( "%zu", l_boxes.currentFrame, ( size_t )2 );

        l_returnValue = boxes_t$step( &l_boxes, false );

        ASSERT_TRUE( l_returnValue );

        ASSERT_EQ( "%zu", l_boxes.currentFrame, ( size_t )2 );

        l_returnValue = boxes_t$step( &l_boxes, true );

        ASSERT_TRUE( l_returnValue );

        ASSERT_EQ( "%zu", l_boxes.currentFrame, ( size_t )0 );
    }

    l_returnValue = boxes_t$unload( &l_boxes );

    ASSERT_TRUE( l_returnValue );

    l_returnValue = boxes_t$destroy( &l_boxes );

    ASSERT_TRUE( l_returnValue );
}
