#include "test.h"

#include "stdfunc.h"

testEntry_t g_testRegistry[ MAX_TESTS ];
size_t g_testCount = 0;

int main( void ) {
    printf( YELLOW "--- Running %lu tests ---\n" RESET, g_testCount );

    for ( size_t _index = 0; _index < g_testCount; _index++ ) {
        printf( CYAN_LIGHT "Running %s test...\n" RESET,
                g_testRegistry[ _index ].name );

        if ( g_testRegistry[ _index ].function() == 0 ) {
            printf( GREEN "[PASSED]" RESET " %s\n",
                    g_testRegistry[ _index ].name );
        }
    }

    return ( 0 );
}
