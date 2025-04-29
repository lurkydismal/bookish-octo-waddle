#pragma once

#include <stddef.h>
#include <stdlib.h>

#include "stdfunc.h"

// Largest normal positive
#define FLOAT16_MAX ( ( float16_t )( 65504.0f ) )

// Smallest positive normal
#define FLOAT16_MIN ( ( float16_t )( 6.103515625e-5f ) )

// Smallest difference between 1 and the next representable number ( in IEEE 754
// format )
#define FLOAT16_EPSILON ( ( float16_t )( 0.0009765625f ) )

// Smallest subnormal number
#define FLOAT16_SUBNORMAL_MIN ( ( float16x2_t )( 5.960464477539063e-8f ) )

// Largest negative normal value
#define FLOAT16_NEG_MAX ( ( float16x2_t )( -65504.0f ) )

// Smallest negative normal value
#define FLOAT16_NEG_MIN ( ( float16x2_t )( -6.103515625e-5f ) )

#define FLOAT16_TYPE_BIT_AMOUNT 16

typedef _Float16 float16_t
    __attribute__( ( aligned( BITS_TO_BYTES( FLOAT16_TYPE_BIT_AMOUNT ) ) ) );

typedef float16_t float16x2_t __attribute__( (
    vector_size( BITS_TO_BYTES( FLOAT16_TYPE_BIT_AMOUNT * 2 ) ) ) );
typedef float16_t float16x4_t __attribute__( (
    vector_size( BITS_TO_BYTES( FLOAT16_TYPE_BIT_AMOUNT * 4 ) ) ) );
typedef float16_t float16x8_t __attribute__( (
    vector_size( BITS_TO_BYTES( FLOAT16_TYPE_BIT_AMOUNT * 8 ) ) ) );
typedef float16_t float16x16_t __attribute__( (
    vector_size( BITS_TO_BYTES( FLOAT16_TYPE_BIT_AMOUNT * 16 ) ) ) );
typedef float16_t float16x32_t __attribute__( (
    vector_size( BITS_TO_BYTES( FLOAT16_TYPE_BIT_AMOUNT * 32 ) ) ) );

static FORCE_INLINE uint16_t float16_bits( float16_t f ) {
    union {
        float16_t f;
        uint16_t u;
    } u = { f };
    return u.u;
}

static FORCE_INLINE void utoa( uint32_t val, char* out ) {
    char tmp[ 12 ];
    int i = 0;
    if ( val == 0 ) {
        out[ 0 ] = '0';
        out[ 1 ] = '\0';
        return;
    }
    while ( val > 0 ) {
        tmp[ i++ ] = '0' + ( val % 10 );
        val /= 10;
    }
    for ( int j = 0; j < i; ++j )
        out[ j ] = tmp[ i - j - 1 ];
    out[ i ] = '\0';
}

static FORCE_INLINE char* float16_to_decimal_str( _Float16 f ) {
    uint16_t bits = float16_bits( f );
    int sign = ( bits >> 15 ) & 1;
    int exp = ( bits >> 10 ) & 0x1F;
    int frac = bits & 0x3FF;

    int e = exp - 15; // exponent bias for float16
    uint32_t mantissa;

    if ( exp == 0 ) {
        // Subnormal numbers
        e = -14;
        mantissa = frac;
    } else if ( exp == 0x1F ) {
        // NaN or Inf
        char* s = malloc( 8 );
        if ( !s )
            return NULL;
        if ( frac )
            return strcpy( s, "nan" );
        return strcpy( s, sign ? "-inf" : "inf" );
    } else {
        // Normalized numbers
        mantissa = ( 1 << 10 ) | frac;
    }

    // Scale mantissa based on exponent
    // We're using a fixed-point representation: integer.fractional
    uint64_t scaled = ( ( uint64_t )mantissa ) << 13; // shift up for precision
    if ( e > 0 )
        scaled <<= e;
    else
        scaled >>= -e;

    // Split integer and fractional parts
    uint32_t integer_part = scaled >> 23;
    uint32_t fractional_part = scaled & 0x7FFFFF;

    char* buffer = malloc( 48 );
    if ( !buffer )
        return NULL;

    char* p = buffer;
    if ( sign )
        *p++ = '-';

    // Integer part
    char tmp[ 12 ];
    utoa( integer_part, tmp );
    for ( char* t = tmp; *t; ++t )
        *p++ = *t;

    *p++ = '.';

    // Fractional part to 6 decimal digits (scaled fixed-point)
    for ( int i = 0; i < 6; ++i ) {
        fractional_part *= 10;
        *p++ = '0' + ( ( fractional_part >> 23 ) & 0xFF );
        fractional_part &= 0x7FFFFF;
    }

    *p = '\0';
    return buffer;
}
