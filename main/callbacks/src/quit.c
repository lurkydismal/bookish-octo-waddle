#include "asset_t.h"
#include "callbacks.h"
#include "log.h"

callbackResult_t quit( applicationState_t* _applicationState,
                       callbackResult_t _exitCode ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    // GLFW
    {
        glfwTerminate();
    }

    // Assets loader
    {
        asset_t$loader$quit();
    }

    // Log
    {
        log$quit();
    }

    l_returnValue = ( callbackResult_t )_exitCode;

    return ( l_returnValue );
}
