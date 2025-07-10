#pragma once

#define XXH_CPU_LITTLE_ENDIAN 1
#define XXH_INLINE_ALL
#define XXH_NO_STDLIB
#define XXH_NO_STREAM
#define XXH_STATIC_LINKING_ONLY

#include <execinfo.h>
#include <libgen.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <xxhash.h>

// Function attributes
#define FORCE_INLINE __attribute__( ( always_inline ) ) inline
#define NO_OPTIMIZE __attribute__( ( optimize( "0" ) ) )
#define NO_RETURN __attribute__( ( noreturn ) )
#define CONST __attribute__( ( const ) )
#define PURE __attribute__( ( pure ) )
#define HOT __attribute__( ( hot ) )
#define COLD __attribute__( ( cold ) )
#define SENTINEL __attribute__( ( sentinel ) )

// Struct attributes
#define PACKED __attribute__( ( packed ) )

// Branch prediction hints
#define LIKELY( _expression ) __builtin_expect( !!( _expression ), 1 )
#define UNLIKELY( _expression ) __builtin_expect( !!( _expression ), 0 )

// Constants
#define COMMENT_SYMBOL ( '#' )
#define DECIMAL_RADIX 10
#define ONE_SECOND_IN_MILLISECONDS ( ( size_t )( 1000 ) )
#define ONE_MILLISECOND_IN_NANOSECONDS ( ( size_t )( 1000000 ) )
#define ASCII_COLOR_CYAN_LIGHT "\e[1;36m"
#define ASCII_COLOR_GREEN "\e[1;32m"
#define ASCII_COLOR_PURPLE_LIGHT "\e[1;35m"
#define ASCII_COLOR_RED "\e[1;31m"
#define ASCII_COLOR_YELLOW "\e[1;33m"
#define ASCII_COLOR_RESET_FOREGROUND "\e[39m"
#define ASCII_COLOR_RESET_BACKGROUND "\e[49m"
#define ASCII_COLOR_RESET "\e[0m"

// Utility macros ( no side-effects )
#define SECONDS_TO_MILLISECONDS( _seconds ) \
    ( ( _seconds ) * ONE_SECOND_IN_MILLISECONDS )
#define MILLISECONDS_TO_NANOSECONDS( _milliseconds ) \
    ( ( _milliseconds ) * ONE_MILLISECOND_IN_NANOSECONDS )
#define BITS_TO_BYTES( _bits ) ( ( size_t )( ( _bits ) / 8 ) )
// This macro turns a value into a string literal
#define STRINGIFY_MACRO( _value ) #_value
// This is a helper macro that handles the stringify
#define MACRO_TO_STRING( _macro ) STRINGIFY_MACRO( _macro )

// Utility functions ( no side-effects )
#define max( _a, _b ) ( ( ( _a ) > ( _b ) ) ? ( _a ) : ( _b ) )
#define min( _a, _b ) ( ( ( _a ) < ( _b ) ) ? ( _a ) : ( _b ) )

// Utility functions ( side-effects )
#define UNUSED( _variable ) ( ( void )( sizeof( _variable ) ) )
#define ASSUME( _expression ) __builtin_assume( _expression )
#define CHECK_CONSTANT_ARGUMENT( _argument ) \
    ( __builtin_constant_p( _argument ) )
#define CHECK_CONSTANT_ARGUMENT_CRITICAL( _argument )       \
    ( _Static_assert( CHECK_CONSTANT_ARGUMENT( _argument ), \
                      "Argument is not a compile-time constant" ) )

// TODO: Implement
#if 0
#define CHECK_CONSTANT_FUNCTION_CRITICAL( _function, ... )                \
    ( _Static_assert(                                                     \
        ( ( _function( __VA_ARGS__ ) ) == ( _function( __VA_ARGS__ ) ) ), \
        "Function cannot be executed at compile-time" ) )
#endif

#if ( defined( DEBUG ) && !defined( TESTS ) )

#if defined( assert )

#undef assert

#endif

#define TRAP_COLOR_FILE_NAME ASCII_COLOR_PURPLE_LIGHT
#define TRAP_COLOR_LINE_NUMBER ASCII_COLOR_PURPLE_LIGHT
#define TRAP_COLOR_FUNCTION_NAME ASCII_COLOR_PURPLE_LIGHT

