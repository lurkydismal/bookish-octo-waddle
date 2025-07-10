#include "stdfunc.h"

#include <math.h>
#include <omp.h>

#include "stdfloat16.h"
#include "test.h"

TEST( stringToBool ) {
    ASSERT_TRUE( stringToBool( "true" ) );

    ASSERT_FALSE( stringToBool( "false" ) );

    ASSERT_FALSE( stringToBool( NULL ) );

    // Empty string
    ASSERT_FALSE( stringToBool( "" ) );

    // String with spaces
    ASSERT_FALSE( stringToBool( " true " ) );

    // Random invalid string
    ASSERT_FALSE( stringToBool( "hello" ) );
}

TEST( trim ) {
    {
        char l_buffer[] = "  hello world  ";
        char* l_string = l_buffer;

        // Removes leading and trailing spaces
        trim( &l_string, 2, 11 );

        ASSERT_STRING_EQ( l_string, "hello world" );
    }

    {
        char l_buffer[] = "abcdef";
        char* l_string = l_buffer;

        trim( &l_string, 1, 3 );

        ASSERT_STRING_EQ( l_string, "bcd" );
    }

    {
        char l_buffer[] = "abcdef";

        char* l_string = l_buffer;
        trim( &l_string, 0, 4 );

        ASSERT_STRING_EQ( l_string, "abcd" );
    }
}

TEST( lengthOfNumber ) {
#define MAX_NUMBER ( 10000000 )

    size_t l_actualLengthFailed = 0;
    size_t l_expectedLengthFailed = 0;

#pragma omp parallel for shared( l_actualLengthFailed, l_expectedLengthFailed )
    FOR_RANGE( size_t, 0, MAX_NUMBER ) {
        const size_t l_actualLength = lengthOfNumber( _index );
        const size_t l_expectedLength =
            ( ( _index == 0 ) ? ( 1 ) : ( log10( _index ) + 1 ) );

        if ( l_actualLength != l_expectedLength ) {
            l_actualLengthFailed = l_actualLength;
            l_expectedLengthFailed = l_expectedLength;
        }
    }

    ASSERT_EQ( "%zu", l_actualLengthFailed, l_expectedLengthFailed );

#undef MAX_NUMBER
}

TEST( randomNumber$seed$set ) {
#define CALLS_AMOUNT ( 10000 )

    // Ensure numbers are equal across calls
    {
        const size_t l_numberFirst = ( size_t )( -1 );

        randomNumber$seed$set( l_numberFirst );

        const size_t l_numberSecond = randomNumber$seed$get();

        ASSERT_EQ( "%zu", l_numberFirst, l_numberSecond );
    }

    // Ensure numbers are equal across multiple calls
    {
        FOR_RANGE( size_t, 0, CALLS_AMOUNT ) {
            randomNumber$seed$set( _index );

            ASSERT_EQ( "%zu", randomNumber$seed$get(), _index );
        }
    }

#undef CALLS_AMOUNT
}

TEST( randomNumber$seed$get ) {
#define CALLS_AMOUNT ( 10000 )

    // Ensure numbers are equal across calls
    {
        const size_t l_numberFirst = ( size_t )( -1 );

        randomNumber$seed$set( l_numberFirst );

        const size_t l_numberSecond = randomNumber$seed$get();

        ASSERT_EQ( "%zu", l_numberFirst, l_numberSecond );
    }

    // Ensure numbers are equal across multiple calls
    {
        FOR_RANGE( size_t, 0, CALLS_AMOUNT ) {
            randomNumber$seed$set( _index );

            ASSERT_EQ( "%zu", randomNumber$seed$get(), _index );
        }
    }

#undef CALLS_AMOUNT
}

