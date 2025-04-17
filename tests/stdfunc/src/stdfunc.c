#include "stdfunc.h"

#include <math.h>
#include <omp.h>

#include "test.h"

TEST( trim ) {
    {
        char l_buffer[] = "  hello world  ";
        char* l_string = l_buffer;

        // Removes leading and trailing spaces
        trim( &l_string, 2, 11 );

        ASSERT_EQ( "%d", __builtin_strcmp( l_string, "hello world" ), 0 );
    }

    {
        char l_buffer[] = "abcdef";
        char* l_string = l_buffer;

        trim( &l_string, 1, 3 );

        ASSERT_EQ( "%d", __builtin_strcmp( l_string, "bcd" ), 0 );
    }

    {
        char l_buffer[] = "xyz";
        char* l_string = l_buffer;

        // No change
        trim( &l_string, -1, -1 );

        ASSERT_EQ( "%d", __builtin_strcmp( l_string, "xyz" ), 0 );
    }

    {
        char l_buffer[] = "abcdef";

        char* l_string = l_buffer;
        trim( &l_string, -1, 4 );

        ASSERT_EQ( "%d", __builtin_strcmp( l_string, "abcd" ), 0 );
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

        if ( UNLIKELY( l_actualLength != l_expectedLength ) ) {
            l_actualLengthFailed = l_actualLength;
            l_expectedLengthFailed = l_expectedLength;
        }
    }

    ASSERT_EQ( "%lu", l_actualLengthFailed, l_expectedLengthFailed );

#undef MAX_NUMBER
}