#define DEBUG_INFORMATION1                                                  \
    TRAP_COLOR_FILE_NAME ASCII_COLOR_RESET_FOREGROUND                       \
        "File '" TRAP_COLOR_FILE_NAME __FILE__ ASCII_COLOR_RESET_FOREGROUND \
        "': line " TRAP_COLOR_LINE_NUMBER                                   \
        MACRO_TO_STRING( __LINE__ ) ASCII_COLOR_RESET_FOREGROUND            \
        " in function '" TRAP_COLOR_FUNCTION_NAME

#define DEBUG_INFORMATION2 ASCII_COLOR_RESET_FOREGROUND "'"
#define DEBUG_INFORMATION3 " | Message: "

#define BACKTRACE_LIMIT ( 5 )

#define trap( ... )                                                      \
    do {                                                                 \
        write( STDERR_FILENO, DEBUG_INFORMATION1,                        \
               __builtin_strlen( DEBUG_INFORMATION1 ) );                 \
        write( STDERR_FILENO, __func__, __builtin_strlen( __func__ ) );  \
        write( STDERR_FILENO, DEBUG_INFORMATION2,                        \
               __builtin_strlen( DEBUG_INFORMATION2 ) );                 \
        __VA_OPT__( write( STDERR_FILENO, DEBUG_INFORMATION3,            \
                           __builtin_strlen( DEBUG_INFORMATION3 ) );     \
                    const char l_message[] = __VA_ARGS__;                \
                    write( STDERR_FILENO, l_message,                     \
                           __builtin_strlen( l_message ) ); );           \
        write( STDERR_FILENO, "\n", 1 );                                 \
        void* l_backtraceBuffer[ BACKTRACE_LIMIT ];                      \
        const size_t l_backtraceAmount =                                 \
            backtrace( l_backtraceBuffer, BACKTRACE_LIMIT );             \
        char** l_backtraceResolved =                                     \
            backtrace_symbols( l_backtraceBuffer, l_backtraceAmount );   \
        FOR_RANGE( size_t, 0, l_backtraceAmount ) {                      \
            char* l_backtrace = l_backtraceResolved[ _index ];           \
            char* l_fileNameEnd = __builtin_strchr( l_backtrace, '(' );  \
            *l_fileNameEnd = '\0';                                       \
            char* l_fileName = ( __builtin_strrchr( l_backtrace, '/' ) + \
                                 ( 1 * sizeof( char ) ) );               \
            write( STDERR_FILENO, l_fileName,                            \
                   __builtin_strlen( l_fileName ) );                     \
            *l_fileNameEnd = '(';                                        \
            write( STDERR_FILENO, l_fileNameEnd,                         \
                   __builtin_strlen( l_fileNameEnd ) );                  \
            write( STDERR_FILENO, ASCII_COLOR_RESET,                     \
                   __builtin_strlen( ASCII_COLOR_RESET ) );              \
            write( STDERR_FILENO, "\n", 1 );                             \
        }                                                                \
        free( l_backtraceResolved );                                     \
        __builtin_trap();                                                \
    } while ( 0 )

#define assert( _expression, ... ) \
    do {                           \
        if ( !( _expression ) ) {  \
            trap( __VA_ARGS__ );   \
        }                          \
    } while ( 0 )

#else

#define trap( ... ) ( ( void )0 )
#define assert( _expression, ... ) ( ( void )0 )

#endif

// Non-native and native array utility functions
#define arrayLengthPointer( _array ) \
    ( ( arrayLength_t* )( ( char* )( _array ) - sizeof( arrayLength_t ) ) )
#define arrayLength( _array ) ( *arrayLengthPointer( _array ) )

#define arrayAllocationPointer( _array ) ( arrayLengthPointer( _array ) )
#define arrayFirstElementPointer( _array ) ( _array )
#define arrayLastElementPointer( _array ) \
    ( ( _array ) + arrayLength( _array ) - 1 )

#define arrayFirstElement( _array ) ( *arrayFirstElementPointer( _array ) )
#define arrayLastElement( _array ) ( *arrayLastElementPointer( _array ) )

#define arrayLastElementIndex( _array ) \
    ( arrayLastElementPointer( _array ) - arrayFirstElementPointer( _array ) )

