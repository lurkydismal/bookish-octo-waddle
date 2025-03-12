#include "test.h"

#include "stdfunc.h"

testEntry_t g_testRegistry[ MAX_TESTS ];
size_t g_testCount = 0;

int main( void ) {
    printf( YELLOW "--- Running %lu tests ---\n" RESET, g_testCount );

    for ( size_t _index = 0; _index < g_testCount; _index++ ) {
        if ( g_testRegistry[ _index ].func() == 0 ) {
            printf( GREEN "[PASSED]" RESET " %s\n",
                    g_testRegistry[ _index ].name );
        }
    }

    return ( 0 );
}
