#pragma once

#include <stdbool.h>

#include "applicationState_t.h"

bool init( applicationState_t* restrict _applicationState,
           int _argumentCount,
           char** _argumentVector );
