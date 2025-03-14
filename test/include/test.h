#pragma once

#include <stdio.h>

#define RED "\e[1;31m"
#define GREEN "\e[1;32m"
#define YELLOW "\e[1;33m"
#define CYAN_LIGHT "\e[1;36m"
#define RESET "\e[0m"

#define MAX_TESTS 1000

typedef int ( *testFunction_t )( void );

typedef struct {
    const char* name;
    testFunction_t function;
} testEntry_t;

extern testEntry_t g_testRegistry[ MAX_TESTS ];
extern size_t g_testCount;

#define TEST( _name )                                                       \
    static int _name##_test( void ) __attribute__( ( used ) );              \
    __attribute__( ( constructor,                                           \
                     used ) ) static void register_##_name##_test( void ) { \
        if ( g_testCount < MAX_TESTS ) {                                    \
            g_testRegistry[ g_testCount++ ] =                               \
                ( testEntry_t ){ #_name, _name##_test };                    \
        }                                                                   \
    }                                                                       \
    static int _name##_test( void )

#define ASSERT_EQ( _format, _actual, _expected )                      \
    do {                                                              \
        if ( ( _actual ) != ( _expected ) ) {                         \
            printf( RED "[FAILED]" RESET                              \
                        " %s:%d: Expected "_format                    \
                        " but got "_format                            \
                        "\n",                                         \
                    __FILE__, __LINE__, ( _expected ), ( _actual ) ); \
            return ( 1 );                                             \
        }                                                             \
    } while ( 0 )

#define ASSERT_NOT_EQ( _format, _actual, _expected )                  \
    do {                                                              \
        if ( ( _actual ) == ( _expected ) ) {                         \
            printf( RED "[FAILED]" RESET                              \
                        " %s:%d: Expected "_format                    \
                        " but got "_format                            \
                        "\n",                                         \
                    __FILE__, __LINE__, ( _expected ), ( _actual ) ); \
            return ( 1 );                                             \
        }                                                             \
    } while ( 0 )
