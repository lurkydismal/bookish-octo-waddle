#include "inputBuffer_t.h"

#include "test.h"

TEST( inputBuffer_t$create ) {
    inputBuffer_t l_inputBuffer = inputBuffer_t$create();

    bool l_returnValue = inputBuffer_t$destroy( &l_inputBuffer );

    ASSERT_TRUE( l_returnValue );
}

TEST( inputBuffer_t$destroy ) {
    // Invalid - NULL input
    {
        bool l_returnValue = inputBuffer_t$destroy( NULL );

        ASSERT_FALSE( l_returnValue );
    }

    // Valid
    {
        inputBuffer_t l_inputBuffer = inputBuffer_t$create();

        bool l_returnValue = inputBuffer_t$destroy( &l_inputBuffer );

        ASSERT_TRUE( l_returnValue );
    }
}

TEST( inputBuffer_t$insert ) {
    inputBuffer_t l_inputBuffer = inputBuffer_t$create();

    // Valid
    {
        {
            const input_t l_input = MAKE_INPUT( UP, A );
            const size_t l_frame = 1;

            bool l_returnValue =
                inputBuffer_t$insert( &l_inputBuffer, &l_input, l_frame );

            ASSERT_TRUE( l_returnValue );

            ASSERT_EQ(
                "%#b",
                inputBuffer_t$inputsSequence$getInput$last( &l_inputBuffer )
                    ->data,
                l_input.data );
            ASSERT_EQ(
                "%zu",
                inputBuffer_t$inputsSequence$getFrame$last( &l_inputBuffer ),
                l_frame );
            ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex, ( size_t )1 );
        }

        // TODO: Fix
        // Wrap
        {
            const input_t l_inputDummy1 = MAKE_INPUT( UP, A );

            FOR_RANGE(
                size_t, MAX_DELAY_BETWEEN_INPUTS,
                ( INPUT_BUFFER_LENGTH + MAX_DELAY_BETWEEN_INPUTS - 1 - 1 ) ) {
                const size_t l_frame = _index;

                bool l_returnValue = inputBuffer_t$insert(
                    &l_inputBuffer, &l_inputDummy1, l_frame );

                ASSERT_TRUE( l_returnValue );

                ASSERT_EQ(
                    "%#b",
                    inputBuffer_t$inputsSequence$getInput$last( &l_inputBuffer )
                        ->data,
                    l_inputDummy1.data );
                ASSERT_EQ( "%zu",
                           inputBuffer_t$inputsSequence$getFrame$last(
                               &l_inputBuffer ),
                           l_frame );
                ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                           ( _index - MAX_DELAY_BETWEEN_INPUTS + 1 + 1 ) );
            }
        }

        // Size max frame
        // and Over limit
        {
            const input_t l_input =
                MAKE_INPUT( ( LEFT | RIGHT ), ( A | B | D ) );
            const size_t l_frame = SIZE_MAX;

            bool l_returnValue =
                inputBuffer_t$insert( &l_inputBuffer, &l_input, l_frame );

            ASSERT_TRUE( l_returnValue );

            ASSERT_EQ(
                "%#b",
                inputBuffer_t$inputsSequence$getInput$last( &l_inputBuffer )
                    ->data,
                l_input.data );
            ASSERT_EQ(
                "%zu",
                inputBuffer_t$inputsSequence$getFrame$last( &l_inputBuffer ),
                l_frame );
            ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex, ( size_t )0 );
        }
    }

    // Invalid
    {
        // NULL input buffer
        {
            input_t l_input = MAKE_INPUT( UP, A );
            bool l_returnValue = inputBuffer_t$insert( NULL, &l_input, 0 );

            ASSERT_FALSE( l_returnValue );
        }

        // NULL input
        {
            bool l_returnValue =
                inputBuffer_t$insert( &l_inputBuffer, NULL, 0 );

            ASSERT_FALSE( l_returnValue );
        }
    }

    bool l_returnValue = inputBuffer_t$destroy( &l_inputBuffer );

    ASSERT_TRUE( l_returnValue );
}