TEST( randomNumber ) {
#define CALLS_AMOUNT ( 10000 )

    // Ensure random numbers are different across calls
    {
        const size_t l_numberFirst = randomNumber();
        const size_t l_numberSecond = randomNumber();

        ASSERT_NOT_EQ( "%zu", l_numberFirst, l_numberSecond );
    }

    // Ensure multiple calls return nonzero values
    {
        FOR_RANGE( size_t, 0, CALLS_AMOUNT ) {
            ASSERT_NOT_EQ( "%zu", randomNumber(), ( size_t )0 );
        }
    }

#undef CALLS_AMOUNT
}

TEST( generateHash ) {
#define MAX_BUFFER_LENGTH ( 10000 )

    // Invalid inputs
    {
        // Both not valid
        {
            ASSERT_EQ( "%zu", generateHash( NULL, 1 ), ( size_t )0 );

            uint8_t* l_buffer = NULL;

            ASSERT_EQ( "%zu", generateHash( l_buffer, 0 ), ( size_t )0 );
        }

        // Valid buffer
        {
            // Non NULL terminated string
            {
                uint8_t l_buffer[ 1 ] = { '0' };

                ASSERT_EQ( "%zu", generateHash( l_buffer, 0 ), ( size_t )0 );
            }

            // NULL terminated string
            {
                uint8_t l_buffer[] = "";

                ASSERT_EQ( "%zu", generateHash( l_buffer, 0 ), ( size_t )0 );
            }
        }
    }

    // Valid buffer
    {
        // Ensure multiple calls return nonzero values
        {
            bool l_hasFailed = false;

#pragma omp parallel for shared( l_hasFailed )
            FOR_RANGE( size_t, 1, MAX_BUFFER_LENGTH ) {
                const size_t l_bufferLength = _index;

                uint8_t* l_buffer =
                    ( uint8_t* )malloc( l_bufferLength * sizeof( uint8_t ) );

                FOR_RANGE( size_t, 0, _index ) {
                    l_buffer[ _index ] = ( randomNumber() % ( 255 + 1 ) );
                }

                const size_t l_actualHash =
                    generateHash( l_buffer, l_bufferLength );

                if ( !l_actualHash ) {
                    l_hasFailed = true;
                }

                free( l_buffer );
            }

            ASSERT_FALSE( l_hasFailed );
        }
    }

#undef MAX_BUFFER_LENGTH
}

TEST( duplicateString ) {
#define duplicateStringTest( _string )                                 \
    do {                                                               \
        char* l_result = duplicateString( _string );                   \
        const size_t l_resultLength = __builtin_strlen( l_result );    \
        char l_resultStatic[ l_resultLength + 1 ];                     \
        strlcpy( l_resultStatic, l_result, sizeof( l_resultStatic ) ); \
        free( l_result );                                              \
        ASSERT_STRING_EQ( l_resultStatic, _string );                   \
    } while ( 0 )

    // Simple string
    duplicateStringTest( "Hello" );

    // Empty string
    duplicateStringTest( "" );

    // Large string
    duplicateStringTest(
        "This is a very long test string to check memory allocation." );

    // NULL input
    {
        char* l_result = duplicateString( NULL );

        // Should return NULL
        ASSERT_EQ( "%p", l_result, NULL );
    }

#undef duplicateStringTest
}

TEST( findSymbolInString ) {
    // Symbol present
    {
        ASSERT_EQ( "%zd", findSymbolInString( "hello", 'e' ), ( ssize_t )1 );
        ASSERT_EQ( "%zd", findSymbolInString( "abcdef", 'd' ), ( ssize_t )3 );
    }

    // Symbol at the beginning
    ASSERT_EQ( "%zd", findSymbolInString( "world", 'w' ), ( ssize_t )0 );

    // Symbol at the end
    ASSERT_EQ( "%zd", findSymbolInString( "test", 't' ), ( ssize_t )0 );

    // Symbol not present
    ASSERT_EQ( "%zd", findSymbolInString( "abc", 'z' ), ( ssize_t )( -1 ) );

    // Empty string
    ASSERT_EQ( "%zd", findSymbolInString( "", 'x' ), ( ssize_t )( -1 ) );

    // NULL string
    ASSERT_EQ( "%zd", findSymbolInString( NULL, 'a' ), ( ssize_t )( -1 ) );
}

