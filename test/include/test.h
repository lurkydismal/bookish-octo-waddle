#pragma once

#include <stdio.h>

#define MAX_TESTS 100

typedef int ( *test_func )( void );

typedef struct {
    const char* name;
    test_func func;
} test_entry;

extern test_entry test_registry[ MAX_TESTS ];
extern int test_count;

#define TEST( _name )                                                        \
    static int _name( void ) __attribute__( ( used ) );                      \
    __attribute__( ( constructor ) ) static void register_##_name( void ) {  \
        if ( test_count < MAX_TESTS ) {                                      \
            test_registry[ test_count++ ] = ( test_entry ){ #_name, _name }; \
        }                                                                    \
    }                                                                        \
    static int _name( void )

#define ASSERT_EQ( _actual, _expected )                                   \
    do {                                                                  \
        if ( ( _actual ) != ( _expected ) ) {                             \
            printf( "[FAILED] %s:%d: Expected %d but got %d\n", __FILE__, \
                    __LINE__, ( _expected ), ( _actual ) );               \
            return ( 1 );                                                 \
        }                                                                 \
    } while ( 0 )