#define randomValueFromArray( _array ) \
    ( ( _array )[ randomNumber() % arrayLength( _array ) ] )

#define arrayLengthNative( _array ) \
    ( sizeof( ( _array ) ) / sizeof( ( _array )[ 0 ] ) )

#define arrayFirstElementPointerNative( _array ) ( _array )
#define arrayLastElementPointerNative( _array ) \
    ( ( _array ) + arrayLengthNative( _array ) - 1 )

#define arrayFirstElementNative( _array ) \
    ( *arrayFirstElementPointerNative( _array ) )
#define arrayLastElementNative( _array ) \
    ( *arrayLastElementPointerNative( _array ) )

#define arrayLastElementIndexNative( _array )   \
    ( arrayLastElementPointerNative( _array ) - \
      arrayFirstElementPointerNative( _array ) )

#define randomValueFromArrayNative( _array ) \
    ( ( _array )[ randomNumber() % arrayLengthNative( _array ) ] )

// Native array iteration FOR
#define FOR( _type, _array )                                       \
    for ( _type _element = ( _array );                             \
          _element < ( ( _array ) + arrayLengthNative( _array ) ); \
          _element++ )

#define FOR_REVERSE( _type, _array )                                         \
    for ( _type _element = ( ( _array ) + arrayLengthNative( _array ) - 1 ); \
          _element > ( _array ); _element-- )

// Non-native array iteration FOR
#define FOR_ARRAY( _type, _array )                             \
    for ( _type _element = arrayFirstElementPointer( _array ); \
          _element != ( arrayLastElementPointer( _array ) + 1 ); _element++ )

#define FOR_ARRAY_REVERSE( _type, _array )                    \
    for ( _type _element = arrayLastElementPointer( _array ); \
          _element != ( arrayFirstElementPointer( _array ) - 1 ); _element-- )

// Range iteration FOR
#define FOR_RANGE( _type, _start, _end ) \
    for ( _type _index = ( _start ); _index < ( _end ); _index++ )

#define FOR_RANGE_REVERSE( _type, _start, _end ) \
    for ( _type _index = ( _start ); _index > ( _end ); _index-- )

// Range iteration FOR with increase BY amount
#define FOR_RANGE_BY( _type, _start, _end, _amount ) \
    for ( _type _index = ( _start ); _index < ( _end ); _index += ( _amount ) )

#define FOR_RANGE_BY_REVERSE( _type, _start, _end, _amount ) \
    for ( _type _index = ( _start ); _index > ( _end ); _index -= ( _amount ) )

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

typedef size_t arrayLength_t;

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

static FORCE_INLINE float clamp$float( float _value,
                                       float _valueMin,
                                       float _valueMax ) {
    return (
        __builtin_fminf( __builtin_fmaxf( _value, _valueMin ), _valueMax ) );
}

static FORCE_INLINE void trim( char* restrict* restrict _string,
                               const size_t _from,
                               const size_t _to ) {
    ( *_string ) += _from;
    ( *_string )[ _to ] = '\0';
}

// clang-format off
#define lengthOfNumber( _number ) ( {\
    ( (_number < 10ULL) ) ? ( 1 ) :\
    ( (_number < 100ULL) ) ? ( 2 ) :\
    ( (_number < 1000ULL) ) ? ( 3 ) :\
    ( (_number < 10000ULL) ) ? ( 4 ) :\
    ( (_number < 100000ULL) ) ? ( 5 ) :\
    ( (_number < 1000000ULL) ) ? ( 6 ) :\
    ( (_number < 10000000ULL) ) ? ( 7 ) :\
    ( (_number < 100000000ULL) ) ? ( 8 ) :\
    ( (_number < 1000000000ULL) ) ? ( 9 ) :\
    ( (_number < 10000000000ULL) ) ? ( 10 ) :\
    ( (_number < 100000000000ULL) ) ? ( 11 ) :\
    ( (_number < 1000000000000ULL) ) ? ( 12 ) :\
    ( (_number < 10000000000000ULL) ) ? ( 13 ) :\
    ( (_number < 100000000000000ULL) ) ? ( 14 ) :\
    ( (_number < 1000000000000000ULL) ) ? ( 15 ) :\
    ( (_number < 10000000000000000ULL) ) ? ( 16 ) :\
    ( (_number < 100000000000000000ULL) ) ? ( 17 ) :\
    ( (_number < 1000000000000000000ULL) ) ? ( 18 ) :\
    ( (_number < 10000000000000000000ULL) ) ? ( 19 ) :\
    ( 20 );\
} )
// clang-format on