TEST( findLastSymbolInString ) {
    // Last occurrence in middle
    ASSERT_EQ( "%zd", findLastSymbolInString( "hello", 'l' ), ( ssize_t )3 );

    // Symbol at the end
    ASSERT_EQ( "%zd", findLastSymbolInString( "abcdef", 'f' ), ( ssize_t )5 );

    // Symbol at the beginning
    ASSERT_EQ( "%zd", findLastSymbolInString( "test", 't' ), ( ssize_t )3 );

    // Multiple occurrences, last one should be returned
    ASSERT_EQ( "%zd", findLastSymbolInString( "banana", 'a' ), ( ssize_t )5 );

    // Symbol not present
    ASSERT_EQ( "%zd", findLastSymbolInString( "xyz", 'a' ), ( ssize_t )( -1 ) );

    // Empty string
    ASSERT_EQ( "%zd", findLastSymbolInString( "", 'x' ), ( ssize_t )( -1 ) );

    // NULL string
    ASSERT_EQ( "%zd", findLastSymbolInString( NULL, 'a' ), ( ssize_t )( -1 ) );
}

TEST( concatBeforeAndAfterString ) {
#define concatBeforeAndAfterStringTest( _string, _beforeString, _afterString ) \
    do {                                                                       \
        char* l_string = duplicateString( _string );                           \
        ASSERT_EQ(                                                             \
            "%zu",                                                             \
            ( size_t )( concatBeforeAndAfterString( &l_string, _beforeString,  \
                                                    _afterString ) ==          \
                        ( ( sizeof( _beforeString _string _afterString ) -     \
                            1 ) *                                              \
                          sizeof( char ) ) ),                                  \
            ( size_t )1 );                                                     \
        ASSERT_STRING_EQ( l_string, ( _beforeString _string _afterString ) );  \
        free( l_string );                                                      \
    } while ( 0 )

    // Normal case
    concatBeforeAndAfterStringTest( "world", "Hello ", "!" );

    // Empty strings
    concatBeforeAndAfterStringTest( "", "Start", "End" );

    // Appending only
    concatBeforeAndAfterStringTest( "Hi", "", " there" );

    // Prepending only
    concatBeforeAndAfterStringTest( "Test", "Unit ", "" );

    // Null input handling
    {
        ASSERT_EQ( "%zu", concatBeforeAndAfterString( NULL, "A", "B" ),
                   ( size_t )0 );

        char* l_string = NULL;

        ASSERT_EQ( "%zu", concatBeforeAndAfterString( &l_string, "A", "B" ),
                   ( size_t )0 );
    }

#undef concatBeforeAndAfterStringTest
}

TEST( sanitizeString ) {
#define sanitizeStringTest( _string, _expectedString ) \
    do {                                               \
        l_result = sanitizeString( _string );          \
        ASSERT_STRING_EQ( l_result, _expectedString ); \
        free( l_result );                              \
    } while ( 0 )

    char* l_result;

    // Normal case with spaces and a comment
    sanitizeStringTest( "  Hello  World   # Comment here", "HelloWorld" );

    // Leading and trailing spaces
    sanitizeStringTest( "   test   ", "test" );

    // No spaces, no comments
    sanitizeStringTest( "NoSpacesOrComments", "NoSpacesOrComments" );

    // Only spaces
    sanitizeStringTest( "       ", "" );

    // Only comment
    sanitizeStringTest( "   # This is a comment", "" );

    // Empty string
    sanitizeStringTest( "", "" );

    // Comment in the middle
    sanitizeStringTest( "KeepThis # RemoveThis", "KeepThis" );

    // NULL input ( should return NULL )
    ASSERT_EQ( "%p", sanitizeString( NULL ), NULL );

#undef sanitizeStringTest
}

