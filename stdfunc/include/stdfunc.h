#pragma once

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

#define trim( _string, _from, _to ) \
    do {                            \
        ( *_string ) += _from;      \
        ( *_string )[ _to ] = '\0'; \
    } while ( false );

#define arrayLength( _array ) ( ( size_t )( _array[ 0 ] ) - 1 )
#define arrayLengthNative( _array ) ( sizeof( _array ) / sizeof( _array[ 0 ] ) )
#define randomValueFromArray( _array ) \
    ( _array[ randomNumber() % arrayLength( _array ) ] )
#define arrayFirstElementPointer( _array ) ( _array + 1 )
#define arrayLastElementPointer( _array ) \
    ( ( arrayFirstElementPointer( _array ) - 1 ) + arrayLength( _array ) )

#define _findStringInArray( _array, _value )                      \
    ( findStringInArray(                                          \
          ( const char** )( arrayFirstElementPointer( _array ) ), \
          arrayLength( _array ), _value ) +                       \
      1 )

#define _findInArray( _array, _value )                                      \
    findInArray( arrayFirstElementPointer( _array ), arrayLength( _array ), \
                 _value )

#define _containsString( _array, _value )                                   \
    containsString( ( const char** )( arrayFirstElementPointer( _array ) ), \
                    arrayLength( _array ), _value )

#define _contains( _array, _value )                                      \
    contains( arrayFirstElementPointer( _array ), arrayLength( _array ), \
              _value )

#define FOR( _type, _array )       \
    for ( _type _element = _array; \
          _element < ( _array + arrayLengthNative( _array ) ); _element++ )

#define FOR_ARRAY( _type, _array )                             \
    for ( _type _element = arrayFirstElementPointer( _array ); \
          _element != ( arrayLastElementPointer( _array ) + 1 ); _element++ )

#define FREE_ARRAY( _type, _array ) \
    FOR_ARRAY( _type, _array ) {    \
        SDL_free( *_element );      \
    }                               \
    SDL_free( _array )

// Utility functions ( no side-effects )
size_t lengthOfNumber( size_t _number );
size_t power( size_t _base, uint8_t _exponent );
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
void** createArray( const size_t _elementSize );
void preallocateArray( void*** _array, const size_t _length );
size_t insertIntoArray( void*** _array, void* _value );
void insertIntoArrayByIndex( void*** _array,
                             const size_t _index,
                             void* _value );
ssize_t findStringInArray( const char** _array,
                           const size_t _arrayLength,
                           const char* _value );
ssize_t findInArray( const size_t* _array,
                     const size_t _arrayLength,
                     const size_t _value );
bool containsString( const char** _array,
                     const size_t _arrayLength,
                     const char* _value );
bool contains( const size_t* _array,
               const size_t _arrayLength,
               const size_t _value );

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
