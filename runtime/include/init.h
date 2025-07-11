#pragma once

#include <stdbool.h>

#include "applicationState_t.h"
#include "stdfunc.h"

EXPORT bool init( applicationState_t* restrict _applicationState,
           int _argumentCount,
           char** _argumentVector );
