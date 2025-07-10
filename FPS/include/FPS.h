#pragma once

#include <stdbool.h>
#include <stddef.h>

bool FPS$init( size_t* restrict _totalFramesPassed );
bool FPS$quit( void );

size_t FPS$get$current( void );
size_t FPS$get$total( void );
