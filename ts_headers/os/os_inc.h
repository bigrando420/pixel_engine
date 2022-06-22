#ifndef OS_INC_H
#define OS_INC_H

#include "os_main.h"
#if defined(OS_FEATURE_GFX)
#include "os_gfx.h"
#endif

#if OS_WINDOWS
#include "win32/win32_main.h"
#if defined(OS_FEATURE_GFX)
#include "win32/win32_gfx.h"
#endif
#else
#error OS layer not implemented.
#endif

#endif // OS_INC_H