static FORCE_INLINE char* numberToString( size_t _number ) {
    char* l_returnValue = NULL;

    {
        const size_t l_numberLength = lengthOfNumber( _number );

        l_returnValue =
            ( char* )malloc( ( l_numberLength + 1 ) * sizeof( char ) );

        FOR_RANGE_REVERSE( ssize_t, ( l_numberLength - 1 ), ( 0 - 1 ) ) {
            l_returnValue[ _index ] = ( ( _number % 10 ) + '0' );

            _number /= 10;
        }

        l_returnValue[ l_numberLength ] = '\0';
    }

    return ( l_returnValue );
}

#define RANDOM_NUMBER_MAX SIZE_MAX

void randomNumber$seed$set( const size_t _seed );
size_t randomNumber$seed$get( void );
size_t randomNumber( void );

static FORCE_INLINE size_t generateHash( const uint8_t* restrict _data,
                                         const size_t _dataSize ) {
    size_t l_returnValue = 0;

    if ( !_data ) {
        trap();

        goto EXIT;
    }

    if ( !_dataSize ) {
        trap();

        goto EXIT;
    }

    {
        l_returnValue = XXH32( _data, _dataSize, randomNumber$seed$get() );
    }

EXIT:
    return ( l_returnValue );
}

#define clone( _element )                                                  \
    ( {                                                                    \
        typeof( _element ) l_allocated =                                   \
            ( typeof( _element ) )malloc( sizeof( typeof( *_element ) ) ); \
        __builtin_memcpy( l_allocated, _element,                           \
                          sizeof( typeof( *_element ) ) );                 \
        ( l_allocated );                                                   \
    } )

