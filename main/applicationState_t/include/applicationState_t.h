#pragma once

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <stdbool.h>

#include "settings_t.h"

#define DEFAULT_APPLICATION_STATE { NULL, DEFAULT_SETTINGS, 0, 60 }

typedef struct {
    GLFWwindow* window;
    settings_t settings;
    int version;
    size_t totalFramesRendered;
} applicationState_t;

applicationState_t applicationState_t$create( void );
bool applicationState_t$destroy( applicationState_t* _applicationState );