TEST( splitStringIntoArray ) {
    char** l_result;

    // Basic case
    {
        l_result = splitStringIntoArray( "apple,banana,cherry", "," );
        ASSERT_STRING_EQ( l_result[ 0 ], "apple" );
        ASSERT_STRING_EQ( l_result[ 1 ], "banana" );
        ASSERT_STRING_EQ( l_result[ 2 ], "cherry" );

        FREE_ARRAY_ELEMENTS( l_result );
        FREE_ARRAY( l_result );
    }

    // Consecutive delimiters (empty tokens)
    {
        l_result = splitStringIntoArray( "one,,two", "," );
        ASSERT_STRING_EQ( l_result[ 0 ], "one" );
        ASSERT_STRING_EQ( l_result[ 1 ], "two" );

        FREE_ARRAY_ELEMENTS( l_result );
        FREE_ARRAY( l_result );
    }

    // Leading and trailing delimiters
    {
        l_result = splitStringIntoArray( ",first,second,", "," );
        ASSERT_STRING_EQ( l_result[ 0 ], "first" );
        ASSERT_STRING_EQ( l_result[ 1 ], "second" );

        FREE_ARRAY_ELEMENTS( l_result );
        FREE_ARRAY( l_result );
    }

    // Single character input
    {
        l_result = splitStringIntoArray( "X", "," );
        ASSERT_STRING_EQ( l_result[ 0 ], "X" );

        FREE_ARRAY_ELEMENTS( l_result );
        FREE_ARRAY( l_result );
    }

    // Only delimiters
    {
        l_result = splitStringIntoArray( ",,,", "," );
        ASSERT_EQ( "%zu", arrayLength( l_result ), ( size_t )0 );

        FREE_ARRAY( l_result );
    }

    // Empty string
    {
        l_result = splitStringIntoArray( "", "," );
        ASSERT_EQ( "%zu", arrayLength( l_result ), ( size_t )0 );

        FREE_ARRAY( l_result );
    }

    // NULL input
    {
        l_result = splitStringIntoArray( NULL, "," );
        ASSERT_EQ( "%zu", arrayLength( l_result ), ( size_t )0 );

        FREE_ARRAY( l_result );
    }
}

TEST( splitStringIntoArrayBySymbol ) {
    char** l_result;

    // Basic case
    {
        l_result = splitStringIntoArrayBySymbol( "apple,banana,cherry", ',' );
        ASSERT_STRING_EQ( l_result[ 0 ], "apple" );
        ASSERT_STRING_EQ( l_result[ 1 ], "banana" );
        ASSERT_STRING_EQ( l_result[ 2 ], "cherry" );

        FREE_ARRAY_ELEMENTS( l_result );
        FREE_ARRAY( l_result );
    }

    // Consecutive delimiters (empty tokens)
    {
        l_result = splitStringIntoArrayBySymbol( "one,,two", ',' );
        ASSERT_STRING_EQ( l_result[ 0 ], "one" );
        ASSERT_STRING_EQ( l_result[ 1 ], "two" );

        FREE_ARRAY_ELEMENTS( l_result );
        FREE_ARRAY( l_result );
    }

    // Leading and trailing delimiters
    {
        l_result = splitStringIntoArrayBySymbol( ",first,second,", ',' );
        ASSERT_STRING_EQ( l_result[ 0 ], "first" );
        ASSERT_STRING_EQ( l_result[ 1 ], "second" );

        FREE_ARRAY_ELEMENTS( l_result );
        FREE_ARRAY( l_result );
    }

    // Single character input
    {
        l_result = splitStringIntoArrayBySymbol( "X", ',' );
        ASSERT_STRING_EQ( l_result[ 0 ], "X" );

        FREE_ARRAY_ELEMENTS( l_result );
        FREE_ARRAY( l_result );
    }

    // Only delimiters
    {
        l_result = splitStringIntoArrayBySymbol( ",,,", ',' );
        ASSERT_EQ( "%zu", arrayLength( l_result ), ( size_t )0 );

        FREE_ARRAY( l_result );
    }

    // Empty string
    {
        l_result = splitStringIntoArrayBySymbol( "", ',' );
        ASSERT_EQ( "%zu", arrayLength( l_result ), ( size_t )0 );

        FREE_ARRAY( l_result );
    }

    // NULL input
    {
        l_result = splitStringIntoArrayBySymbol( NULL, ',' );
        ASSERT_EQ( "%zu", arrayLength( l_result ), ( size_t )0 );

        FREE_ARRAY( l_result );
    }
}

