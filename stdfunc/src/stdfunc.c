#include "stdfunc.h"

#include <limits.h>
#include <string.h>
#include <unistd.h>

#include "log.h"

#if 0
#include "_useCallback.h"
#endif

char* convertNumberToString( size_t _number ) {
    char* l_returnValue = NULL;

    {
        const size_t l_lengthOfNumber = lengthOfNumber( _number );
        char* l_buffer = ( char* )malloc( l_lengthOfNumber + 1 );

#pragma omp simd
        for ( ssize_t _characterIndex = ( l_lengthOfNumber - 1 );
              _characterIndex >= 0; _characterIndex-- ) {
            l_buffer[ _characterIndex ] = ( '0' + ( _number % 10 ) );

            _number /= 10;
        }

        l_buffer[ l_lengthOfNumber ] = '\0';

        l_returnValue = l_buffer;
    }

    return ( l_returnValue );
}

// TODO: Make seed changeable
size_t randomNumber( void ) {
    size_t l_returnValue = 0;

    {
#define COMPILATION_TIME_AS_SEED                                      \
    ( __TIME__[ 0 ] + __TIME__[ 1 ] + __TIME__[ 2 ] + __TIME__[ 3 ] + \
      __TIME__[ 4 ] + __TIME__[ 5 ] )

        static uint32_t l_seed = COMPILATION_TIME_AS_SEED;

        l_seed ^= ( l_seed << 13 );
        l_seed ^= ( l_seed >> 17 );
        l_seed ^= ( l_seed << 5 );

        l_returnValue = l_seed;

#undef COMPILATION_TIME_AS_SEED
    }

    return ( l_returnValue );
}

