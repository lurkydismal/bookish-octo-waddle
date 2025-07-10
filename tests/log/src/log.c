#include "log.h"

#include "test.h"

TEST( log$level$convert$toStaticString ) {
    // debug - "DEBUG"
    {
        const char* l_logLevelAsString =
            log$level$convert$toStaticString( ( logLevel_t )debug );

        ASSERT_NOT_EQ( "%p", l_logLevelAsString, NULL );
        ASSERT_STRING_EQ( l_logLevelAsString, LOG_LEVEL_AS_STRING_DEBUG );
    }

    // info - "INFO"
    {
        const char* l_logLevelAsString =
            log$level$convert$toStaticString( ( logLevel_t )info );

        ASSERT_NOT_EQ( "%p", l_logLevelAsString, NULL );
        ASSERT_STRING_EQ( l_logLevelAsString, LOG_LEVEL_AS_STRING_INFO );
    }

    // warn - "WARN"
    {
        const char* l_logLevelAsString =
            log$level$convert$toStaticString( ( logLevel_t )warn );

        ASSERT_NOT_EQ( "%p", l_logLevelAsString, NULL );
        ASSERT_STRING_EQ( l_logLevelAsString, LOG_LEVEL_AS_STRING_WARN );
    }

    // error - "ERROR"
    {
        const char* l_logLevelAsString =
            log$level$convert$toStaticString( ( logLevel_t )error );

        ASSERT_NOT_EQ( "%p", l_logLevelAsString, NULL );
        ASSERT_STRING_EQ( l_logLevelAsString, LOG_LEVEL_AS_STRING_ERROR );
    }

    // out-of-range ( negative ) - "UNKNOWN"
    {
        const char* l_logLevelAsString =
            log$level$convert$toStaticString( ( logLevel_t )( -1 ) );

        ASSERT_NOT_EQ( "%p", l_logLevelAsString, NULL );
        ASSERT_STRING_EQ( l_logLevelAsString, LOG_LEVEL_AS_STRING_UNKNOWN );
    }

    // out-of-range ( arbitrary ) - "UNKNOWN"
    {
        const char* l_logLevelAsString =
            log$level$convert$toStaticString( ( logLevel_t )42 );

        ASSERT_NOT_EQ( "%p", l_logLevelAsString, NULL );
        ASSERT_STRING_EQ( l_logLevelAsString, LOG_LEVEL_AS_STRING_UNKNOWN );
    }
}

TEST( log$level$convert$fromString ) {
    const logLevel_t l_unknown = ( logLevel_t )unknownLogLevel;

    // NULL input - unknownLogLevel
    {
        const logLevel_t l_logLevel = log$level$convert$fromString( NULL );

        ASSERT_EQ( "%d", l_logLevel, l_unknown );
    }

    // "DEBUG" - debug
    {
        const logLevel_t l_logLevel =
            log$level$convert$fromString( LOG_LEVEL_AS_STRING_DEBUG );

        ASSERT_EQ( "%d", l_logLevel, ( logLevel_t )debug );
    }

    // "INFO" - info
    {
        const logLevel_t l_logLevel =
            log$level$convert$fromString( LOG_LEVEL_AS_STRING_INFO );

        ASSERT_EQ( "%d", l_logLevel, ( logLevel_t )info );
    }

    // "WARN" - warn
    {
        const logLevel_t l_logLevel =
            log$level$convert$fromString( LOG_LEVEL_AS_STRING_WARN );

        ASSERT_EQ( "%d", l_logLevel, ( logLevel_t )warn );
    }

    // "ERROR" - error
    {
        const logLevel_t l_logLevel =
            log$level$convert$fromString( LOG_LEVEL_AS_STRING_ERROR );

        ASSERT_EQ( "%d", l_logLevel, ( logLevel_t )error );
    }

    // Unknown literal - unknownLogLevel
    {
        const logLevel_t l_logLevel = log$level$convert$fromString( "FOOBAR" );

        ASSERT_EQ( "%d", l_logLevel, l_unknown );
    }

    // Case-sensitivity: lowercase "debug" is not recognized - unknownLogLevel
    {
        const logLevel_t l_logLevel = log$level$convert$fromString( "debug" );

        ASSERT_EQ( "%d", l_logLevel, l_unknown );
    }
}

TEST( log$level$set ) {
    // Set to DEBUG
    {
        bool l_returnValue = log$level$set( ( logLevel_t )debug );

        ASSERT_TRUE( l_returnValue );
        ASSERT_EQ( "%d", log$level$get(), ( logLevel_t )debug );
    }

    // Set to ERROR
    {
        bool l_returnValue = log$level$set( ( logLevel_t )error );

        ASSERT_TRUE( l_returnValue );
        ASSERT_EQ( "%d", log$level$get(), ( logLevel_t )error );
    }
}

