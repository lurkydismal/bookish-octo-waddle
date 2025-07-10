#include "test.h"

#include "stdfunc.h"

#if defined( TESTS )

testEntry_t g_testRegistry[ MAX_TESTS ];
size_t g_testCount = 0;

int main( int _argumentCount, char** _argumentsVector ) {
    size_t l_testsPassed = 0;
    size_t l_testsFailed = 0;

    printf( YELLOW "--- Running %zu tests ---" RESET "\n", g_testCount );

    for ( size_t _index = 0; _index < g_testCount; _index++ ) {
        bool l_needExecute = true;
        const char* l_testName = g_testRegistry[ _index ].name;

        if ( _argumentCount > 1 ) {
            l_needExecute = false;

            FOR_RANGE( int, 1, _argumentCount ) {
                const char* l_testNameFromArguments =
                    _argumentsVector[ _index ];

                if ( __builtin_strcmp( l_testNameFromArguments, l_testName ) ==
                     0 ) {
                    l_needExecute = true;
                }
            }
        }

        if ( !l_needExecute ) {
            continue;
        }

        printf( CYAN_LIGHT "Running %s test..." RESET "\n", l_testName );

        if ( g_testRegistry[ _index ].function() == 0 ) {
            printf( GREEN "[PASSED]" RESET " %s\n", l_testName );

            l_testsPassed++;

        } else {
            l_testsFailed++;
        }
    }

    printf( YELLOW "--- Test Summary ---" RESET "\n" );

    if ( l_testsPassed ) {
        printf( GREEN "Passed: %zu" RESET "\n", l_testsPassed );
    }

    if ( l_testsFailed ) {
        printf( RED "Failed: %zu" RESET "\n", l_testsFailed );
    }

    return ( l_testsFailed > 0 );
}

#endif