TEST( createArray ) {
    // Normal
    {
        // Create an array of pointers
        void** l_array = createArray( void* );

        // Ensure it's not NULL
        ASSERT_NOT_EQ( "%p", l_array, NULL );

        // Ensure array length is initialized correctly
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )0 );

        // Free allocated memory
        FREE_ARRAY( l_array );
    }

    // Change array length manually
    {
        // Create an array of pointers
        uint8_t* l_array = createArray( uint8_t );

        // Ensure it's not NULL
        ASSERT_NOT_EQ( "%p", l_array, NULL );

        // Ensure array length is initialized correctly
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )0 );

        // Insert values
        {
            insertIntoArray( &l_array, 0 );
            insertIntoArray( &l_array, 1 );
        }

        // Ensure new length is updated
        // ( 0 existing + 2 new )
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )2 );

        // Change array length to maximum possible value
        *arrayLengthPointer( l_array ) = UINT8_MAX;

        // Ensure array length is changed correctly
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )( UINT8_MAX ) );

        // Ensure original values are intact
        {
            ASSERT_EQ( "%u", l_array[ 0 ], 0 );
            ASSERT_EQ( "%u", l_array[ 1 ], 1 );
        }

        // Free allocated memory
        // No allocated elements in array
        FREE_ARRAY( l_array );
    }

    // float
    {
        // Create an array of pointers
        float* l_array = createArray( float );

        // Ensure it's not NULL
        ASSERT_NOT_EQ( "%p", l_array, NULL );

        // Ensure array length is initialized correctly
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )0 );

        // Insert values
        {
            insertIntoArray( &l_array, 0 );
            insertIntoArray( &l_array, 1 );
        }

        // Ensure new length is updated
        // ( 0 existing + 2 new )
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )( 2 ) );

        // Change array length to maximum possible value
        *arrayLengthPointer( l_array ) = UINT8_MAX;

        // Ensure array length is changed correctly
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )( UINT8_MAX ) );

        // Ensure original values are intact
        {
            ASSERT_EQ( "%f", l_array[ 0 ], 0.0f );
            ASSERT_EQ( "%f", l_array[ 1 ], 1.0f );
        }

        // Free allocated memory
        // No allocated elements in array
        FREE_ARRAY( l_array );
    }

    // float16_t
    {
        // Create an array of pointers
        float16_t* l_array = createArray( float16_t );

        // Ensure it's not NULL
        ASSERT_NOT_EQ( "%p", l_array, NULL );

        // Ensure array length is initialized correctly
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )0 );

        // Insert values
        {
            insertIntoArray( &l_array, 0 );
            insertIntoArray( &l_array, 1 );
        }

        // Ensure new length is updated
        // ( 0 existing + 2 new )
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )( 2 ) );

        // Change array length to maximum possible value
        *arrayLengthPointer( l_array ) = UINT8_MAX;

        // Ensure array length is changed correctly
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )( UINT8_MAX ) );

        // Ensure original values are intact
        {
            ASSERT_EQ( "%f", ( float )( l_array[ 0 ] ),
                       ( float )( float16_t )( 0.0f ) );
            ASSERT_EQ( "%f", ( float )( l_array[ 1 ] ),
                       ( float )( float16_t )( 1.0f ) );
        }

        // Free allocated memory
        // No allocated elements in array
        FREE_ARRAY( l_array );
    }
}

