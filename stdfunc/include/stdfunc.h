#pragma once

#include <mimalloc.h>
#include <omp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

// Function attributes
#define FORCE_INLINE __attribute__( ( always_inline ) ) inline
#define NO_OPTIMIZE __attribute__( ( optimize( "0" ) ) )
#define NO_RETURN __attribute__( ( noreturn ) )

// Branch prediction hints
#define LIKELY( _expression ) __builtin_expect( !!( _expression ), 1 )
#define UNLIKELY( _expression ) __builtin_expect( !!( _expression ), 0 )

// Constants
#define ONE_SECOND_IN_MILLISECONDS 1000
#define ONE_MILLISECOND_IN_NANOSECONDS 1000000

// Utility macros ( no side-effects )
#define MILLISECONDS_TO_NANOSECONDS( _milliseconds ) \
    ( _milliseconds * ONE_MILLISECOND_IN_NANOSECONDS )

// Non-native and native array utility functions
#define arrayLengthPointer( _array ) ( ( size_t* )( &( _array[ 0 ] ) ) )
#define arrayLength( _array ) ( ( size_t )( _array[ 0 ] ) - 1 )
#define arrayLengthNative( _array ) ( sizeof( _array ) / sizeof( _array[ 0 ] ) )
#define randomValueFromArray( _array ) \
    ( _array[ randomNumber() % arrayLength( _array ) ] )
#define arrayFirstElementPointer( _array ) ( _array + 1 )
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

// Non-native array free every element
#define FREE_ARRAY( _type, _array )  \
    do {                             \
        FOR_ARRAY( _type, _array ) { \
            mi_free( *_element );    \
        }                            \
        mi_free( _array );           \
    } while ( 0 )

// Utility functions ( no side-effects )
static FORCE_INLINE void trim( char** _string,
                               const ssize_t _from,
                               const ssize_t _to ) {
    if ( _from >= 0 ) {
        ( *_string ) += _from;
    }

    if ( _to >= 0 ) {
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

static FORCE_INLINE size_t power( size_t _base, uint8_t _exponent ) {
    size_t l_returnValue = 1;

    for ( ;; ) {
        if ( _exponent & 1 ) {
            l_returnValue *= _base;
        }

        _exponent >>= 1;

        if ( !_exponent ) {
            break;
        }

        _base *= _base;
    }

    return ( l_returnValue );
}

char* convertNumberToString( size_t _number );
size_t randomNumber( void );
char* duplicateString( const char* _string );
ssize_t findSymbolInString( const char* _string, const char _symbol );
ssize_t findLastSymbolInString( const char* _string, const char _symbol );
size_t concatBeforeAndAfterString( char** _string,
                                   const char* _beforeString,
                                   const char* _afterString );
char* sanitizeString( const char* _string );
char** splitStringIntoArray( const char* _string, const char* _delimiter );

static FORCE_INLINE void** createArray( const size_t _elementSize ) {
    void** l_array = ( void** )mi_malloc( 1 * _elementSize );

    *arrayLengthPointer( l_array ) = ( size_t )( 1 );

    return ( l_array );
}

static FORCE_INLINE void preallocateArray( void*** _array,
                                           const size_t _length ) {
    if ( UNLIKELY( !_array ) ) {
        return;
    }

    if ( UNLIKELY( !*_array ) ) {
        return;
    }

    const size_t l_currentArrayLength = arrayLength( *_array );

    *_array = ( void** )mi_realloc( *_array,
                                    ( ( l_currentArrayLength + _length + 1 ) *
                                      sizeof( ( *_array )[ 0 ] ) ) );

    *arrayLengthPointer( *_array ) =
        ( size_t )( l_currentArrayLength + _length + 1 );
}

static FORCE_INLINE ssize_t insertIntoArray( void*** _array, void* _value ) {
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

        *_array = ( void** )mi_realloc(
            *_array, ( l_index + 1 ) * sizeof( ( *_array )[ 0 ] ) );

        ( *_array )[ l_index ] = _value;

        ( *arrayLengthPointer( *_array ) )++;

        l_returnValue = l_index;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE void insertIntoArrayByIndex( void*** _array,
                                                 const size_t _index,
                                                 void* _value ) {
    ( *_array )[ _index ] = _value;
}

ssize_t findStringInArray( const char** _array,
                           const size_t _arrayLength,
                           const char* _value );

static FORCE_INLINE ssize_t _findStringInArray( const char** _array,
                                                const char* _value ) {
    return ( findStringInArray(
                 ( const char** )( arrayFirstElementPointer( _array ) ),
                 arrayLength( _array ), _value ) +
             1 );
}

ssize_t findInArray( const size_t* _array,
                     const size_t _arrayLength,
                     const size_t _value );

static FORCE_INLINE ssize_t _findInArray( const size_t* _array,
                                          const size_t _value ) {
    return ( findInArray( arrayFirstElementPointer( _array ),
                          arrayLength( _array ), _value ) );
}

static FORCE_INLINE bool containsString( const char** _array,
                                         const size_t _arrayLength,
                                         const char* _value ) {
    return ( findStringInArray( _array, _arrayLength, _value ) >= 0 );
}

static FORCE_INLINE bool _containsString( const char** _array,
                                          const char* _value ) {
    return (
        containsString( ( const char** )( arrayFirstElementPointer( _array ) ),
                        arrayLength( _array ), _value ) );
}

static FORCE_INLINE bool contains( const size_t* _array,
                                   const size_t _arrayLength,
                                   const size_t _value ) {
    return ( findInArray( _array, _arrayLength, _value ) >= 0 );
}

// Utility functions ( no side-effects ) wrappers for non-naitve array
static FORCE_INLINE bool _contains( const size_t* _array,
                                    const size_t _value ) {
    return ( contains( arrayFirstElementPointer( _array ),
                       arrayLength( _array ), _value ) );
}

#if 0
void freeSettingsContent( char*** _content );
ssize_t findKeyInSettings( char*** _settings, const char* _key );
ssize_t findValueInSettings( char*** _settings, const char* _value );
char*** getLabelFromSettingsOrDefault( const char* _label,
                                       const char* _default );
char* getKeyFromSettingsOrDefault( const char* _label,
                                   const char* _key,
                                   const char* _default );
#endif
