#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "stdfunc.h"

#define VSYNC_TYPE_AS_STRING_OFF "OFF"
#define VSYNC_TYPE_AS_STRING_NORMAL "NORMAL"
#define VSYNC_TYPE_AS_STRING_ADAPTIVE "ADAPTIVE"
#define VSYNC_TYPE_AS_STRING_UNKNOWN "UNKNOWN"

#define DEFAULT_VSYNC ( ( vsync_t )off )
#define VSYNC_LEVEL_DEFAULT DEFAULT_VSYNC

typedef enum { off = 0, normal, adaptive, unknownVsync } vsync_t;

vsync_t vsync_t$fromString( const char* _string );

bool vsync$init( const vsync_t _vsync, const size_t _desiredFPS );
bool vsync$quit( void );

bool vsync$begin( void );
bool vsync$end( void );
