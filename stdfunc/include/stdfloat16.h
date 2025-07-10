#pragma once

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