TEST( preallocateArray ) {
    // void* array
    {
        // Create initial array
        void** l_array = createArray( void* );

        // Insert values
        {
            insertIntoArray( &l_array, 100 );
            insertIntoArray( &l_array, 200 );
        }

        // Preallocate more space
        preallocateArray( &l_array, 3 );

        // Ensure new length is updated
        // ( 2 existing + 3 new )
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )( 2 + 3 ) );

        // Ensure original values are intact
        {
            ASSERT_EQ( "%p", l_array[ 0 ], ( void* )100 );
            ASSERT_EQ( "%p", l_array[ 1 ], ( void* )200 );
        }

        // Free memory
        FREE_ARRAY( l_array );
    }

    // 8 bit value array
    {
        // Create an array of pointers
        uint8_t* l_array = createArray( uint8_t );

        // Ensure it's not NULL
        ASSERT_NOT_EQ( "%p", l_array, NULL );

        // Ensure array length is initialized correctly
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )0 );

        // Insert values
        {
            insertIntoArray( &l_array, 100 );
            insertIntoArray( &l_array, 200 );
        }

        // Preallocate more space
        preallocateArray( &l_array, 3 );

        // Ensure new length is updated
        // ( 2 existing + 3 new )
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )( 2 + 3 ) );

        // Ensure original values are intact
        {
            ASSERT_EQ( "%u", l_array[ 0 ], 100 );
            ASSERT_EQ( "%u", l_array[ 1 ], 200 );
        }

        // Free memory
        FREE_ARRAY( l_array );
    }

    // float array
    {
        // Create an array of pointers
        float* l_array = createArray( float );

        // Ensure it's not NULL
        ASSERT_NOT_EQ( "%p", l_array, NULL );

        // Ensure array length is initialized correctly
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )0 );

        // Insert values
        {
            insertIntoArray( &l_array, 100 );
            insertIntoArray( &l_array, 200 );
        }

        // Preallocate more space
        preallocateArray( &l_array, 3 );

        // Ensure new length is updated
        // ( 2 existing + 3 new )
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )( 2 + 3 ) );

        // Ensure original values are intact
        {
            ASSERT_EQ( "%f", l_array[ 0 ], 100.0f );
            ASSERT_EQ( "%f", l_array[ 1 ], 200.0f );
        }

        // Free memory
        FREE_ARRAY( l_array );
    }

    // 16 bit value array
    {
        // Create an array of pointers
        float16_t* l_array = createArray( float16_t );

        // Ensure it's not NULL
        ASSERT_NOT_EQ( "%p", l_array, NULL );

        // Ensure array length is initialized correctly
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )0 );

        // Insert values
        {
            insertIntoArray( &l_array, 100 );
            insertIntoArray( &l_array, 200 );
        }

        // Preallocate more space
        preallocateArray( &l_array, 3 );

        // Ensure new length is updated
        // ( 2 existing + 3 new )
        ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )( 2 + 3 ) );

        // Ensure original values are intact
        {
            ASSERT_EQ( "%f", ( float )( l_array[ 0 ] ),
                       ( float )( float16_t )( 100.0f ) );
            ASSERT_EQ( "%f", ( float )( l_array[ 1 ] ),
                       ( float )( float16_t )( 200.0f ) );
        }

        // Free memory
        FREE_ARRAY( l_array );
    }
}

