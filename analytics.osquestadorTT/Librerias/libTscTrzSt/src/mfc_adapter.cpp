
#include "mfc_adapter.h"
#include <windows.h>

#if defined _DEBUG
#ifdef DEBUG_NEW
#undef DEBUG_NEW
#endif
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void trace_ex(const char* format, ...)
{
   char buffer[1000];

   va_list argptr;
   va_start(argptr, format);
   wvsprintf(buffer, format, argptr);
   va_end(argptr);

   OutputDebugString(buffer);
}