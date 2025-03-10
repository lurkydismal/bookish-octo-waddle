#include <glad/gl.h>
#include <stdio.h>

#include "callbacks.h"

callbackResult_t init( applicationState_t* _applicationState ) {
    callbackResult_t l_returnValue = ( callbackResult_t )failure;

    if ( !glfwInit() ) {
        l_returnValue = ( callbackResult_t )failure;

        goto EXIT;
    }

    _applicationState->window =
        glfwCreateWindow( 640, 480, "Hello World", NULL, NULL );

    glfwMakeContextCurrent( _applicationState->window );

    _applicationState->version = gladLoadGL( glfwGetProcAddress );
    printf( "GL %d.%d\n", GLAD_VERSION_MAJOR( _applicationState->version ),
            GLAD_VERSION_MINOR( _applicationState->version ) );

    glfwSwapInterval( 1 );

    l_returnValue = ( callbackResult_t )remain;

EXIT:
    return ( l_returnValue );
}
