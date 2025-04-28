#pragma once

#define XXH_CPU_LITTLE_ENDIAN 1
#define XXH_INLINE_ALL
#define XXH_NO_STDLIB
#define XXH_NO_STREAM
#define XXH_STATIC_LINKING_ONLY
#define XXH_memcmp __builtin_memcmp
#define XXH_memcpy __builtin_memcpy
#define XXH_memset __builtin_memset

#include <omp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <xxhash.h>

#include "stdfloat16.h"

// Function attributes
#define FORCE_INLINE __attribute__( ( always_inline ) ) inline
#define NO_OPTIMIZE __attribute__( ( optimize( "0" ) ) )
#define NO_RETURN __attribute__( ( noreturn ) )
#define HOT __attribute__( ( hot ) )
#define COLD __attribute__( ( cold ) )
#define SENTINEL __attribute__( ( sentinel ) )

// Branch prediction hints
#define LIKELY( _expression ) __builtin_expect( !!( _expression ), 1 )
#define UNLIKELY( _expression ) __builtin_expect( !!( _expression ), 0 )

// Constants
#define DECIMAL_RADIX 10
#define ONE_SECOND_IN_MILLISECONDS ( ( size_t )( 1000 ) )
#define ONE_MILLISECOND_IN_NANOSECONDS ( ( size_t )( 1000000 ) )

// Utility macros ( no side-effects )
#define MILLISECONDS_TO_NANOSECONDS( _milliseconds ) \
    ( ( _milliseconds ) * ONE_MILLISECOND_IN_NANOSECONDS )
// This macro turns a value into a string literal
#define STRINGIFY_MACRO( _value ) #_value
// This is a helper macro that handles the stringify
#define MACRO_TO_STRING( _macro ) STRINGIFY_MACRO( _macro )

// Utility functions ( no side-effects )
#define max( _a, _b ) ( ( ( _a ) > ( _b ) ) ? ( _a ) : ( _b ) )
#define min( _a, _b ) ( ( ( _a ) < ( _b ) ) ? ( _a ) : ( _b ) )

// Non-native and native array utility functions
#define arrayLengthPointer( _array ) \
    ( ( size_t* )( ( char* )( _array ) - sizeof( size_t ) ) )
#define arrayAllocationPointer( _array ) ( arrayLengthPointer( _array ) )
#define arrayFirstElementPointer( _array ) ( _array )
#define arrayLastElementPointer( _array ) \
    ( ( _array ) + arrayLength( _array ) - 1 )

#define arrayFirstElement( _array ) ( *arrayFirstElementPointer( _array ) )
#define arrayLastElement( _array ) ( *arrayLastElementPointer( _array ) )

#define arrayLength( _array ) ( *arrayLengthPointer( _array ) )
#define randomValueFromArray( _array ) \
    ( ( _array )[ randomNumber() % arrayLength( ( _array ) ) ] )

#define arrayLengthNative( _array ) \
    ( sizeof( ( _array ) ) / sizeof( ( _array )[ 0 ] ) )
#define randomValueFromArrayNative( _array ) \
    ( ( _array )[ randomNumber() % arrayLengthNative( ( _array ) ) ] )

// Native array iteration FOR
#define FOR( _type, _array )                                       \
    for ( _type _element = ( _array );                             \
          _element < ( ( _array ) + arrayLengthNative( _array ) ); \
          _element++ )

// Non-native array iteration FOR
#define FOR_ARRAY( _type, _array )                             \
    for ( _type _element = arrayFirstElementPointer( _array ); \
          _element != ( arrayLastElementPointer( _array ) + 1 ); _element++ )

// Range iteration FOR
#define FOR_RANGE( _type, _start, _end ) \
    for ( _type _index = ( _start ); _index < ( _end ); _index++ )

// Range iteration FOR with increase BY amount
#define FOR_RANGE_BY( _type, _start, _end, _amount ) \
    for ( _type _index = ( _start ); _index < ( _end ); _index += ( _amount ) )

// Non-native array free every element
#define FREE_ARRAY_ELEMENTS( _array )               \
    do {                                            \
        FOR_ARRAY( typeof( _array ), ( _array ) ) { \
            free( *_element );                      \
        }                                           \
    } while ( 0 )

#define FREE_ARRAY( _array )                      \
    do {                                          \
        free( arrayAllocationPointer( _array ) ); \
    } while ( 0 )

// Utility functions ( no side-effects )
static FORCE_INLINE bool stringToBool( const char* restrict _string ) {
    if ( UNLIKELY( !_string ) ) {
        return ( false );
    }

    if ( __builtin_strcmp( _string, "true" ) == 0 ) {
        return ( true );

    } else {
        return ( false );
    }
}

