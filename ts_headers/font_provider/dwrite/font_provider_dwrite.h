#ifndef FONT_PROVIDER_DWRITE_H
#define FONT_PROVIDER_DWRITE_H

#include <windows.h>
#include <dwrite_1.h>

exported FP_BakedFontInfo BakedFontInfoFromPath(M_Arena *arena, FP_TextAtomArray atoms, F32 size, F32 dpi, String8 path);

#endif // FONT_PROVIDER_DWRITE_H
