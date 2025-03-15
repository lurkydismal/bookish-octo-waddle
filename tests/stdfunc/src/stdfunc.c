#include "stdfunc.h"

#include <omp.h>
#include <string.h>

#include "test.h"

TEST( lengthOfNumber ) {
#define MAX_NUMBER 1000000000

    for ( size_t _number = 0; _number < MAX_NUMBER; _number++ ) {
        if ( _number < 10 ) {
            ASSERT_EQ( "%lu", lengthOfNumber( _number ), ( size_t )1 );

        } else if ( _number < 100 ) {
            ASSERT_EQ( "%lu", lengthOfNumber( _number ), ( size_t )2 );

        } else if ( _number < 1000 ) {
            ASSERT_EQ( "%lu", lengthOfNumber( _number ), ( size_t )3 );

        } else if ( _number < 10000 ) {
            ASSERT_EQ( "%lu", lengthOfNumber( _number ), ( size_t )4 );

        } else if ( _number < 100000 ) {
            ASSERT_EQ( "%lu", lengthOfNumber( _number ), ( size_t )5 );

        } else if ( _number < 1000000 ) {
            ASSERT_EQ( "%lu", lengthOfNumber( _number ), ( size_t )6 );

        } else if ( _number < 10000000 ) {
            ASSERT_EQ( "%lu", lengthOfNumber( _number ), ( size_t )7 );

        } else if ( _number < 100000000 ) {
            ASSERT_EQ( "%lu", lengthOfNumber( _number ), ( size_t )8 );

        } else if ( _number < 1000000000 ) {
            ASSERT_EQ( "%lu", lengthOfNumber( _number ), ( size_t )9 );
        }
    }

#undef MAX_NUMBER
}

TEST( power ) {
    ASSERT_EQ( "%lu", power( 2, 3 ), ( size_t )8 );
    ASSERT_EQ( "%lu", power( 5, 0 ), ( size_t )1 );
    ASSERT_EQ( "%lu", power( 7, 1 ), ( size_t )7 );
    ASSERT_EQ( "%lu", power( 0, 0 ), ( size_t )1 );
    ASSERT_EQ( "%lu", power( 0, 5 ), ( size_t )0 );
    ASSERT_EQ( "%lu", power( 1, 255 ), ( size_t )1 );
    ASSERT_EQ( "%lu", power( 2, 8 ), ( size_t )256 );
    ASSERT_EQ( "%lu", power( 2, 31 ), ( size_t )2147483648 );
    ASSERT_EQ( "%lu", power( 10, 10 ), ( size_t )10000000000 );
}

