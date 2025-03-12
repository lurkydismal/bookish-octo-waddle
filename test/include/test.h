#pragma once

#include <stdio.h>

#define RED "\e[1;31m"
#define GREEN "\e[1;32m"
#define YELLOW "\e[1;33m"
#define RESET "\e[0m"

#define MAX_TESTS 100

typedef int ( *testFunc_t )( void );

typedef struct {
    const char* name;
    testFunc_t func;
} testEntry_t;

extern testEntry_t g_testRegistry[ MAX_TESTS ];
extern size_t g_testCount;

#define TEST( _name )                                                       \
    static int _name( void ) __attribute__( ( used ) );                     \
    __attribute__( ( constructor ) ) static void register_##_name( void ) { \
        if ( g_testCount < MAX_TESTS ) {                                    \
            g_testRegistry[ g_testCount++ ] =                               \
                ( testEntry_t ){ #_name, _name };                           \
        }                                                                   \
    }                                                                       \
    static int _name( void )

#define ASSERT_EQ( _actual, _expected )                                      \
    do {                                                                     \
        if ( ( _actual ) != ( _expected ) ) {                                \
            printf( RED "[FAILED]" RESET " %s:%d: Expected %d but got %d\n", \
                    __FILE__, __LINE__, ( _expected ), ( _actual ) );        \
            return ( 1 );                                                    \
        }                                                                    \
    } while ( 0 )
