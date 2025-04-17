#pragma once

typedef _Float16 float16_t __attribute__( ( aligned( 2 ) ) );

typedef float16_t float16x2_t __attribute__( ( vector_size( 4 ) ) );
typedef float16_t float16x4_t __attribute__( ( vector_size( 8 ) ) );
typedef float16_t float16x8_t __attribute__( ( vector_size( 16 ) ) );
typedef float16_t float16x16_t __attribute__( ( vector_size( 32 ) ) );
typedef float16_t float16x32_t __attribute__( ( vector_size( 64 ) ) );
