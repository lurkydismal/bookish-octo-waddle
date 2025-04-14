#include "FPS.h"
#include "asset_t.h"
#include "callbacks.h"
#include "log.h"
#include "vsync.h"

// TODO: Log errors
callbackResult_t quit( applicationState_t* restrict _applicationState,
                       callbackResult_t _exitCode ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    // FPS
    {
        FPS$quit();
    }

    // Vsync
    {
        vsync$quit();
    }

    // GLFW
    {
        glfwTerminate();
    }

    // Application state
    {
        applicationState_t$destroy( _applicationState );
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
