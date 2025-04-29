#pragma once

#include <stdlib.h>

#include "stdfunc.h"

#define UINT128_MAX ( ( uint128_t )( ( uint128_t ) ~( ( uint128_t )0 ) ) )
#define INT128_MAX ( ( int128_t )( ( int128_t )( ( uint128_t )1 << 127 ) - 1 ) )
#define INT128_MIN ( ( int128_t )( ( int128_t ) - ( ( uint128_t )1 << 127 ) ) )

#define INT128_TYPE_BIT_AMOUNT 128

typedef __int128 int128_t
    __attribute__( ( aligned( BITS_TO_BYTES( INT128_TYPE_BIT_AMOUNT ) ) ) );
typedef unsigned __int128 uint128_t
    __attribute__( ( aligned( BITS_TO_BYTES( INT128_TYPE_BIT_AMOUNT ) ) ) );

static FORCE_INLINE char* uint128_to_str( uint128_t value ) {
    // 128-bit max value in base 10 has 39 digits + 1 null terminator
    char* buffer = ( char* )malloc( 40 );
    if ( !buffer )
        return NULL;

    char temp[ 40 ];
    int pos = 0;

    if ( value == 0 ) {
        buffer[ 0 ] = '0';
        buffer[ 1 ] = '\0';
        return buffer;
    }

    while ( value > 0 ) {
        temp[ pos++ ] = '0' + ( value % 10 );
        value /= 10;
    }

    for ( int i = 0; i < pos; ++i ) {
        buffer[ i ] = temp[ pos - i - 1 ];
    }

    buffer[ pos ] = '\0';
    return buffer;
}

static FORCE_INLINE char* int128_to_str( int128_t value ) {
    if ( value == 0 ) {
        char* buffer = ( char* )malloc( 2 );
        if ( !buffer )
            return NULL;
        buffer[ 0 ] = '0';
        buffer[ 1 ] = '\0';
        return buffer;
    }

    int negative = value < 0;
    uint128_t uvalue = negative ? -value : value;

    char* result = uint128_to_str( uvalue );
    if ( !result )
        return NULL;

    if ( negative ) {
        size_t len = __builtin_strlen( result );
        char* with_sign = ( char* )malloc( len + 2 );
        if ( !with_sign ) {
            free( result );
            return NULL;
        }
        with_sign[ 0 ] = '-';
        __builtin_memcpy( with_sign + 1, result, len + 1 );
        free( result );
        return with_sign;
    }

    return result;
}