TEST( inputBuffer_t$inputsSequence$get ) {
    // No wrap
    {
        inputBuffer_t l_inputBuffer = inputBuffer_t$create();

        // Valid
        {
            // Empty buffer
            {
                input_t** l_inputs =
                    inputBuffer_t$inputsSequence$get( &l_inputBuffer, 1 );

                ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )0 );

                FREE_ARRAY( l_inputs );
            }

            // Zero current frame
            {
                input_t** l_inputs =
                    inputBuffer_t$inputsSequence$get( &l_inputBuffer, 0 );

                ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )0 );

                FREE_ARRAY( l_inputs );
            }

            const input_t l_inputDummy1 = MAKE_INPUT( UP, A );
            const input_t l_inputDummy2 =
                MAKE_INPUT( ( LEFT | RIGHT ), ( A | B | D ) );

            // Insert
            {
                {
                    const size_t l_frame = 1;

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, &l_inputDummy1, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    ASSERT_EQ( "%#b",
                               inputBuffer_t$inputsSequence$getInput$last(
                                   &l_inputBuffer )
                                   ->data,
                               l_inputDummy1.data );
                    ASSERT_EQ( "%zu",
                               inputBuffer_t$inputsSequence$getFrame$last(
                                   &l_inputBuffer ),
                               l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( size_t )1 );
                }

                {
                    const size_t l_frame = 20;

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, &l_inputDummy2, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    ASSERT_EQ( "%#b",
                               inputBuffer_t$inputsSequence$getInput$last(
                                   &l_inputBuffer )
                                   ->data,
                               l_inputDummy2.data );
                    ASSERT_EQ( "%zu",
                               inputBuffer_t$inputsSequence$getFrame$last(
                                   &l_inputBuffer ),
                               l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( size_t )2 );
                }
            }

            {
                input_t** l_inputs =
                    inputBuffer_t$inputsSequence$get( &l_inputBuffer, 21 );

                ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )1 );

                {
                    const input_t* l_input = arrayFirstElement( l_inputs );

                    ASSERT_STRING_EQ(
                        input_t$convert$toStaticString( l_input ),
                        input_t$convert$toStaticString( &l_inputDummy2 ) );

                    ASSERT_EQ( "%#b", GET_DIRECTION( l_input ),
                               GET_DIRECTION( &l_inputDummy2 ) );
                    ASSERT_EQ( "%#b", GET_BUTTON( l_input ),
                               GET_BUTTON( &l_inputDummy2 ) );
                }

                FREE_ARRAY( l_inputs );
            }

            // Insert one more and close by frame
            {
                const size_t l_frame = 25;

                bool l_returnValue = inputBuffer_t$insert(
                    &l_inputBuffer, &l_inputDummy1, l_frame );

                ASSERT_TRUE( l_returnValue );

                ASSERT_EQ(
                    "%#b",
                    inputBuffer_t$inputsSequence$getInput$last( &l_inputBuffer )
                        ->data,
                    l_inputDummy1.data );
                ASSERT_EQ( "%zu",
                           inputBuffer_t$inputsSequence$getFrame$last(
                               &l_inputBuffer ),
                           l_frame );
                ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                           ( size_t )3 );
            }

            {
                input_t** l_inputs =
                    inputBuffer_t$inputsSequence$get( &l_inputBuffer, 26 );

                ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )2 );

                // First
                {
                    const input_t* l_input = arrayFirstElement( l_inputs );

                    ASSERT_STRING_EQ(
                        input_t$convert$toStaticString( l_input ),
                        input_t$convert$toStaticString( &l_inputDummy1 ) );

                    ASSERT_EQ( "%#b", GET_DIRECTION( l_input ),
                               GET_DIRECTION( &l_inputDummy1 ) );
                    ASSERT_EQ( "%#b", GET_BUTTON( l_input ),
                               GET_BUTTON( &l_inputDummy1 ) );
                }

                // Second
                {
                    const input_t* l_input = arrayLastElement( l_inputs );

                    ASSERT_STRING_EQ(
                        input_t$convert$toStaticString( l_input ),
                        input_t$convert$toStaticString( &l_inputDummy1 ) );

                    ASSERT_EQ( "%#b", GET_DIRECTION( l_input ),
                               GET_DIRECTION( &l_inputDummy2 ) );
                    ASSERT_EQ( "%#b", GET_BUTTON( l_input ),
                               GET_BUTTON( &l_inputDummy2 ) );
                }

                FREE_ARRAY( l_inputs );
            }
        }

        // Invalid
        {
            // NULL input buffer
            {
                input_t** l_inputs =
                    inputBuffer_t$inputsSequence$get( NULL, 1 );

                ASSERT_EQ( "%p", l_inputs, NULL );
            }
        }

        bool l_returnValue = inputBuffer_t$destroy( &l_inputBuffer );

        ASSERT_TRUE( l_returnValue );
    }

    // Wrap
    {
        inputBuffer_t l_inputBuffer = inputBuffer_t$create();

        const input_t l_inputDummy1 = MAKE_INPUT( UP, A );
        const input_t l_inputDummy2 = MAKE_INPUT( LEFT, ( C | D ) );

        // Insert
        {
            {
                const size_t l_frame = 1;

                bool l_returnValue = inputBuffer_t$insert(
                    &l_inputBuffer, &l_inputDummy1, l_frame );

                ASSERT_TRUE( l_returnValue );

                ASSERT_EQ(
                    "%#b",
                    inputBuffer_t$inputsSequence$getInput$last( &l_inputBuffer )
                        ->data,
                    l_inputDummy1.data );
                ASSERT_EQ( "%zu",
                           inputBuffer_t$inputsSequence$getFrame$last(
                               &l_inputBuffer ),
                           l_frame );
                ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                           ( size_t )1 );
            }

            // TODO: Fix
            // Up to limit
            {
                FOR_RANGE( size_t, MAX_DELAY_BETWEEN_INPUTS,
                           ( INPUT_BUFFER_LENGTH + MAX_DELAY_BETWEEN_INPUTS -
                             1 - 1 ) ) {
                    const size_t l_frame = _index;

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, &l_inputDummy1, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    ASSERT_EQ( "%#b",
                               inputBuffer_t$inputsSequence$getInput$last(
                                   &l_inputBuffer )
                                   ->data,
                               l_inputDummy1.data );
                    ASSERT_EQ( "%zu",
                               inputBuffer_t$inputsSequence$getFrame$last(
                                   &l_inputBuffer ),
                               l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( _index - MAX_DELAY_BETWEEN_INPUTS + 1 + 1 ) );
                }
            }

            // Over limit
            {
                {
                    const size_t l_frame =
                        ( INPUT_BUFFER_LENGTH + MAX_DELAY_BETWEEN_INPUTS - 1 );

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, &l_inputDummy1, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    ASSERT_EQ( "%#b",
                               inputBuffer_t$inputsSequence$getInput$last(
                                   &l_inputBuffer )
                                   ->data,
                               l_inputDummy1.data );
                    ASSERT_EQ( "%zu",
                               inputBuffer_t$inputsSequence$getFrame$last(
                                   &l_inputBuffer ),
                               l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( size_t )0 );
                }

                {
                    input_t** l_inputs = inputBuffer_t$inputsSequence$get(
                        &l_inputBuffer,
                        ( MAX_DELAY_BETWEEN_INPUTS + INPUT_BUFFER_LENGTH +
                          MAX_DELAY_BETWEEN_INPUTS - 1 ) );

                    ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                    ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )100 );

                    FREE_ARRAY( l_inputs );
                }

                {
                    const size_t l_frame =
                        ( INPUT_BUFFER_LENGTH + MAX_DELAY_BETWEEN_INPUTS );

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, &l_inputDummy2, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    ASSERT_EQ( "%#b",
                               inputBuffer_t$inputsSequence$getInput$last(
                                   &l_inputBuffer )
                                   ->data,
                               l_inputDummy2.data );
                    ASSERT_EQ( "%zu",
                               inputBuffer_t$inputsSequence$getFrame$last(
                                   &l_inputBuffer ),
                               l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( size_t )1 );
                }

                {
                    input_t** l_inputs = inputBuffer_t$inputsSequence$get(
                        &l_inputBuffer,
                        ( MAX_DELAY_BETWEEN_INPUTS + INPUT_BUFFER_LENGTH +
                          MAX_DELAY_BETWEEN_INPUTS ) );

                    ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                    ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )100 );

                    FREE_ARRAY( l_inputs );
                }

                {
                    input_t** l_inputs =
                        inputBuffer_t$inputsSequence$get$withLimit(
                            &l_inputBuffer, SIZE_MAX, SIZE_MAX );

                    ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                    ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )0 );

                    FREE_ARRAY( l_inputs );
                }
            }
        }

        bool l_returnValue = inputBuffer_t$destroy( &l_inputBuffer );

        ASSERT_TRUE( l_returnValue );
    }
}

