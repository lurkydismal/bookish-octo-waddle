#pragma once

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

typedef struct {
    GLFWwindow* window;
    int version;
    size_t totalFramesRendered;
} applicationState_t;