static FORCE_INLINE char* duplicateString( const char* restrict _string ) {
    char* l_returnValue = NULL;

    if ( UNLIKELY( !_string ) ) {
        trap();

        goto EXIT;
    }

    {
        const size_t l_stringLength = __builtin_strlen( _string );

        l_returnValue =
            ( char* )malloc( ( l_stringLength + 1 ) * sizeof( char ) );

        __builtin_memcpy( l_returnValue, _string, ( l_stringLength + 1 ) );
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE ssize_t findSymbolInString( const char* restrict _string,
                                                const char _symbol ) {
    ssize_t l_returnValue = -1;

    if ( UNLIKELY( !_string ) ) {
        trap();

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
        trap();

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

#define createArray( _elementType )                              \
    ( {                                                          \
        arrayLength_t* l_array =                                 \
            ( arrayLength_t* )malloc( sizeof( arrayLength_t ) ); \
        *l_array = ( arrayLength_t )0;                           \
        ( _elementType* )( l_array + 1 );                        \
    } )

#define preallocateArray( _array, _length )                                    \
    do {                                                                       \
        arrayLength_t* l_arrayAllocationCurrent =                              \
            arrayAllocationPointer( *( _array ) );                             \
        const arrayLength_t l_arrayLengthCurrent = arrayLength( *( _array ) ); \
        const arrayLength_t l_arrayLengthNew =                                 \
            ( l_arrayLengthCurrent + ( _length ) );                            \
        arrayLength_t* l_arrayAllocationNew = ( arrayLength_t* )realloc(       \
            l_arrayAllocationCurrent,                                          \
            ( sizeof( arrayLength_t ) +                                        \
              ( l_arrayLengthNew * sizeof( typeof( *( _array ) ) ) ) ) );      \
        if ( l_arrayAllocationNew ) {                                          \
            *l_arrayAllocationNew = l_arrayLengthNew;                          \
            *( _array ) =                                                      \
                ( typeof( *( _array ) ) )( l_arrayAllocationNew + 1 );         \
        }                                                                      \
    } while ( 0 )

#define insertIntoArray( _array, _value )                                      \
    ( {                                                                        \
        arrayLength_t* l_arrayAllocationCurrent =                              \
            arrayAllocationPointer( *( _array ) );                             \
        const arrayLength_t l_arrayLengthCurrent = arrayLength( *( _array ) ); \
        const arrayLength_t l_arrayLengthNew = ( l_arrayLengthCurrent + 1 );   \
        arrayLength_t* l_arrayAllocationNew = ( arrayLength_t* )realloc(       \
            l_arrayAllocationCurrent,                                          \
            ( sizeof( arrayLength_t ) +                                        \
              ( l_arrayLengthNew * sizeof( typeof( *( _array ) ) ) ) ) );      \
        *l_arrayAllocationNew = l_arrayLengthNew;                              \
        *( _array ) = ( typeof( *( _array ) ) )( l_arrayAllocationNew + 1 );   \
        ( *( _array ) )[ l_arrayLengthCurrent ] =                              \
            ( typeof( **( _array ) ) )( _value );                              \
        ( ( arrayLength_t )l_arrayLengthCurrent );                             \
    } )

#define pluckArray( _array, _value )                                           \
    ( {                                                                        \
        arrayLength_t* l_arrayAllocationCurrent =                              \
            arrayAllocationPointer( *( _array ) );                             \
        const arrayLength_t l_arrayLengthCurrent = arrayLength( *( _array ) ); \
        FOR_ARRAY( typeof( *_array ), *_array ) {                              \
            if ( *_element == ( typeof( **( _array ) ) )( _value ) ) {         \
                if ( _element != arrayLastElementPointer( *_array ) ) {        \
                    *arrayLastElementPointer( *_array ) =                      \
                        ( typeof( **( _array ) ) )( _value );                  \
                }                                                              \
                const arrayLength_t l_arrayLengthNew =                         \
                    ( l_arrayLengthCurrent - 1 );                              \
                arrayLength_t* l_arrayAllocationNew =                          \
                    ( arrayLength_t* )realloc(                                 \
                        l_arrayAllocationCurrent,                              \
                        ( sizeof( arrayLength_t ) +                            \
                          ( l_arrayLengthNew *                                 \
                            sizeof( typeof( *( _array ) ) ) ) ) );             \
                *l_arrayAllocationNew = l_arrayLengthNew;                      \
                *( _array ) =                                                  \
                    ( typeof( *( _array ) ) )( l_arrayAllocationNew + 1 );     \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        ( arrayLength( *( _array ) ) != l_arrayLengthCurrent );                \
    } )

#define removeLastArray( _array )                                              \
    do {                                                                       \
        arrayLength_t* l_arrayAllocationCurrent =                              \
            arrayAllocationPointer( *( _array ) );                             \
        const arrayLength_t l_arrayLengthCurrent = arrayLength( *( _array ) ); \
        const arrayLength_t l_arrayLengthNew = ( l_arrayLengthCurrent - 1 );   \
        arrayLength_t* l_arrayAllocationNew = ( arrayLength_t* )realloc(       \
            l_arrayAllocationCurrent,                                          \
            ( sizeof( arrayLength_t ) +                                        \
              ( l_arrayLengthNew * sizeof( typeof( *( _array ) ) ) ) ) );      \
        *l_arrayAllocationNew = l_arrayLengthNew;                              \
        *( _array ) = ( typeof( *( _array ) ) )( l_arrayAllocationNew + 1 );   \
    } while ( 0 )

ssize_t findStringInArray( char* restrict* restrict _array,
                           const size_t _arrayLength,
                           char* restrict _value );

ssize_t findInArray( const size_t* restrict _array,
                     const size_t _arrayLength,
                     const size_t _value );

static FORCE_INLINE bool containsString( char* restrict* restrict _array,
                                         const size_t _arrayLength,
                                         char* restrict _value ) {
    return ( findStringInArray( _array, _arrayLength, _value ) >= 0 );
}

static FORCE_INLINE bool contains( const size_t* restrict _array,
                                   const size_t _arrayLength,
                                   const size_t _value ) {
    return ( findInArray( _array, _arrayLength, _value ) >= 0 );
}

// Utility functions ( no side-effects ) wrappers for non-naitve array
static FORCE_INLINE ssize_t _findStringInArray( char* restrict* restrict _array,
                                                char* restrict _string ) {
    return ( findStringInArray( arrayFirstElementPointer( _array ),
                                arrayLength( _array ), _string ) );
}

static FORCE_INLINE ssize_t _findInArray( const size_t* restrict _array,
                                          const size_t _value ) {
    return ( findInArray( arrayFirstElementPointer( _array ),
                          arrayLength( _array ), _value ) );
}

static FORCE_INLINE bool _containsString( char* restrict* restrict _array,
                                          char* restrict _string ) {
    return ( containsString( arrayFirstElementPointer( _array ),
                             arrayLength( _array ), _string ) );
}

static FORCE_INLINE bool _contains( const size_t* restrict _array,
                                    const size_t _value ) {
    return ( contains( arrayFirstElementPointer( _array ),
                       arrayLength( _array ), _value ) );
}

// Utility OS specific functions ( no side-effects )
static FORCE_INLINE char* getApplicationDirectoryAbsolutePath( void ) {
    char* l_returnValue = NULL;

    {
        char* l_executablePath = ( char* )malloc( PATH_MAX * sizeof( char ) );

        // Get executable path
        {
            ssize_t l_executablePathLength = readlink(
                "/proc/self/exe", l_executablePath, ( PATH_MAX - 1 ) );

            if ( UNLIKELY( l_executablePathLength == -1 ) ) {
                free( l_executablePath );

                goto EXIT;
            }

            l_executablePath[ l_executablePathLength ] = '\0';
        }

        char* l_directoryPath = NULL;

        // Get directory path
        {
            l_directoryPath = l_executablePath;

            // Do not move the beginning
            // Keep last '/'
            trim( &l_directoryPath, 0,
                  ( findLastSymbolInString( l_executablePath, '/' ) + 1 ) );
        }

        l_returnValue = l_directoryPath;
    }

EXIT:
    return ( l_returnValue );
}

static FORCE_INLINE bool comparePathsDirectories( const char* _path1,
                                                  const char* _path2 ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_path1 ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_path2 ) ) {
        goto EXIT;
    }

    {
        char l_path1[ PATH_MAX ];
        __builtin_memcpy( l_path1, _path1, ( __builtin_strlen( _path1 ) + 1 ) );

        char l_path2[ PATH_MAX ];
        __builtin_memcpy( l_path2, _path2, ( __builtin_strlen( _path2 ) + 1 ) );

        l_returnValue =
            ( __builtin_strcmp( dirname( l_path1 ), dirname( l_path2 ) ) == 0 );
    }

EXIT:
    return ( l_returnValue );
}

bool doesPathExist( const char* restrict _path );

bool isPathDirectory( const char* restrict _path );

char** getPathsByGlob( const char* restrict _glob,
                       const char* restrict _directory,
                       const bool _needSort );

// Utility Compiler specific functions ( no side-effects )
// format - "%s%s %s = %p'
static FORCE_INLINE void dumpCallback( void* _callback,
                                       void* _context,
                                       const char* restrict _format,
                                       ... )
    __attribute__( ( format( printf,
                             3, // Format index
                             4  // First format argument index
                             ) ) ) {
    static size_t l_depth = 0;

    va_list l_arguments;

    va_start( l_arguments, _format );

    // Skip nested structs
    if ( findSymbolInString( _format, '{' ) != -1 ) {
        l_depth++;
    }

    // Only act on top-level fields
    if ( ( l_depth == 1 ) && ( findSymbolInString( _format, '=' ) != -1 ) ) {
        // Skip indentation
        va_arg( l_arguments, char* );

        // Skip type
        va_arg( l_arguments, char* );

        char* l_fieldName = duplicateString( va_arg( l_arguments, char* ) );

        // Field name
        bool l_result =
            ( ( bool ( * )( char* _fieldName, void* _context ) )_callback )(
                l_fieldName, _context );

        free( l_fieldName );

        if ( UNLIKELY( !l_result ) ) {
            goto EXIT;
        }
    }

    if ( findSymbolInString( _format, '}' ) != -1 ) {
        l_depth--;
    }

EXIT:
    va_end( l_arguments );
}

#define iterateTopMostFields( _type, _callback, _context )                   \
    do {                                                                     \
        _type l_structSample = { 0 };                                        \
        __builtin_dump_struct( &l_structSample, dumpCallback, ( _callback ), \
                               ( _context ) );                               \
    } while ( 0 )
