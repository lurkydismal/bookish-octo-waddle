#include "callbacks.h"

callbackResult_t quit( applicationState_t* _applicationState,
                       callbackResult_t _exitCode ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    glfwTerminate();

    return ( l_returnValue );
}
