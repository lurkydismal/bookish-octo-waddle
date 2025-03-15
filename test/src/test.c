#include "test.h"

#include "stdfunc.h"

testEntry_t g_testRegistry[ MAX_TESTS ];
size_t g_testCount = 0;

int main( void ) {
    size_t l_testsPassed = 0;
    size_t l_testsFailed = 0;

    printf( YELLOW "--- Running %lu tests ---\n" RESET, g_testCount );

    for ( size_t _index = 0; _index < g_testCount; _index++ ) {
        printf( CYAN_LIGHT "Running %s test...\n" RESET,
                g_testRegistry[ _index ].name );

        if ( g_testRegistry[ _index ].function() == 0 ) {
            printf( GREEN "[PASSED]" RESET " %s\n",
                    g_testRegistry[ _index ].name );

            l_testsPassed++;

        } else {
            l_testsFailed++;
        }
    }

    printf( YELLOW "--- Test Summary ---\n" RESET );

    if ( l_testsPassed ) {
        printf( GREEN "Passed: %lu\n" RESET, l_testsPassed );
    }

    if ( l_testsFailed ) {
        printf( RED "Failed: %lu\n" RESET, l_testsFailed );
    }

    return ( l_testsFailed > 0 );
}
