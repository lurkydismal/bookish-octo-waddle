#pragma once

#define UINT128_MAX ( ( uint128_t )( ( uint128_t ) ~( ( uint128_t )0 ) ) )
#define INT128_MAX ( ( int128_t )( ( int128_t )( ( uint128_t )1 << 127 ) - 1 ) )
#define INT128_MIN ( ( int128_t )( ( int128_t ) - ( ( uint128_t )1 << 127 ) ) )

typedef __int128 int128_t __attribute__( ( aligned( 16 ) ) );
typedef unsigned __int128 uint128_t __attribute__( ( aligned( 16 ) ) );
