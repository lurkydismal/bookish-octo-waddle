#pragma once

#define VSYNC_TYPE_AS_STRING_OFF "OFF"
#define VSYNC_TYPE_AS_STRING_UNKNOWN "UNKNOWN"

#define DEFAULT_VSYNC ( ( vsync_t )off )
#define VSYNC_LEVEL_DEFAULT DEFAULT_VSYNC

typedef enum {
    off = 0,
    unknownVsync,
} vsync_t;
