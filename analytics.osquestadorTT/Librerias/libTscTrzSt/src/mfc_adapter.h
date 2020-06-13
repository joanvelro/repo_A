
#pragma once

#include "tchar.h"

void trace_ex(const char * format, ...);

#ifndef TRACE
   #define TRACE trace_ex
#endif
