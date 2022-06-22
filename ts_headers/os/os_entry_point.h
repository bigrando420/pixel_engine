#ifndef OS_ENTRY_POINT_H
#define OS_ENTRY_POINT_H

#if OS_WINDOWS
#include "win32/win32_entry_point.h"
#else
#error Entry point not defined for OS.
#endif

#endif //OS_ENTRY_POINT_H
