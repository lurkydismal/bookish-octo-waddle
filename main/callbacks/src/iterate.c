#include <glad/gl.h>

#include "callbacks.h"
#include "image_t.h"
#include "log.h"
#include "stdfunc.h"

callbackResult_t iterate$unlimited( applicationState_t* _applicationState ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    static bool x = false;
    static image_t t;

    if ( !x ) {
        x = true;

        t = image_t$create();

        if ( !image_t$load$fromPath( &t, "t.jxl" ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "image load from path\n" );
        }

        log$transaction$query( ( logLevel_t )info, "image load from path\n" );

        if ( !image_t$unload( &t ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "image load from path\n" );
        }

        log$transaction$query$format( ( logLevel_t )info, "image: s %u\n",
                                      t.size );

        FOR_RANGE( size_t, 0, 16 ) {
            log$transaction$query$format( ( logLevel_t )info, "d %p\n",
                                          t.data[ _index ] );
        }
    }

    {
        int width, height;
        glfwGetFramebufferSize( _applicationState->window, &width, &height );
        glViewport( 0, 0, width, height );

        // Render
        {
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

            if ( x ) {
                GLuint textureID;
                glGenTextures( 1, &textureID );
                glBindTexture( GL_TEXTURE_2D, textureID );

                glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, t.width, t.height, 0,
                              GL_RGBA, GL_UNSIGNED_BYTE, t.data );

                // Set texture parameters (wrap, filter)
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                 GL_LINEAR );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                                 GL_LINEAR );
            }

            glfwSwapBuffers( _applicationState->window );
        }
    }

    _applicationState->totalFramesRendered++;

    l_returnValue = ( callbackResult_t )remain;

    return ( l_returnValue );
}

callbackResult_t iterate$limited( applicationState_t* _applicationState ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    const bool l_logTransactionResult = log$transaction$commit();

    if ( UNLIKELY( !l_logTransactionResult ) ) {
        l_returnValue = ( callbackResult_t )failure;

    } else {
        l_returnValue = ( callbackResult_t )remain;
    }

    return ( l_returnValue );
}
