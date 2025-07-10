#include <stdio.h>

#include "stdfunc.h"

__attribute__( ( constructor ) ) static void hotReload$load( void ) {
    printf( ">>> " ASCII_COLOR_GREEN "Loaded" ASCII_COLOR_RESET "\n" );
}

__attribute__( ( destructor ) ) static void hotReload$unload( void ) {
    printf( "<<< " ASCII_COLOR_RED "Unloaded" ASCII_COLOR_RESET "\n" );
}
