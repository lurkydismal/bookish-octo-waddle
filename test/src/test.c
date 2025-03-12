#include "test.h"

#include "stdfunc.h"

test_entry test_registry[ MAX_TESTS ];
int test_count = 0;

static void test$run$all( void ) {
    printf( "\n--- Running %d tests ---\n", test_count );

    for ( int i = 0; i < test_count; i++ ) {
        printf( "Running %s...\n", test_registry[ i ].name );

        if ( test_registry[ i ].func() == 0 ) {
            printf( "[PASSED] %s\n", test_registry[ i ].name );

        } else {
            printf( "[FAILED] %s\n", test_registry[ i ].name );
        }
    }
}

int main( void ) {
    test$run$all();

    return ( 0 );
}