TEST( randomNumber$seed$set ) {
#define CALLS_AMOUNT ( 10000 )

    // Ensure numbers are equal across calls
    {
        const size_t l_numberFirst = ( size_t )( -1 );

        randomNumber$seed$set( l_numberFirst );

        const size_t l_numberSecond = randomNumber$seed$get();

        ASSERT_EQ( "%lu", l_numberFirst, l_numberSecond );
    }

    // Ensure numbers are equal across multiple calls
    {
        FOR_RANGE( size_t, 0, CALLS_AMOUNT ) {
            randomNumber$seed$set( _index );

            ASSERT_EQ( "%lu", randomNumber$seed$get(), _index );
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

        ASSERT_EQ( "%lu", l_numberFirst, l_numberSecond );
    }

    // Ensure numbers are equal across multiple calls
    {
        FOR_RANGE( size_t, 0, CALLS_AMOUNT ) {
            randomNumber$seed$set( _index );

            ASSERT_EQ( "%lu", randomNumber$seed$get(), _index );
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

        ASSERT_NOT_EQ( "%lu", l_numberFirst, l_numberSecond );
    }

    // Ensure multiple calls return nonzero values
    {
        FOR_RANGE( size_t, 0, CALLS_AMOUNT ) {
            ASSERT_NOT_EQ( "%lu", randomNumber(), ( size_t )0 );
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
            ASSERT_EQ( "%lu", generateHash( NULL, 1 ), ( size_t )0 );

            uint8_t* l_buffer = NULL;

            ASSERT_EQ( "%lu", generateHash( l_buffer, 0 ), ( size_t )0 );
        }

        // Valid buffer
        {
            // Non NULL terminated string
            {
                uint8_t l_buffer[ 1 ] = { '0' };

                ASSERT_EQ( "%lu", generateHash( l_buffer, 0 ), ( size_t )0 );
            }

            // NULL terminated string
            {
                uint8_t l_buffer[] = "";

                ASSERT_EQ( "%lu", generateHash( l_buffer, 0 ), ( size_t )0 );
            }
        }
    }

    // Valid buffer
    {
        // Ensure multiple calls return nonzero values
        {
            size_t l_actualHashFailed = 0;
            size_t l_expectedHashFailed = 0;

#pragma omp parallel for shared( l_actualHashFailed, l_expectedHashFailed )
            FOR_RANGE( size_t, 1, MAX_BUFFER_LENGTH ) {
                const size_t l_bufferLength = _index;

                uint8_t* l_buffer =
                    ( uint8_t* )malloc( l_bufferLength * sizeof( uint8_t ) );

                FOR_RANGE( size_t, 0, _index ) {
                    l_buffer[ _index ] = ( randomNumber() % ( 255 + 1 ) );
                }

                const size_t l_actualHash =
                    generateHash( l_buffer, l_bufferLength );
                const size_t l_expectedHash = ( size_t )0;

                if ( UNLIKELY( l_actualHash != l_expectedHash ) ) {
                    l_actualHashFailed = l_actualHash;
                    l_expectedHashFailed = l_expectedHash;
                }

                free( l_buffer );
            }

            ASSERT_NOT_EQ( "%lu", l_actualHashFailed, l_expectedHashFailed );
        }
    }

#undef MAX_BUFFER_LENGTH
}

TEST( duplicateString ) {
#define duplicateStringTest( _string )                               \
    do {                                                             \
        l_result = duplicateString( _string );                       \
        ASSERT_EQ( "%d", __builtin_strcmp( l_result, _string ), 0 ); \
        free( l_result );                                            \
    } while ( 0 )

    char* l_result;

    // Simple string
    duplicateStringTest( "Hello" );

    // Empty string
    duplicateStringTest( "" );

    // Large string
    duplicateStringTest(
        "This is a very long test string to check memory allocation." );

    // NULL input
    {
        l_result = duplicateString( NULL );

        // Should return NULL
        ASSERT_EQ( "%p", l_result, NULL );
    }

#undef duplicateStringTest
}

TEST( findSymbolInString ) {
    // Symbol present
    {
        ASSERT_EQ( "%ld", findSymbolInString( "hello", 'e' ), ( ssize_t )1 );
        ASSERT_EQ( "%ld", findSymbolInString( "abcdef", 'd' ), ( ssize_t )3 );
    }

    // Symbol at the beginning
    ASSERT_EQ( "%ld", findSymbolInString( "world", 'w' ), ( ssize_t )0 );

    // Symbol at the end
    ASSERT_EQ( "%ld", findSymbolInString( "test", 't' ), ( ssize_t )0 );

    // Symbol not present
    ASSERT_EQ( "%ld", findSymbolInString( "abc", 'z' ), ( ssize_t )( -1 ) );

    // Empty string
    ASSERT_EQ( "%ld", findSymbolInString( "", 'x' ), ( ssize_t )( -1 ) );

    // NULL string
    ASSERT_EQ( "%ld", findSymbolInString( NULL, 'a' ), ( ssize_t )( -1 ) );
}

TEST( findLastSymbolInString ) {
    // Last occurrence in middle
    ASSERT_EQ( "%ld", findLastSymbolInString( "hello", 'l' ), ( ssize_t )3 );

    // Symbol at the end
    ASSERT_EQ( "%ld", findLastSymbolInString( "abcdef", 'f' ), ( ssize_t )5 );

    // Symbol at the beginning
    ASSERT_EQ( "%ld", findLastSymbolInString( "test", 't' ), ( ssize_t )3 );

    // Multiple occurrences, last one should be returned
    ASSERT_EQ( "%ld", findLastSymbolInString( "banana", 'a' ), ( ssize_t )5 );

    // Symbol not present
    ASSERT_EQ( "%ld", findLastSymbolInString( "xyz", 'a' ), ( ssize_t )( -1 ) );

    // Empty string
    ASSERT_EQ( "%ld", findLastSymbolInString( "", 'x' ), ( ssize_t )( -1 ) );

    // NULL string
    ASSERT_EQ( "%ld", findLastSymbolInString( NULL, 'a' ), ( ssize_t )( -1 ) );
}

TEST( concatBeforeAndAfterString ) {
#define concatBeforeAndAfterStringTest( _string, _beforeString, _afterString ) \
    do {                                                                       \
        l_string = ( char* )malloc( ( sizeof( _string ) * sizeof( char ) ) );  \
        __builtin_strcpy( l_string, _string );                                 \
        ASSERT_EQ(                                                             \
            "%lu",                                                             \
            ( size_t )( concatBeforeAndAfterString( &l_string, _beforeString,  \
                                                    _afterString ) ==          \
                        ( ( sizeof( _beforeString _string _afterString ) -     \
                            1 ) *                                              \
                          sizeof( char ) ) ),                                  \
            ( size_t )1 );                                                     \
        ASSERT_EQ( "%d",                                                       \
                   __builtin_strcmp( l_string,                                 \
                                     ( _beforeString _string _afterString ) ), \
                   0 );                                                        \
        free( l_string );                                                      \
    } while ( 0 )

    char* l_string;

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
        ASSERT_EQ( "%lu", concatBeforeAndAfterString( NULL, "A", "B" ),
                   ( size_t )0 );

        l_string = NULL;

        ASSERT_EQ( "%lu", concatBeforeAndAfterString( &l_string, "A", "B" ),
                   ( size_t )0 );
    }

#undef concatBeforeAndAfterStringTest
}

TEST( sanitizeString ) {
#define sanitizeStringTest( _string, _expectedString )                       \
    do {                                                                     \
        l_result = sanitizeString( _string );                                \
        ASSERT_EQ( "%d", __builtin_strcmp( l_result, _expectedString ), 0 ); \
        free( l_result );                                                    \
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
        ASSERT_EQ( "%d", __builtin_strcmp( l_result[ 1 ], "apple" ), 0 );
        ASSERT_EQ( "%d", __builtin_strcmp( l_result[ 2 ], "banana" ), 0 );
        ASSERT_EQ( "%d", __builtin_strcmp( l_result[ 3 ], "cherry" ), 0 );

        FREE_ARRAY( l_result );
    }

    // Consecutive delimiters (empty tokens)
    {
        l_result = splitStringIntoArray( "one,,two", "," );
        ASSERT_EQ( "%d", __builtin_strcmp( l_result[ 1 ], "one" ), 0 );
        ASSERT_EQ( "%d", __builtin_strcmp( l_result[ 2 ], "two" ), 0 );

        FREE_ARRAY( l_result );
    }

    // Leading and trailing delimiters
    {
        l_result = splitStringIntoArray( ",first,second,", "," );
        ASSERT_EQ( "%d", __builtin_strcmp( l_result[ 1 ], "first" ), 0 );
        ASSERT_EQ( "%d", __builtin_strcmp( l_result[ 2 ], "second" ), 0 );

        FREE_ARRAY( l_result );
    }

    // Single character input
    {
        l_result = splitStringIntoArray( "X", "," );
        ASSERT_EQ( "%d", __builtin_strcmp( l_result[ 1 ], "X" ), 0 );

        FREE_ARRAY( l_result );
    }

    // Only delimiters
    {
        l_result = splitStringIntoArray( ",,,", "," );
        ASSERT_EQ( "%lu", arrayLength( l_result ), ( size_t )0 );

        FREE_ARRAY( l_result );
    }

    // Empty string
    {
        l_result = splitStringIntoArray( "", "," );
        ASSERT_EQ( "%lu", arrayLength( l_result ), ( size_t )0 );

        FREE_ARRAY( l_result );
    }

    // NULL input
    {
        l_result = splitStringIntoArray( NULL, "," );
        ASSERT_EQ( "%lu", arrayLength( l_result ), ( size_t )0 );

        FREE_ARRAY( l_result );
    }
}

TEST( splitStringIntoArrayBySymbol ) {
    char** l_result;

    // Basic case
    {
        l_result = splitStringIntoArrayBySymbol( "apple,banana,cherry", ',' );
        ASSERT_EQ( "%d", __builtin_strcmp( l_result[ 1 ], "apple" ), 0 );
        ASSERT_EQ( "%d", __builtin_strcmp( l_result[ 2 ], "banana" ), 0 );
        ASSERT_EQ( "%d", __builtin_strcmp( l_result[ 3 ], "cherry" ), 0 );

        FREE_ARRAY( l_result );
    }

    // Consecutive delimiters (empty tokens)
    {
        l_result = splitStringIntoArrayBySymbol( "one,,two", ',' );
        ASSERT_EQ( "%d", __builtin_strcmp( l_result[ 1 ], "one" ), 0 );
        ASSERT_EQ( "%d", __builtin_strcmp( l_result[ 2 ], "two" ), 0 );

        FREE_ARRAY( l_result );
    }

    // Leading and trailing delimiters
    {
        l_result = splitStringIntoArrayBySymbol( ",first,second,", ',' );
        ASSERT_EQ( "%d", __builtin_strcmp( l_result[ 1 ], "first" ), 0 );
        ASSERT_EQ( "%d", __builtin_strcmp( l_result[ 2 ], "second" ), 0 );

        FREE_ARRAY( l_result );
    }

    // Single character input
    {
        l_result = splitStringIntoArrayBySymbol( "X", ',' );
        ASSERT_EQ( "%d", __builtin_strcmp( l_result[ 1 ], "X" ), 0 );

        FREE_ARRAY( l_result );
    }

    // Only delimiters
    {
        l_result = splitStringIntoArrayBySymbol( ",,,", ',' );
        ASSERT_EQ( "%lu", arrayLength( l_result ), ( size_t )0 );

        FREE_ARRAY( l_result );
    }

    // Empty string
    {
        l_result = splitStringIntoArrayBySymbol( "", ',' );
        ASSERT_EQ( "%lu", arrayLength( l_result ), ( size_t )0 );

        FREE_ARRAY( l_result );
    }

    // NULL input
    {
        l_result = splitStringIntoArrayBySymbol( NULL, ',' );
        ASSERT_EQ( "%lu", arrayLength( l_result ), ( size_t )0 );

        FREE_ARRAY( l_result );
    }
}

TEST( createArray ) {
    // Create an array of pointers
    void** l_array = createArray( sizeof( void* ) );

    // Ensure it's not NULL
    ASSERT_NOT_EQ( "%p", l_array, NULL );

    // Ensure array length is initialized correctly
    ASSERT_EQ( "%lu", arrayLength( l_array ), ( size_t )0 );

    // Free allocated memory
    FREE_ARRAY( l_array );
}

TEST( preallocateArray ) {
    // Create initial array
    void** l_array = ( void** )createArray( sizeof( void* ) );

    // Insert values
    {
        insertIntoArray( &l_array, ( void* )100 );
        insertIntoArray( &l_array, ( void* )200 );
    }

    // Preallocate more space
    preallocateArray( &l_array, 3 );

    // Ensure new length is updated
    // ( 2 existing + 3 new + 1 extra )
    ASSERT_EQ( "%lu", arrayLength( l_array ), ( size_t )( 2 + 3 ) );

    // Ensure original values are intact
    {
        ASSERT_EQ( "%p", l_array[ 1 ], ( void* )100 );
        ASSERT_EQ( "%p", l_array[ 2 ], ( void* )200 );
    }

    // Free memory
    free( l_array );
}

TEST( insertIntoArray ) {
    // Create an initial array
    void** l_array = ( void** )createArray( sizeof( void* ) );

    // Insert values
    {
        ASSERT_EQ( "%lu", insertIntoArray( &l_array, ( void* )200 ),
                   ( size_t )1 );
        ASSERT_EQ( "%lu", insertIntoArray( &l_array, ( void* )300 ),
                   ( size_t )2 );
    }

    // Ensure new elements are inserted at correct indices
    {
        ASSERT_EQ( "%p", l_array[ 1 ], ( void* )200 );
        ASSERT_EQ( "%p", l_array[ 2 ], ( void* )300 );
    }

    // Ensure array length is updated correctly
    ASSERT_EQ( "%lu", arrayLength( l_array ), ( size_t )2 );

    // Free memory
    free( l_array );
}

TEST( findStringInArray ) {
    const char* l_array[] = { "apple", "banana", "cherry", "date",
                              "elderberry" };

    // Cases
    {
        ASSERT_EQ(
            "%ld",
            findStringInArray( l_array, arrayLengthNative( l_array ), "apple" ),
            ( ssize_t )0 );
        ASSERT_EQ( "%ld",
                   findStringInArray( l_array, arrayLengthNative( l_array ),
                                      "banana" ),
                   ( ssize_t )1 );
        ASSERT_EQ( "%ld",
                   findStringInArray( l_array, arrayLengthNative( l_array ),
                                      "cherry" ),
                   ( ssize_t )2 );
        ASSERT_EQ(
            "%ld",
            findStringInArray( l_array, arrayLengthNative( l_array ), "date" ),
            ( ssize_t )3 );
        ASSERT_EQ( "%ld",
                   findStringInArray( l_array, arrayLengthNative( l_array ),
                                      "elderberry" ),
                   ( ssize_t )4 );
    }

    // String not found
    ASSERT_EQ(
        "%ld",
        findStringInArray( l_array, arrayLengthNative( l_array ), "fig" ),
        ( ssize_t )( -1 ) );

    // Empty array
    ASSERT_EQ( "%ld", findStringInArray( NULL, 0, "apple" ),
               ( ssize_t )( -1 ) );

    // NULL search string
    ASSERT_EQ( "%ld",
               findStringInArray( l_array, arrayLengthNative( l_array ), NULL ),
               ( ssize_t )( -1 ) );
}

TEST( findInArray ) {
    const size_t l_array[] = { 10, 20, 30, 40, 50 };

    // Cases
    {
        ASSERT_EQ( "%ld",
                   findInArray( l_array, arrayLengthNative( l_array ), 10 ),
                   ( ssize_t )0 );
        ASSERT_EQ( "%ld",
                   findInArray( l_array, arrayLengthNative( l_array ), 20 ),
                   ( ssize_t )1 );
        ASSERT_EQ( "%ld",
                   findInArray( l_array, arrayLengthNative( l_array ), 30 ),
                   ( ssize_t )2 );
        ASSERT_EQ( "%ld",
                   findInArray( l_array, arrayLengthNative( l_array ), 40 ),
                   ( ssize_t )3 );
        ASSERT_EQ( "%ld",
                   findInArray( l_array, arrayLengthNative( l_array ), 50 ),
                   ( ssize_t )4 );
    }

    // String not found
    ASSERT_EQ( "%ld", findInArray( l_array, arrayLengthNative( l_array ), 60 ),
               ( ssize_t )( -1 ) );

    // Empty array
    ASSERT_EQ( "%ld", findInArray( ( size_t* )NULL, 0, 0 ), ( ssize_t )( -1 ) );

    // NULL search string
    ASSERT_EQ(
        "%ld",
        findInArray( l_array, arrayLengthNative( l_array ), ( size_t )NULL ),
        ( ssize_t )( -1 ) );
}

TEST( containsString ) {
    const char* l_array[] = { "apple", "banana", "cherry", "date" };

    // Value exists in the array
    {
        ASSERT_EQ(
            "%u",
            containsString( l_array, arrayLengthNative( l_array ), "banana" ),
            true );
        ASSERT_EQ(
            "%u",
            containsString( l_array, arrayLengthNative( l_array ), "date" ),
            true );
    }

    // Value does not exist in the array
    ASSERT_EQ( "%u",
               containsString( l_array, arrayLengthNative( l_array ), "grape" ),
               false );

    // Empty array
    {
        const char* l_array[] = {};

        ASSERT_EQ(
            "%u",
            containsString( l_array, arrayLengthNative( l_array ), "apple" ),
            false );
    }

    // NULL input
    {
        ASSERT_EQ(
            "%u",
            containsString( NULL, arrayLengthNative( l_array ), "banana" ),
            false );
        ASSERT_EQ(
            "%u", containsString( l_array, arrayLengthNative( l_array ), NULL ),
            false );
        ASSERT_EQ( "%u", containsString( NULL, 0, NULL ), false );
    }
}

TEST( contains ) {
    const size_t l_array[] = { 1, 2, 3, 4, 5 };

    // Value exists in the array
    {
        ASSERT_EQ( "%u", contains( l_array, arrayLengthNative( l_array ), 2 ),
                   true );
        ASSERT_EQ( "%u", contains( l_array, arrayLengthNative( l_array ), 5 ),
                   true );
    }

    // Value does not exist in the array
    ASSERT_EQ( "%u", contains( l_array, arrayLengthNative( l_array ), 10 ),
               false );

    // Empty array
    {
        const size_t l_array[] = {};

        ASSERT_EQ( "%u", contains( l_array, arrayLengthNative( l_array ), 1 ),
                   false );
    }

    // NULL input
    {
        ASSERT_EQ( "%u", contains( NULL, arrayLengthNative( l_array ), 2 ),
                   false );
        ASSERT_EQ( "%u", contains( l_array, arrayLengthNative( l_array ), 0 ),
                   false );
        ASSERT_EQ( "%u", contains( NULL, 0, 0 ), false );
    }
}