char* duplicateString( const char* _string ) {
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

ssize_t findSymbolInString( const char* _string, const char _symbol ) {
    ssize_t l_returnValue = -1;

    if ( UNLIKELY( !_string ) ) {
        goto EXIT;
    }

    for ( size_t _index = 0; ( _string[ _index ] != '\0' ); _index++ ) {
        const char l_symbol = _string[ _index ];

        if ( l_symbol == _symbol ) {
            l_returnValue = _index;

            break;
        }
    }

EXIT:
    return ( l_returnValue );
}

ssize_t findLastSymbolInString( const char* _string, const char _symbol ) {
    ssize_t l_returnValue = -1;

    if ( UNLIKELY( !_string ) ) {
        goto EXIT;
    }

    for ( size_t _index = 0; ( _string[ _index ] != '\0' ); _index++ ) {
        const char l_symbol = _string[ _index ];

        if ( l_symbol == _symbol ) {
            l_returnValue = _index;
        }
    }

EXIT:
    return ( l_returnValue );
}

size_t concatBeforeAndAfterString( char** _string,
                                   const char* _beforeString,
                                   const char* _afterString ) {
    size_t l_returnValue = 0;

    {
        size_t l_stringLength = 0;

        // TODO: Discover if _string check is reduntant by *_string
        if ( LIKELY( _string ) && LIKELY( *_string ) ) {
            l_stringLength = __builtin_strlen( *_string );
        }

        size_t l_beforeStringLength = 0;

        if ( LIKELY( _beforeString ) ) {
            l_beforeStringLength = __builtin_strlen( _beforeString );
        }

        size_t l_afterStringLegnth = 0;

        if ( LIKELY( _afterString ) ) {
            l_afterStringLegnth = __builtin_strlen( _afterString );
        }

        const size_t l_totalLength =
            ( l_beforeStringLength + l_stringLength + l_afterStringLegnth );

        if ( UNLIKELY( !l_totalLength ) ) {
            goto EXIT;
        }

        // If NULL is passed, only the total length is calculated and returned
        if ( UNLIKELY( !_string ) ) {
            goto EXIT;
        }

        {
            char* l_buffer;

            if ( LIKELY( *_string ) ) {
                l_buffer = ( char* )malloc( l_stringLength * sizeof( char ) );

                __builtin_memcpy( l_buffer, *_string, l_stringLength );

                *_string = ( char* )realloc(
                    *_string, ( l_totalLength + 1 ) * sizeof( char ) );

            } else {
                *_string =
                    ( char* )malloc( ( l_totalLength + 1 ) * sizeof( char ) );
            }

            if ( LIKELY( _beforeString ) ) {
                __builtin_memcpy( *_string, _beforeString,
                                  l_beforeStringLength );
            }

            if ( LIKELY( *_string ) ) {
                __builtin_memcpy( ( l_beforeStringLength + *_string ), l_buffer,
                                  l_stringLength );

                free( l_buffer );
            }

            if ( LIKELY( _afterString ) ) {
                __builtin_memcpy(
                    ( l_beforeStringLength + l_stringLength + *_string ),
                    _afterString, l_afterStringLegnth );
            }

            ( *_string )[ l_totalLength ] = '\0';
        }

        l_returnValue = l_totalLength;
    }

EXIT:
    return ( l_returnValue );
}

char* sanitizeString( const char* _string ) {
    char* l_returnValue = NULL;

    if ( UNLIKELY( !_string ) ) {
        goto EXIT;
    }

    {
        const size_t l_stringLength = __builtin_strlen( _string );
        char* l_buffer =
            ( char* )malloc( ( l_stringLength + 1 ) * sizeof( char ) );
        size_t l_bufferLength = 0;

#define COMMENT_SYMBOL ( '#' )

        for ( const char* _symbol = _string;
              _symbol < ( _string + l_stringLength ); _symbol++ ) {
            if ( __builtin_isspace( *_symbol ) ) {
                continue;

            } else if ( *_symbol == COMMENT_SYMBOL ) {
                break;
            }

            l_buffer[ l_bufferLength ] = *_symbol;
            l_bufferLength++;
        }

#undef COMMENT_SYMBOL

        l_buffer[ l_bufferLength ] = '\0';
        l_bufferLength++;

        l_buffer = ( char* )realloc( l_buffer, l_bufferLength );

        l_returnValue = l_buffer;
    }

EXIT:
    return ( l_returnValue );
}

char** splitStringIntoArray( const char* _string, const char* _delimiter ) {
    char** l_returnValue = ( char** )createArray( sizeof( char* ) );

    if ( UNLIKELY( !_string ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_delimiter ) ) {
        goto EXIT;
    }

    {
        char* l_string = duplicateString( _string );
        char* l_splitted = strtok( l_string, _delimiter );

        while ( l_splitted ) {
            insertIntoArray( ( void*** )&l_returnValue,
                             duplicateString( l_splitted ) );

            l_splitted = strtok( NULL, _delimiter );
        }

        free( l_string );
    }

EXIT:
    return ( l_returnValue );
}

// TODO: Implement
char** splitStringIntoArrayBySymbol( const char* _string, const char _symbol ) {
    char l_delimiter[ 2 ] = " ";

    l_delimiter[ 0 ] = _symbol;

    return ( splitStringIntoArray( _string, l_delimiter ) );
}

ssize_t findStringInArray( const char** _array,
                           const size_t _arrayLength,
                           const char* _value ) {
    ssize_t l_returnValue = -1;

    if ( UNLIKELY( !_array ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_arrayLength ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_value ) ) {
        goto EXIT;
    }

    {
        ssize_t l_index = -1;

        for ( size_t _index = 0; _index < _arrayLength; _index++ ) {
            const char* l_value = _array[ _index ];

            if ( UNLIKELY( !l_value ) ) {
                continue;
            }

            if ( __builtin_strcmp( l_value, _value ) == 0 ) {
                l_index = _index;

                break;
            }
        }

        l_returnValue = l_index;
    }

EXIT:
    return ( l_returnValue );
}

ssize_t findInArray( const size_t* _array,
                     const size_t _arrayLength,
                     const size_t _value ) {
    ssize_t l_returnValue = -1;

    if ( UNLIKELY( !_array ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_arrayLength ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_value ) ) {
        goto EXIT;
    }

    {
        ssize_t l_index = -1;

        for ( size_t _index = 0; _index < _arrayLength; _index++ ) {
            if ( _array[ _index ] == _value ) {
                l_index = _index;

                break;
            }
        }

        l_returnValue = l_index;
    }

EXIT:
    return ( l_returnValue );
}

char* getApplicationDirectoryAbsolutePath( void ) {
    char* l_returnValue = NULL;

    {
        char* l_executablePath = ( char* )malloc( PATH_MAX * sizeof( char ) );

        // Get executable path
        {
            ssize_t l_executablePathLength = readlink(
                "/proc/self/exe", l_executablePath, ( PATH_MAX - 1 ) );

            if ( UNLIKELY( l_executablePathLength == -1 ) ) {
                log$transaction$query( ( logLevel_t )error, "readlink\n" );

                free( l_executablePath );

                goto EXIT_FILE_PATH;
            }

            l_executablePath[ l_executablePathLength ] = '\0';
        }

        char* l_directoryPath;

        // Get directory path
        {
            char* l_lastSlash = __builtin_strrchr( l_executablePath, '/' );

            if ( UNLIKELY( !l_lastSlash ) ) {
                log$transaction$query$format( ( logLevel_t )error,
                                              "Extracting directory: '%s'\n",
                                              l_executablePath );

                goto EXIT_FILE_PATH;
            }

            const ssize_t l_lastSlashIndex = ( l_lastSlash - l_executablePath );

            l_directoryPath = l_executablePath;

            // Do not move the beginning
            trim( &l_directoryPath, -1, l_lastSlashIndex );

            concatBeforeAndAfterString( &l_directoryPath, NULL, "/" );
        }

        l_returnValue = l_directoryPath;

    EXIT_FILE_PATH:
    }

    return ( l_returnValue );
}

#if 0
enum SETTINGS_ITEM_TYPE { KEY, VALUE };

void freeSettingsContent( char*** _content ) {
#pragma omp simd
    FOR_ARRAY( char***, _content ) {
        free( ( *_element )[ 0 ] );

        free( ( *_element )[ 1 ] );

        free( ( *_element ) );
    }

    free( _content );
}

static FORCE_INLINE ssize_t findInSettings( char** const* _settings,
                                      const char* _string,
                                      const enum SETTINGS_ITEM_TYPE _type ) {
    ssize_t l_index = -1;

    FOR_ARRAY( char** const*, _settings ) {
        const char* l_string = ( *_element )[ _type ];

        if ( strcmp( l_string, _string ) == 0 ) {
            l_index = ( _element - arrayFirstElementPointer( _settings ) + 1 );

            break;
        }
    }

    return ( l_index );
}

FORCE_INLINE ssize_t findKeyInSettings( char*** _settings, const char* _key ) {
    return ( findInSettings( _settings, _key, KEY ) );
}

FORCE_INLINE ssize_t findValueInSettings( char*** _settings, const char* _value ) {
    return ( findInSettings( _settings, _value, VALUE ) );
}

FORCE_INLINE bool containsKeyInSettings( char*** _settings, const char* _value ) {
    return ( findKeyInSettings( _settings, _value ) >= 0 );
}

char*** getLabelFromSettingsOrDefault( const char* _label,
                                       const char* _default ) {
    char*** l_returnValue = NULL;

    if ( LIKELY(_useCallback( "core$getSettingsContentByLabel", &l_returnValue,
                       _label ) != 0 )) {
        _useCallback( "core$readSettingsFromString", _default );

        _useCallback( "core$getSettingsContentByLabel", &l_returnValue,
                      _label );
    }

    return ( l_returnValue );
}

char* getKeyFromSettingsOrDefault( const char* _label,
                                   const char* _key,
                                   const char* _default ) {
    char* l_returnValue = NULL;
    char*** l_settings;

    if ( LIKELY(_useCallback( "core$getSettingsContentByLabel", &l_settings,
                       _label ) == 0 )) {
        const ssize_t l_settingIndex = findKeyInSettings( l_settings, _key );

        // Has such key
        if ( l_settingIndex >= 0 ) {
            l_returnValue = duplicateString( l_settings[ l_settingIndex ][ 1 ] );

            freeSettingsContent( l_settings );

        } else {
            // Add key
            _useCallback( "core$changeSettingsKeyByLabel", _key, _label,
                          _default );

            l_returnValue = duplicateString( _default );
        }
    }

    return ( l_returnValue );
}
#endif