static FORCE_INLINE void trim( char** restrict _string,
                               const ssize_t _from,
                               const ssize_t _to ) {
    if ( UNLIKELY( _from >= 0 ) ) {
        ( *_string ) += _from;
    }

    if ( UNLIKELY( _to >= 0 ) ) {
        ( *_string )[ _to ] = '\0';
    }
}

static FORCE_INLINE size_t lengthOfNumber( size_t _number ) {
    // clang-format off
    return (
        ( (_number < 10ULL) ) ? ( 1 ) :
        ( (_number < 100ULL) ) ? ( 2 ) :
        ( (_number < 1000ULL) ) ? ( 3 ) :
        ( (_number < 10000ULL) ) ? ( 4 ) :
        ( (_number < 100000ULL) ) ? ( 5 ) :
        ( (_number < 1000000ULL) ) ? ( 6 ) :
        ( (_number < 10000000ULL) ) ? ( 7 ) :
        ( (_number < 100000000ULL) ) ? ( 8 ) :
        ( (_number < 1000000000ULL) ) ? ( 9 ) :
        ( (_number < 10000000000ULL) ) ? ( 10 ) :
        ( (_number < 100000000000ULL) ) ? ( 11 ) :
        ( (_number < 1000000000000ULL) ) ? ( 12 ) :
        ( (_number < 10000000000000ULL) ) ? ( 13 ) :
        ( (_number < 100000000000000ULL) ) ? ( 14 ) :
        ( (_number < 1000000000000000ULL) ) ? ( 15 ) :
        ( (_number < 10000000000000000ULL) ) ? ( 16 ) :
        ( (_number < 100000000000000000ULL) ) ? ( 17 ) :
        ( (_number < 1000000000000000000ULL) ) ? ( 18 ) :
        ( (_number < 10000000000000000000ULL) ) ? ( 19 ) :
        ( 20 )
    );
    // clang-format on
}

void randomNumber$seed$set( const size_t _seed );
size_t randomNumber$seed$get( void );
size_t randomNumber( void );

