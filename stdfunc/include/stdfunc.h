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
#include <stdlib.h>
#include <sys/types.h>
#include <xxhash.h>

// Function attributes
#define FORCE_INLINE __attribute__( ( always_inline ) ) inline
#define NO_OPTIMIZE __attribute__( ( optimize( "0" ) ) )
#define NO_RETURN __attribute__( ( noreturn ) )
#define HOT __attribute__( ( hot ) )
#define COLD __attribute__( ( cold ) )

// Branch prediction hints
#define LIKELY( _expression ) __builtin_expect( !!( _expression ), 1 )
#define UNLIKELY( _expression ) __builtin_expect( !!( _expression ), 0 )

// Constants
#define ONE_SECOND_IN_MILLISECONDS 1000
#define ONE_MILLISECOND_IN_NANOSECONDS 1000000

// Utility macros ( no side-effects )
#define MILLISECONDS_TO_NANOSECONDS( _milliseconds ) \
    ( _milliseconds * ONE_MILLISECOND_IN_NANOSECONDS )

// Utility functions ( no side-effects )
#define max( _a, _b ) ( ( _a > _b ) ? ( _a ) : ( _b ) )
#define min( _a, _b ) ( ( _a < _b ) ? ( _a ) : ( _b ) )

// Non-native and native array utility functions
#define arrayElementSizePointer( _array ) \
    ( ( uint8_t* )( ( uint8_t* )_array + 0 ) )
#define arrayLengthPointer( _array ) \
    ( ( size_t* )( ( uint8_t* )_array + sizeof( uint8_t ) ) )
#define arrayElementByIndexPointer( _array, _index )                   \
    ( ( typeof( _array ) )( ( ( uint8_t* )_array + sizeof( uint8_t ) + \
                              sizeof( size_t ) ) +                     \
                            ( _index * arrayElementSize( _array ) ) ) )

#define arrayElementSize( _array ) ( *arrayElementSizePointer( _array ) )
#define arrayLength( _array ) ( ( *arrayLengthPointer( _array ) ) - 1 )
#define arrayLengthNative( _array ) ( sizeof( _array ) / sizeof( _array[ 0 ] ) )
#define arrayElementByIndex( _array, _index ) \
    ( *arrayElementByIndexPointer( _array, _index ) )

#define randomValueFromArray( _array ) \
    ( _array[ randomNumber() % arrayLength( _array ) ] )
#define arrayFirstElementPointer( _array ) \
    ( arrayElementByIndexPointer( _array, 0 ) )
#define arrayLastElementPointer( _array ) \
    ( ( arrayFirstElementPointer( _array ) - 1 ) + arrayLength( _array ) )

// Native array iteration FOR
#define FOR( _type, _array )       \
    for ( _type _element = _array; \
          _element < ( _array + arrayLengthNative( _array ) ); _element++ )

// Non-native array iteration FOR
#define FOR_ARRAY( _type, _array )                             \
    for ( _type _element = arrayFirstElementPointer( _array ); \
          _element != ( arrayLastElementPointer( _array ) + 1 ); _element++ )

// Range iteration FOR
#define FOR_RANGE( _type, _start, _end ) \
    for ( _type _index = _start; _index < _end; _index++ )

// Range iteration FOR with increase BY amount
#define FOR_RANGE_BY( _type, _start, _end, _amount ) \
    for ( _type _index = _start; _index < _end; _index += _amount )

// Non-native array free every element
#define FREE_ARRAY( _array )                    \
    do {                                        \
        FOR_ARRAY( typeof( _array ), _array ) { \
            free( *_element );                  \
        }                                       \
        free( _array );                         \
    } while ( 0 )

// Utility functions ( no side-effects )
static FORCE_INLINE bool stringToBool( const char* restrict _string ) {
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
    size_t l_length = 0;

    do {
        l_length++;

        _number /= 10;
    } while ( _number );

    return ( l_length );
}

void randomNumber$seed$set( const size_t _seed );
size_t randomNumber$seed$get( void );
size_t randomNumber( void );

static FORCE_INLINE size_t generateHash( uint8_t* restrict _data,
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

char* duplicateString( const char* restrict _string );

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

static FORCE_INLINE void** createArray( const uint8_t _elementSize ) {
    void** l_array = ( void** )malloc( ( 1 * sizeof( uint8_t ) ) +
                                       ( 1 * sizeof( size_t ) ) );

    *arrayElementSizePointer( l_array ) = _elementSize;
    *arrayLengthPointer( l_array ) = ( size_t )( 1 );

    return ( l_array );
}

static FORCE_INLINE void preallocateArray( void*** restrict _array,
                                           const size_t _length ) {
    if ( UNLIKELY( !_array ) ) {
        return;
    }

    if ( UNLIKELY( !*_array ) ) {
        return;
    }

    const size_t l_currentArrayLength = arrayLength( *_array );

    *_array = ( void** )realloc( *_array,
                                 ( sizeof( uint8_t ) + sizeof( size_t ) +
                                   ( l_currentArrayLength + _length ) *
                                       *arrayElementSizePointer( *_array ) ) );

    *arrayLengthPointer( *_array ) += _length;
}

static FORCE_INLINE ssize_t insertIntoArray( void*** restrict _array,
                                             void* restrict _value ) {
    ssize_t l_returnValue = -1;

    if ( UNLIKELY( !_array ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !*_array ) ) {
        goto EXIT;
    }

    {
        const size_t l_currentArrayLength = arrayLength( *_array );

        *_array = ( void** )realloc(
            *_array, ( sizeof( uint8_t ) + sizeof( size_t ) +
                       ( ( l_currentArrayLength + 1 ) *
                         *arrayElementSizePointer( *_array ) ) ) );

        *arrayElementByIndexPointer( *_array, l_currentArrayLength ) = _value;

        ( *arrayLengthPointer( *_array ) )++;

        l_returnValue = arrayLength( *_array );
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE void insertIntoArrayByIndex( void*** restrict _array,
                                                 void* restrict _value,
                                                 const size_t _index ) {
    if ( UNLIKELY( !_array ) ) {
        return;
    }

    if ( UNLIKELY( !*_array ) ) {
        return;
    }

    {
        *arrayElementByIndexPointer( *_array, _index ) = _value;
    }
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