TEST( insertIntoArray ) {
    // Create an initial array
    void** l_array = createArray( void* );

    // Ensure array length is updated correctly
    ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )0 );

    // Insert values
    {
        ASSERT_EQ( "%zu", insertIntoArray( &l_array, 200 ), ( size_t )0 );
        ASSERT_EQ( "%zu", insertIntoArray( &l_array, 300 ), ( size_t )1 );
    }

    // Ensure array length is updated correctly
    ASSERT_EQ( "%zu", arrayLength( l_array ), ( size_t )2 );

    // Ensure new elements are inserted at correct indices
    {
        ASSERT_EQ( "%p", l_array[ 0 ], ( void* )200 );
        ASSERT_EQ( "%p", l_array[ 1 ], ( void* )300 );
    }

    // Custom struct values
    {
        // Custom struct
        struct person {
            size_t id;
            char name[ 20 ];
            char* nameAllocated;
        };

        // Dynamically allocate and initialize two struct person instances
        struct person* l_personFirst =
            ( struct person* )malloc( 1 * sizeof( struct person ) );
        struct person* l_personSecond =
            ( struct person* )malloc( 1 * sizeof( struct person ) );

        l_personFirst->id = 1;
        __builtin_strcpy( l_personFirst->name, "Alice" );
        l_personFirst->nameAllocated = duplicateString( "Alice_dyn" );

        l_personSecond->id = 2;
        __builtin_strcpy( l_personSecond->name, "Bob" );
        l_personSecond->nameAllocated = duplicateString( "Bob_dyn" );

        // Insert struct pointers into the array
        ASSERT_EQ( "%zu", insertIntoArray( &l_array, l_personFirst ),
                   ( size_t )2 );
        ASSERT_EQ( "%zu", insertIntoArray( &l_array, l_personSecond ),
                   ( size_t )3 );

        // Retrieve inserted structs from the array and verify their contents
        struct person* test1 = ( struct person* )l_array[ 2 ];
        struct person* test2 = ( struct person* )l_array[ 3 ];

        // Verify that the struct data was correctly inserted and preserved
        ASSERT_EQ( "%zu", test1->id, ( size_t )1 );
        ASSERT_STRING_EQ( test1->name, "Alice" );
        ASSERT_STRING_EQ( test1->nameAllocated, "Alice_dyn" );

        ASSERT_EQ( "%zu", test2->id, ( size_t )2 );
        ASSERT_STRING_EQ( test2->name, "Bob" );
        ASSERT_STRING_EQ( test2->nameAllocated, "Bob_dyn" );

        // Free the allocated structs to avoid memory leaks
        free( l_personFirst->nameAllocated );
        free( l_personSecond->nameAllocated );
        free( l_personFirst );
        free( l_personSecond );
    }

    // Free memory
    FREE_ARRAY( l_array );
}

TEST( findStringInArray ) {
    const char* l_array[] = { "apple", "banana", "cherry", "date",
                              "elderberry" };

    // Cases
    {
        ASSERT_EQ(
            "%zd",
            findStringInArray( l_array, arrayLengthNative( l_array ), "apple" ),
            ( ssize_t )0 );
        ASSERT_EQ( "%zd",
                   findStringInArray( l_array, arrayLengthNative( l_array ),
                                      "banana" ),
                   ( ssize_t )1 );
        ASSERT_EQ( "%zd",
                   findStringInArray( l_array, arrayLengthNative( l_array ),
                                      "cherry" ),
                   ( ssize_t )2 );
        ASSERT_EQ(
            "%zd",
            findStringInArray( l_array, arrayLengthNative( l_array ), "date" ),
            ( ssize_t )3 );
        ASSERT_EQ( "%zd",
                   findStringInArray( l_array, arrayLengthNative( l_array ),
                                      "elderberry" ),
                   ( ssize_t )4 );
    }

    // String not found
    ASSERT_EQ(
        "%zd",
        findStringInArray( l_array, arrayLengthNative( l_array ), "fig" ),
        ( ssize_t )( -1 ) );

    // Empty array
    ASSERT_EQ( "%zd", findStringInArray( NULL, 0, "apple" ),
               ( ssize_t )( -1 ) );

    // NULL search string
    ASSERT_EQ( "%zd",
               findStringInArray( l_array, arrayLengthNative( l_array ), NULL ),
               ( ssize_t )( -1 ) );
}

