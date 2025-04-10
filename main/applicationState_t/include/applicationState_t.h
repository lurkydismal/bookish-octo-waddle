#pragma once

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <stdbool.h>

#include "settings_t.h"

#define DEFAULT_APPLICATION_STATE   \
    { .window = NULL,               \
      .settings = DEFAULT_SETTINGS, \
      .glVersion = 0,               \
      .totalFramesRendered = 0 }

typedef struct {
    GLFWwindow* window;
    settings_t settings;
    int glVersion;
    size_t totalFramesRendered;
} applicationState_t;

applicationState_t applicationState_t$create( void );
bool applicationState_t$destroy( applicationState_t* _applicationState );