TEST( convertNumberToString ) {
#define convertNumberToStringTest( _number )                \
    do {                                                    \
        l_result = convertNumberToString( _number );        \
        ASSERT_EQ( "%d", strcmp( l_result, #_number ), 0 ); \
        mi_free( l_result );                                \
    } while ( 0 )

    char* l_result;

    // Simple numbers
    {
        convertNumberToStringTest( 0 );
        convertNumberToStringTest( 1 );
        convertNumberToStringTest( 42 );
        convertNumberToStringTest( 999 );
    }

    // Larger numbers
    {
        convertNumberToStringTest( 123456789 );
        convertNumberToStringTest( 1000000000 );
    }

    // Maximum 'size_t' value ( for robustness )
    {
        const size_t l_maxNumber = ( ( size_t )( -1 ) );

        l_result = convertNumberToString( l_maxNumber );

        // Ensure it does not crash
        ASSERT_EQ( "%d", ( strlen( l_result ) > 0 ), 1 );

        mi_free( l_result );
    }

#undef convertNumberToStringTest
}

TEST( randomNumber ) {
    // Ensure random numbers are different across calls
    {
        const size_t l_numberFirst = randomNumber();
        const size_t l_numberSecond = randomNumber();

        ASSERT_NOT_EQ( "%lu", l_numberFirst, l_numberSecond );
    }

    // Ensure multiple calls return nonzero values
    {
        for ( size_t _index = 0; _index < 1000; _index++ ) {
            ASSERT_NOT_EQ( "%lu", randomNumber(), ( size_t )0 );
        }
    }
}

TEST( duplicateString ) {
#define duplicateStringTest( _string )                     \
    do {                                                   \
        l_result = duplicateString( _string );             \
        ASSERT_EQ( "%d", strcmp( l_result, _string ), 0 ); \
        mi_free( l_result );                               \
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
        ASSERT_EQ( "%d", ( l_result == NULL ), 1 );
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
        l_string =                                                             \
            ( char* )mi_malloc( ( sizeof( _string ) * sizeof( char ) ) );      \
        strcpy( l_string, _string );                                           \
        ASSERT_EQ(                                                             \
            "%lu",                                                             \
            ( size_t )( concatBeforeAndAfterString( &l_string, _beforeString,  \
                                                    _afterString ) ==          \
                        ( ( sizeof( _beforeString _string _afterString ) -     \
                            1 ) *                                              \
                          sizeof( char ) ) ),                                  \
            ( size_t )1 );                                                     \
        ASSERT_EQ( "%d",                                                       \
                   strcmp( l_string, ( _beforeString _string _afterString ) ), \
                   0 );                                                        \
        mi_free( l_string );                                                   \
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
                   ( size_t )2 );
    }

#undef concatBeforeAndAfterStringTest
}

TEST( sanitizeString ) {
#define sanitizeStringTest( _string, _expectedString )             \
    do {                                                           \
        l_result = sanitizeString( _string );                      \
        ASSERT_EQ( "%d", strcmp( l_result, _expectedString ), 0 ); \
        mi_free( l_result );                                       \
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
        ASSERT_EQ( "%d", strcmp( l_result[ 1 ], "apple" ), 0 );
        ASSERT_EQ( "%d", strcmp( l_result[ 2 ], "banana" ), 0 );
        ASSERT_EQ( "%d", strcmp( l_result[ 3 ], "cherry" ), 0 );

        FREE_ARRAY( char**, l_result );
    }

    // Consecutive delimiters (empty tokens)
    {
        l_result = splitStringIntoArray( "one,,two", "," );
        ASSERT_EQ( "%d", strcmp( l_result[ 1 ], "one" ), 0 );
        ASSERT_EQ( "%d", strcmp( l_result[ 2 ], "two" ), 0 );

        FREE_ARRAY( char**, l_result );
    }

    // Leading and trailing delimiters
    {
        l_result = splitStringIntoArray( ",first,second,", "," );
        ASSERT_EQ( "%d", strcmp( l_result[ 1 ], "first" ), 0 );
        ASSERT_EQ( "%d", strcmp( l_result[ 2 ], "second" ), 0 );

        FREE_ARRAY( char**, l_result );
    }

    // Single character input
    {
        l_result = splitStringIntoArray( "X", "," );
        ASSERT_EQ( "%d", strcmp( l_result[ 1 ], "X" ), 0 );

        FREE_ARRAY( char**, l_result );
    }

    // Only delimiters
    {
        l_result = splitStringIntoArray( ",,,", "," );
        ASSERT_EQ( "%lu", arrayLength( l_result ), ( size_t )0 );

        FREE_ARRAY( char**, l_result );
    }

    // Empty string
    {
        l_result = splitStringIntoArray( "", "," );
        ASSERT_EQ( "%lu", arrayLength( l_result ), ( size_t )0 );

        FREE_ARRAY( char**, l_result );
    }

    // NULL input
    {
        l_result = splitStringIntoArray( NULL, "," );
        ASSERT_EQ( "%lu", arrayLength( l_result ), ( size_t )0 );

        FREE_ARRAY( char**, l_result );
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
    FREE_ARRAY( void**, l_array );
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
    FREE_ARRAY( void**, l_array );
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
    FREE_ARRAY( void**, l_array );
}

TEST( insertIntoArrayByIndex ) {
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

    // Insert values by indexes
    {
        insertIntoArrayByIndex( &l_array, 3, ( void* )300 );
        insertIntoArrayByIndex( &l_array, 4, ( void* )400 );
    }

    // Ensure original values are intact
    {
        ASSERT_EQ( "%p", l_array[ 1 ], ( void* )100 );
        ASSERT_EQ( "%p", l_array[ 2 ], ( void* )200 );
        ASSERT_EQ( "%p", l_array[ 3 ], ( void* )300 );
        ASSERT_EQ( "%p", l_array[ 4 ], ( void* )400 );
    }

    // Free memory
    FREE_ARRAY( void**, l_array );
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