TEST( findInArray ) {
    const size_t l_array[] = { 10, 20, 30, 40, 50 };

    // Cases
    {
        ASSERT_EQ( "%zd",
                   findInArray( l_array, arrayLengthNative( l_array ), 10 ),
                   ( ssize_t )0 );
        ASSERT_EQ( "%zd",
                   findInArray( l_array, arrayLengthNative( l_array ), 20 ),
                   ( ssize_t )1 );
        ASSERT_EQ( "%zd",
                   findInArray( l_array, arrayLengthNative( l_array ), 30 ),
                   ( ssize_t )2 );
        ASSERT_EQ( "%zd",
                   findInArray( l_array, arrayLengthNative( l_array ), 40 ),
                   ( ssize_t )3 );
        ASSERT_EQ( "%zd",
                   findInArray( l_array, arrayLengthNative( l_array ), 50 ),
                   ( ssize_t )4 );
    }

    // String not found
    ASSERT_EQ( "%zd", findInArray( l_array, arrayLengthNative( l_array ), 60 ),
               ( ssize_t )( -1 ) );

    // Empty array
    ASSERT_EQ( "%zd", findInArray( ( size_t* )NULL, 0, 0 ), ( ssize_t )( -1 ) );

    // NULL search string
    ASSERT_EQ(
        "%zd",
        findInArray( l_array, arrayLengthNative( l_array ), ( size_t )NULL ),
        ( ssize_t )( -1 ) );
}

TEST( containsString ) {
    const char* l_array[] = { "apple", "banana", "cherry", "date" };

    // Value exists in the array
    {
        ASSERT_TRUE(
            containsString( l_array, arrayLengthNative( l_array ), "banana" ) );
        ASSERT_TRUE(
            containsString( l_array, arrayLengthNative( l_array ), "date" ) );
    }

    // Value does not exist in the array
    ASSERT_FALSE(
        containsString( l_array, arrayLengthNative( l_array ), "grape" ) );

    // Empty array
    {
        const char* l_array[] = {};

        ASSERT_FALSE(
            containsString( l_array, arrayLengthNative( l_array ), "apple" ) );
    }

    // NULL input
    {
        ASSERT_FALSE(
            containsString( NULL, arrayLengthNative( l_array ), "banana" ) );
        ASSERT_FALSE(
            containsString( l_array, arrayLengthNative( l_array ), NULL ) );
        ASSERT_FALSE( containsString( NULL, 0, NULL ) );
    }
}

TEST( contains ) {
    const size_t l_array[] = { 1, 2, 3, 4, 5 };

    // Value exists in the array
    {
        ASSERT_TRUE( contains( l_array, arrayLengthNative( l_array ), 2 ) );
        ASSERT_TRUE( contains( l_array, arrayLengthNative( l_array ), 5 ) );
    }

    // Value does not exist in the array
    ASSERT_FALSE( contains( l_array, arrayLengthNative( l_array ), 10 ) );

    // Empty array
    {
        const size_t l_array[] = {};

        ASSERT_FALSE( contains( l_array, arrayLengthNative( l_array ), 1 ) );
    }

    // NULL input
    {
        ASSERT_FALSE( contains( NULL, arrayLengthNative( l_array ), 2 ) );
        ASSERT_FALSE( contains( l_array, arrayLengthNative( l_array ), 0 ) );
        ASSERT_FALSE( contains( NULL, 0, 0 ) );
    }
}

TEST( getApplicationDirectoryAbsolutePath ) {
    char* l_path = getApplicationDirectoryAbsolutePath();

    ASSERT_NOT_EQ( "%p", l_path, NULL );

    free( l_path );
}
