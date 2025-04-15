#include <cglm/cglm.h>
#include <glad/gl.h>

#include "callbacks.h"
#include "font_t.h"
#include "log.h"
#include "stdfunc.h"

callbackResult_t iterate$unlimited(
    applicationState_t* restrict _applicationState ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    static bool x = false;
    static font_t l_font;

    if ( !x ) {
        x = true;

        {
            l_font = font_t$create();

            l_font.color[ 0 ] = 1;
            l_font.color[ 1 ] = 0;
            l_font.color[ 2 ] = 1;
            l_font.height = 20;

            if ( UNLIKELY( !font_t$load$fromPath( &l_font, "t.ttf" ) ) ) {
                log$transaction$query( ( logLevel_t )error, "font1\n" );
            }
        }
    }

    // Render
    {
        glClearColor( 1, 1, 1, 1.0f ); // dark gray background

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glfwSwapBuffers( _applicationState->window );
    }

    _applicationState->totalFramesRendered++;

    l_returnValue = ( callbackResult_t )remain;

    return ( l_returnValue );
}

callbackResult_t iterate$limited(
    applicationState_t* restrict _applicationState ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    const bool l_logTransactionResult = log$transaction$commit();

    if ( UNLIKELY( !l_logTransactionResult ) ) {
        l_returnValue = ( callbackResult_t )failure;

    } else {
        l_returnValue = ( callbackResult_t )remain;
    }

    return ( l_returnValue );
}