TEST( log$level$set$string ) {
    // Valid string - returns true, level set to WARN
    {
        bool l_returnValue = log$level$set$string( LOG_LEVEL_AS_STRING_WARN );

        ASSERT_TRUE( l_returnValue );
        ASSERT_EQ( "%d", log$level$get(), ( logLevel_t )warn );
    }

    // Unknown string - returns true, level becomes unknownLogLevel
    {
        bool l_returnValue = log$level$set$string( "FOOBAR" );

        ASSERT_TRUE( l_returnValue );
        ASSERT_EQ( "%d", log$level$get(), ( logLevel_t )unknownLogLevel );
    }

    // NULL string - returns false, level unchanged
    {
        // first set to INFO
        bool l_returnValue = log$level$set$string( LOG_LEVEL_AS_STRING_INFO );

        ASSERT_TRUE( l_returnValue );

        logLevel_t l_before = log$level$get();

        l_returnValue = log$level$set$string( NULL );

        ASSERT_FALSE( l_returnValue );
        ASSERT_EQ( "%d", log$level$get(), l_before );
    }
}

TEST( log$level$get ) {
    // Use log$level$set
    {
        bool l_returnValue = log$level$set( ( logLevel_t )debug );

        ASSERT_TRUE( l_returnValue );
        ASSERT_EQ( "%d", log$level$get(), ( logLevel_t )debug );
    }

    {
        bool l_returnValue = log$level$set( ( logLevel_t )error );

        ASSERT_TRUE( l_returnValue );
        ASSERT_EQ( "%d", log$level$get(), ( logLevel_t )error );
    }
}

TEST( log$level$get$string ) {
    // When level is DEBUG
    {
        bool l_returnValue = log$level$set( ( logLevel_t )debug );

        ASSERT_TRUE( l_returnValue );

        const char* l_logLevelAsString = log$level$get$string();

        ASSERT_NOT_EQ( "%p", l_logLevelAsString, NULL );
        ASSERT_STRING_EQ( l_logLevelAsString, LOG_LEVEL_AS_STRING_DEBUG );
    }

    // When level is unknownLogLevel
    {
        bool l_returnValue = log$level$set( ( logLevel_t )unknownLogLevel );

        ASSERT_TRUE( l_returnValue );

        const char* l_logLevelAsString = log$level$get$string();

        ASSERT_NOT_EQ( "%p", l_logLevelAsString, NULL );
        ASSERT_STRING_EQ( l_logLevelAsString, LOG_LEVEL_AS_STRING_UNKNOWN );
    }
}

TEST( log$quit ) {
    bool l_returnValue = log$quit();

    ASSERT_FALSE( l_returnValue );
}

TEST( log$transaction$query ) {
    bool l_returnValue = log$transaction$query( ( logLevel_t )debug, "TEST" );

    ASSERT_FALSE( l_returnValue );

    l_returnValue = log$transaction$query( ( logLevel_t )info, "TEST" );

    ASSERT_FALSE( l_returnValue );

    l_returnValue = log$transaction$query( ( logLevel_t )warn, "TEST" );

    ASSERT_FALSE( l_returnValue );

    l_returnValue = log$transaction$query( ( logLevel_t )error, "TEST" );

    ASSERT_FALSE( l_returnValue );

    l_returnValue =
        log$transaction$query( ( logLevel_t )unknownLogLevel, "TEST" );

    ASSERT_FALSE( l_returnValue );
}

TEST( log$transaction$query$format ) {
    bool l_returnValue =
        log$transaction$query$format( ( logLevel_t )debug, "%s", "TEST" );

    ASSERT_FALSE( l_returnValue );

    l_returnValue =
        log$transaction$query$format( ( logLevel_t )info, "%s", "TEST" );

    ASSERT_FALSE( l_returnValue );

    l_returnValue =
        log$transaction$query$format( ( logLevel_t )warn, "%s", "TEST" );

    ASSERT_FALSE( l_returnValue );

    l_returnValue =
        log$transaction$query$format( ( logLevel_t )error, "%s", "TEST" );

    ASSERT_FALSE( l_returnValue );

    l_returnValue = log$transaction$query$format( ( logLevel_t )unknownLogLevel,
                                                  "%s", "TEST" );

    ASSERT_FALSE( l_returnValue );
}

TEST( log$transaction$commit ) {
    bool l_returnValue = log$transaction$commit();

    ASSERT_FALSE( l_returnValue );
}
