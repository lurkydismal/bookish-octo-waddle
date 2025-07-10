#pragma once

#include <stdio.h>

#define RED "\e[1;31m"
#define GREEN "\e[1;32m"
#define YELLOW "\e[1;33m"
#define CYAN_LIGHT "\e[1;36m"
#define RESET "\e[0m"

#define MAX_TESTS 1000

#if defined( TESTS )

typedef int ( *testFunction_t )( void );

typedef struct {
    const char* name;
    testFunction_t function;
} testEntry_t;

extern testEntry_t g_testRegistry[ MAX_TESTS ];
extern size_t g_testCount;

#endif

__attribute__( ( used ) ) static int g_status = 0;

#if defined( TESTS )

#define TEST( _name )                                                          \
    static void _name##_test_implementation( void ) __attribute__( ( used ) ); \
    static int _name##_test( void ) {                                          \
        g_status = 0;                                                          \
        _name##_test_implementation();                                         \
        return ( g_status );                                                   \
    }                                                                          \
    __attribute__( ( constructor,                                              \
                     used ) ) static void register_##_name##_test( void ) {    \
        if ( g_testCount < MAX_TESTS ) {                                       \
            g_testRegistry[ g_testCount++ ] =                                  \
                ( testEntry_t ){ #_name, _name##_test };                       \
        }                                                                      \
    }                                                                          \
    static void _name##_test_implementation( void )

#endif

#define ASSERT_TRUE( _actual )             \
    do {                                   \
        if ( !( _actual ) ) {              \
            printf( RED "[FAILED]" RESET   \
                        " %s:%d: Not true" \
                        "\n",              \
                    __FILE__, __LINE__ );  \
            g_status = 1;                  \
            return;                        \
        }                                  \
    } while ( 0 )

#define ASSERT_FALSE( _actual )             \
    do {                                    \
        if ( _actual ) {                    \
            printf( RED "[FAILED]" RESET    \
                        " %s:%d: Not false" \
                        "\n",               \
                    __FILE__, __LINE__ );   \
            g_status = 1;                   \
            return;                         \
        }                                   \
    } while ( 0 )

#define ASSERT_EQ( _format, _actual, _expected )                      \
    do {                                                              \
        if ( ( _actual ) != ( _expected ) ) {                         \
            printf( RED "[FAILED]" RESET                              \
                        " %s:%d: Expected "_format                    \
                        " but got "_format                            \
                        "\n",                                         \
                    __FILE__, __LINE__, ( _expected ), ( _actual ) ); \
            g_status = 1;                                             \
            return;                                                   \
        }                                                             \
    } while ( 0 )

#define ASSERT_NOT_EQ( _format, _actual, _expected )                  \
    do {                                                              \
        if ( ( _actual ) == ( _expected ) ) {                         \
            printf( RED "[FAILED]" RESET                              \
                        " %s:%d: Expected different from "_format     \
                        " but got "_format                            \
                        "\n",                                         \
                    __FILE__, __LINE__, ( _expected ), ( _actual ) ); \
            g_status = 1;                                             \
            return;                                                   \
        }                                                             \
    } while ( 0 )

#define ASSERT_STRING_EQ( _actual, _expected )                        \
    do {                                                              \
        if ( __builtin_strcmp( _actual, _expected ) != 0 ) {          \
            printf( RED "[FAILED]" RESET                              \
                        " %s:%d: Expected '%s'"                       \
                        " but got '%s'"                               \
                        "\n",                                         \
                    __FILE__, __LINE__, ( _expected ), ( _actual ) ); \
            g_status = 1;                                             \
            return;                                                   \
        }                                                             \
    } while ( 0 )

#define ASSERT_STRING_NOT_EQ( _actual, _expected )                    \
    do {                                                              \
        if ( __builtin_strcmp( _actual, _expected ) == 0 ) {          \
            printf( RED "[FAILED]" RESET                              \
                        " %s:%d: Expected different from '%s'"        \
                        " but got '%s'"                               \
                        "\n",                                         \
                    __FILE__, __LINE__, ( _expected ), ( _actual ) ); \
            g_status = 1;                                             \
            return;                                                   \
        }                                                             \
    } while ( 0 )