static FORCE_INLINE size_t generateHash( const uint8_t* restrict _data,
                                         const size_t _dataSize ) {
    size_t l_returnValue = 0;

    if ( !_data ) {
        goto EXIT;
    }

    if ( !_dataSize ) {
        goto EXIT;
    }

    {
        l_returnValue = XXH32( _data, _dataSize, randomNumber$seed$get() );
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE char* duplicateString( const char* restrict _string ) {
    char* l_returnValue = NULL;

    if ( UNLIKELY( !_string ) ) {
        goto EXIT;
    }

    const size_t l_stringLength = __builtin_strlen( _string );

    l_returnValue = ( char* )malloc( ( l_stringLength + 1 ) * sizeof( char ) );

    __builtin_memcpy( l_returnValue, _string, ( l_stringLength + 1 ) );

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE ssize_t findSymbolInString( const char* restrict _string,
                                                const char _symbol ) {
    ssize_t l_returnValue = -1;

    if ( UNLIKELY( !_string ) ) {
        goto EXIT;
    }

    {
        char* l_result = __builtin_strchr( _string, _symbol );

        if ( UNLIKELY( !l_result ) ) {
            goto EXIT;
        }

        l_returnValue = ( l_result - _string );
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE ssize_t
findLastSymbolInString( const char* restrict _string, const char _symbol ) {
    ssize_t l_returnValue = -1;

    if ( UNLIKELY( !_string ) ) {
        goto EXIT;
    }

    {
        char* l_result = __builtin_strrchr( _string, _symbol );

        if ( UNLIKELY( !l_result ) ) {
            goto EXIT;
        }

        l_returnValue = ( l_result - _string );
    }

EXIT:
    return ( l_returnValue );
}

size_t concatBeforeAndAfterString( char* restrict* restrict _string,
                                   const char* restrict _beforeString,
                                   const char* restrict _afterString );
char* sanitizeString( const char* restrict _string );
char** splitStringIntoArray( const char* restrict _string,
                             const char* restrict _delimiter );
char** splitStringIntoArrayBySymbol( const char* restrict _string,
                                     const char _symbol );

#define createArray( _elementType )                   \
    ( {                                               \
        size_t* l_array = malloc( sizeof( size_t ) ); \
        *l_array = ( size_t )0;                       \
        ( _elementType* )( l_array + 1 );             \
    } )

static FORCE_INLINE void** _createArray( const size_t _elementSize ) {
    void** l_array = ( void** )malloc( 1 * _elementSize );

    *arrayLengthPointer( l_array ) = ( size_t )( 1 );

    return ( l_array );
}

#define preallocateArray( _array, _length )                                    \
    do {                                                                       \
        size_t* l_base = arrayLengthPointer( *( _array ) );                    \
        size_t l_oldLen = *l_base;                                             \
        size_t l_newLen = l_oldLen + ( _length );                              \
        size_t* l_realloc =                                                    \
            realloc( l_base, sizeof( size_t ) +                                \
                                 ( l_newLen * sizeof( typeof( *_array ) ) ) ); \
        if ( l_realloc ) {                                                     \
            *l_realloc = l_newLen;                                             \
            *( _array ) = ( typeof( *_array ) )( l_realloc + 1 );              \
        }                                                                      \
    } while ( 0 )

static FORCE_INLINE void _preallocateArray( void*** restrict _array,
                                            const size_t _length ) {
    if ( UNLIKELY( !_array ) ) {
        return;
    }

    if ( UNLIKELY( !*_array ) ) {
        return;
    }

    const size_t l_currentArrayLength = arrayLength( *_array );

    *_array = ( void** )realloc(
        arrayAllocationPointer( *_array ),
        ( ( sizeof( size_t ) + l_currentArrayLength + _length ) *
          sizeof( void* ) ) );

    *arrayLengthPointer( *_array ) =
        ( size_t )( l_currentArrayLength + _length + 1 );
}

#define insertIntoArray( _array, _value )                                      \
    ( {                                                                        \
        size_t l_ret = -1;                                                     \
        size_t* l_base = arrayLengthPointer( *( _array ) );                    \
        size_t l_oldLen = *l_base;                                             \
        size_t l_newLen = l_oldLen + 1;                                        \
        size_t* l_realloc =                                                    \
            realloc( l_base, sizeof( size_t ) +                                \
                                 ( l_newLen * sizeof( typeof( *_array ) ) ) ); \
        if ( l_realloc ) {                                                     \
            *l_realloc = l_newLen;                                             \
            *( _array ) = ( typeof( *_array ) )( l_realloc + 1 );              \
            ( *( _array ) )[ l_oldLen ] = ( typeof( **_array ) )( _value );    \
            l_ret = ( size_t )l_oldLen;                                        \
        }                                                                      \
        ( size_t )l_ret;                                                       \
    } )

static FORCE_INLINE ssize_t _insertIntoArray( void*** restrict _array,
                                              void* restrict _value ) {
    ssize_t l_returnValue = -1;

    if ( UNLIKELY( !_array ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !*_array ) ) {
        goto EXIT;
    }

    {
        const size_t l_arrayLength = arrayLength( *_array );
        const ssize_t l_index = ( 1 + l_arrayLength );

        *_array = ( void** )realloc(
            arrayAllocationPointer( *_array ),
            ( sizeof( size_t ) + ( ( l_index + 1 ) * sizeof( void* ) ) ) );

        ( *_array )[ l_index ] = _value;

        ( *arrayLengthPointer( *_array ) )++;

        l_returnValue = l_index;
    }

EXIT:
    return ( l_returnValue );
}

ssize_t findStringInArray( const char** restrict _array,
                           const size_t _arrayLength,
                           const char* restrict _value );

ssize_t findInArray( const size_t* restrict _array,
                     const size_t _arrayLength,
                     const size_t _value );

static FORCE_INLINE bool containsString( const char** restrict _array,
                                         const size_t _arrayLength,
                                         const char* restrict _value ) {
    return ( findStringInArray( _array, _arrayLength, _value ) >= 0 );
}

static FORCE_INLINE bool contains( const size_t* restrict _array,
                                   const size_t _arrayLength,
                                   const size_t _value ) {
    return ( findInArray( _array, _arrayLength, _value ) >= 0 );
}

// Utility functions ( no side-effects ) wrappers for non-naitve array
static FORCE_INLINE ssize_t _findStringInArray( const char** restrict _array,
                                                const char* restrict _value ) {
    return ( findStringInArray(
                 ( const char** )( arrayFirstElementPointer( _array ) ),
                 arrayLength( _array ), _value ) +
             1 );
}

static FORCE_INLINE ssize_t _findInArray( const size_t* restrict _array,
                                          const size_t _value ) {
    return ( findInArray( arrayFirstElementPointer( _array ),
                          arrayLength( _array ), _value ) );
}

static FORCE_INLINE bool _containsString( const char** restrict _array,
                                          const char* restrict _value ) {
    return (
        containsString( ( const char** )( arrayFirstElementPointer( _array ) ),
                        arrayLength( _array ), _value ) );
}

static FORCE_INLINE bool _contains( const size_t* restrict _array,
                                    const size_t _value ) {
    return ( contains( arrayFirstElementPointer( _array ),
                       arrayLength( _array ), _value ) );
}

// Utility OS specific functions ( no side-effects )
char* getApplicationDirectoryAbsolutePath( void );
