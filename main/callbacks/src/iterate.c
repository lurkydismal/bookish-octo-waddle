#include <glad/gl.h>

#include "callbacks.h"
#include "log.h"
#include "stdfunc.h"

callbackResult_t iterate$unlimited( applicationState_t* _applicationState ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    {
        int width, height;
        glfwGetFramebufferSize( _applicationState->window, &width, &height );
        glViewport( 0, 0, width, height );

        // Render
        {
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

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
