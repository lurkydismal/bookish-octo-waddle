#pragma once

#include "stdfunc.h"

#define UINT128_MAX ( ( uint128_t )( ( uint128_t ) ~( ( uint128_t )0 ) ) )
#define INT128_MAX ( ( int128_t )( ( int128_t )( ( int128_t )1 << 127 ) - 1 ) )
#define INT128_MIN ( ( int128_t )( -( ( int128_t )1 << 127 ) ) )

#define INT128_TYPE_BIT_AMOUNT 128

typedef __int128 int128_t
    __attribute__( ( aligned( BITS_TO_BYTES( INT128_TYPE_BIT_AMOUNT ) ) ) );
typedef unsigned __int128 uint128_t
    __attribute__( ( aligned( BITS_TO_BYTES( INT128_TYPE_BIT_AMOUNT ) ) ) );