TEST( inputBuffer_t$inputsSequence$get$withLimit ) {
    // No wrap
    {
        inputBuffer_t l_inputBuffer = inputBuffer_t$create();

        // Valid
        {
            // Empty buffer
            {
                input_t** l_inputs = inputBuffer_t$inputsSequence$get$withLimit(
                    &l_inputBuffer, 1, SIZE_MAX );

                ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )0 );

                FREE_ARRAY( l_inputs );
            }

            // Zero current frame
            {
                input_t** l_inputs = inputBuffer_t$inputsSequence$get$withLimit(
                    &l_inputBuffer, 0, SIZE_MAX );

                ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )0 );

                FREE_ARRAY( l_inputs );
            }

            const input_t l_inputDummy1 = MAKE_INPUT( UP, A );
            const input_t l_inputDummy2 =
                MAKE_INPUT( ( LEFT | RIGHT ), ( A | B | D ) );

            // Insert
            {
                {
                    const size_t l_frame = 1;

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, &l_inputDummy1, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    ASSERT_EQ( "%#b",
                               inputBuffer_t$inputsSequence$getInput$last(
                                   &l_inputBuffer )
                                   ->data,
                               l_inputDummy1.data );
                    ASSERT_EQ( "%zu",
                               inputBuffer_t$inputsSequence$getFrame$last(
                                   &l_inputBuffer ),
                               l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( size_t )1 );
                }

                {
                    const size_t l_frame = 20;

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, &l_inputDummy2, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    ASSERT_EQ( "%#b",
                               inputBuffer_t$inputsSequence$getInput$last(
                                   &l_inputBuffer )
                                   ->data,
                               l_inputDummy2.data );
                    ASSERT_EQ( "%zu",
                               inputBuffer_t$inputsSequence$getFrame$last(
                                   &l_inputBuffer ),
                               l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( size_t )2 );
                }
            }

            {
                input_t** l_inputs = inputBuffer_t$inputsSequence$get$withLimit(
                    &l_inputBuffer, 21, SIZE_MAX );

                ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )1 );

                {
                    const input_t* l_input = arrayFirstElement( l_inputs );

                    ASSERT_STRING_EQ(
                        input_t$convert$toStaticString( l_input ),
                        input_t$convert$toStaticString( &l_inputDummy2 ) );

                    ASSERT_EQ( "%#b", GET_DIRECTION( l_input ),
                               GET_DIRECTION( &l_inputDummy2 ) );
                    ASSERT_EQ( "%#b", GET_BUTTON( l_input ),
                               GET_BUTTON( &l_inputDummy2 ) );
                }

                FREE_ARRAY( l_inputs );
            }

            // Insert one more and close by frame
            {
                const size_t l_frame = 25;

                bool l_returnValue = inputBuffer_t$insert(
                    &l_inputBuffer, &l_inputDummy1, l_frame );

                ASSERT_TRUE( l_returnValue );

                ASSERT_EQ(
                    "%#b",
                    inputBuffer_t$inputsSequence$getInput$last( &l_inputBuffer )
                        ->data,
                    l_inputDummy1.data );
                ASSERT_EQ( "%zu",
                           inputBuffer_t$inputsSequence$getFrame$last(
                               &l_inputBuffer ),
                           l_frame );
                ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                           ( size_t )3 );
            }

            {
                input_t** l_inputs = inputBuffer_t$inputsSequence$get$withLimit(
                    &l_inputBuffer, 26, SIZE_MAX );

                ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )2 );

                // First
                {
                    const input_t* l_input = arrayFirstElement( l_inputs );

                    ASSERT_STRING_EQ(
                        input_t$convert$toStaticString( l_input ),
                        input_t$convert$toStaticString( &l_inputDummy1 ) );

                    ASSERT_EQ( "%#b", GET_DIRECTION( l_input ),
                               GET_DIRECTION( &l_inputDummy1 ) );
                    ASSERT_EQ( "%#b", GET_BUTTON( l_input ),
                               GET_BUTTON( &l_inputDummy1 ) );
                }

                // Second
                {
                    const input_t* l_input = arrayLastElement( l_inputs );

                    ASSERT_STRING_EQ(
                        input_t$convert$toStaticString( l_input ),
                        input_t$convert$toStaticString( &l_inputDummy1 ) );

                    ASSERT_EQ( "%#b", GET_DIRECTION( l_input ),
                               GET_DIRECTION( &l_inputDummy2 ) );
                    ASSERT_EQ( "%#b", GET_BUTTON( l_input ),
                               GET_BUTTON( &l_inputDummy2 ) );
                }

                FREE_ARRAY( l_inputs );
            }
        }

        // Invalid
        {
            // NULL input buffer
            {
                input_t** l_inputs = inputBuffer_t$inputsSequence$get$withLimit(
                    NULL, 1, SIZE_MAX );

                ASSERT_EQ( "%p", l_inputs, NULL );
            }
        }

        bool l_returnValue = inputBuffer_t$destroy( &l_inputBuffer );

        ASSERT_TRUE( l_returnValue );
    }

    // Wrap
    {
        inputBuffer_t l_inputBuffer = inputBuffer_t$create();

        const input_t l_inputDummy1 = MAKE_INPUT( UP, A );
        const input_t l_inputDummy2 = MAKE_INPUT( LEFT, ( C | D ) );

        // Insert
        {
            {
                const size_t l_frame = 1;

                bool l_returnValue = inputBuffer_t$insert(
                    &l_inputBuffer, &l_inputDummy1, l_frame );

                ASSERT_TRUE( l_returnValue );

                ASSERT_EQ(
                    "%#b",
                    inputBuffer_t$inputsSequence$getInput$last( &l_inputBuffer )
                        ->data,
                    l_inputDummy1.data );
                ASSERT_EQ( "%zu",
                           inputBuffer_t$inputsSequence$getFrame$last(
                               &l_inputBuffer ),
                           l_frame );
                ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                           ( size_t )1 );
            }

            // TODO: Fix
            // Up to limit
            {
                FOR_RANGE( size_t, MAX_DELAY_BETWEEN_INPUTS,
                           ( INPUT_BUFFER_LENGTH + MAX_DELAY_BETWEEN_INPUTS -
                             1 - 1 ) ) {
                    const size_t l_frame = _index;

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, &l_inputDummy1, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    ASSERT_EQ( "%#b",
                               inputBuffer_t$inputsSequence$getInput$last(
                                   &l_inputBuffer )
                                   ->data,
                               l_inputDummy1.data );
                    ASSERT_EQ( "%zu",
                               inputBuffer_t$inputsSequence$getFrame$last(
                                   &l_inputBuffer ),
                               l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( _index - MAX_DELAY_BETWEEN_INPUTS + 1 + 1 ) );
                }
            }

            // Over limit
            {
                // Insert
                {
                    const size_t l_frame =
                        ( INPUT_BUFFER_LENGTH + MAX_DELAY_BETWEEN_INPUTS - 1 );

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, &l_inputDummy1, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    ASSERT_EQ( "%#b",
                               inputBuffer_t$inputsSequence$getInput$last(
                                   &l_inputBuffer )
                                   ->data,
                               l_inputDummy1.data );
                    ASSERT_EQ( "%zu",
                               inputBuffer_t$inputsSequence$getFrame$last(
                                   &l_inputBuffer ),
                               l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( size_t )0 );
                }

                // Get all
                {
                    input_t** l_inputs =
                        inputBuffer_t$inputsSequence$get$withLimit(
                            &l_inputBuffer,
                            ( MAX_DELAY_BETWEEN_INPUTS + INPUT_BUFFER_LENGTH +
                              MAX_DELAY_BETWEEN_INPUTS - 1 ),
                            SIZE_MAX );

                    ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                    ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )100 );

                    FREE_ARRAY( l_inputs );
                }

                // Insert
                {
                    const size_t l_frame =
                        ( INPUT_BUFFER_LENGTH + MAX_DELAY_BETWEEN_INPUTS );

                    bool l_returnValue = inputBuffer_t$insert(
                        &l_inputBuffer, &l_inputDummy2, l_frame );

                    ASSERT_TRUE( l_returnValue );

                    ASSERT_EQ( "%#b",
                               inputBuffer_t$inputsSequence$getInput$last(
                                   &l_inputBuffer )
                                   ->data,
                               l_inputDummy2.data );
                    ASSERT_EQ( "%zu",
                               inputBuffer_t$inputsSequence$getFrame$last(
                                   &l_inputBuffer ),
                               l_frame );
                    ASSERT_EQ( "%zu", l_inputBuffer.currentBufferIndex,
                               ( size_t )1 );
                }

                // Get all
                {
                    input_t** l_inputs =
                        inputBuffer_t$inputsSequence$get$withLimit(
                            &l_inputBuffer,
                            ( MAX_DELAY_BETWEEN_INPUTS + INPUT_BUFFER_LENGTH +
                              MAX_DELAY_BETWEEN_INPUTS ),
                            SIZE_MAX );

                    ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                    ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )100 );

                    FREE_ARRAY( l_inputs );
                }

                // With limit
                {
                    FOR_RANGE(
                        size_t, 0,
                        ( arrayLengthNative( l_inputBuffer.inputs ) + 1 ) ) {
                        input_t** l_inputs =
                            inputBuffer_t$inputsSequence$get$withLimit(
                                &l_inputBuffer,
                                ( MAX_DELAY_BETWEEN_INPUTS +
                                  INPUT_BUFFER_LENGTH +
                                  MAX_DELAY_BETWEEN_INPUTS ),
                                _index );

                        ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                        ASSERT_EQ( "%zu", arrayLength( l_inputs ), _index );

                        FREE_ARRAY( l_inputs );
                    }
                }

                {
                    input_t** l_inputs =
                        inputBuffer_t$inputsSequence$get$withLimit(
                            &l_inputBuffer, SIZE_MAX, SIZE_MAX );

                    ASSERT_NOT_EQ( "%p", l_inputs, NULL );

                    ASSERT_EQ( "%zu", arrayLength( l_inputs ), ( size_t )0 );

                    FREE_ARRAY( l_inputs );
                }
            }
        }

        bool l_returnValue = inputBuffer_t$destroy( &l_inputBuffer );

        ASSERT_TRUE( l_returnValue );
    }
}
