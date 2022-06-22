/* date = March 17th 2022 10:38 pm */

#ifndef FONT_PROVIDER_H
#define FONT_PROVIDER_H

////////////////////////////////
//~ rjf: Types

typedef struct FP_TextAtom FP_TextAtom;
struct FP_TextAtom
{
    // TODO(rjf): support for strings of many codepoints
    String32 str;
    Rng1U64 cp_range;
};

typedef struct FP_TextAtomArray FP_TextAtomArray;
struct FP_TextAtomArray
{
    FP_TextAtom *v;
    U64 count;
};

typedef struct FP_BakedGlyph FP_BakedGlyph;
struct FP_BakedGlyph
{
    Rng2F32 src;
    Vec2F32 off;
    Vec2F32 size;
    F32 advance;
};

typedef struct FP_BakedGlyphArray FP_BakedGlyphArray;
struct FP_BakedGlyphArray
{
    FP_BakedGlyph *v;
    U64 count;
};

typedef struct FP_BakedFontInfo FP_BakedFontInfo;
struct FP_BakedFontInfo
{
    Vec2S64 atlas_dim;
    void *atlas_data;
    U64 glyph_array_count;
    FP_BakedGlyphArray *glyph_arrays;
    F32 line_advance;
};

typedef FP_BakedFontInfo FP_BakedFontInfoFromPathFunction(M_Arena *arena, FP_TextAtomArray atoms, F32 size, F32 dpi, String8 path);

typedef struct FP_Backend FP_Backend;
struct FP_Backend
{
    OS_Handle library;
    FP_BakedFontInfoFromPathFunction *BakedFontInfoFromPath;
};

////////////////////////////////
//~ rjf: Helpers

engine_function FP_Backend FP_BackendLoad(String8 path);
engine_function void FP_BackendUnload(FP_Backend backend);
engine_function R_Font FP_FontFromBakedFontInfo(R_Backend backend, R_Handle os_equip, FP_TextAtomArray atoms, FP_BakedFontInfo info);

#endif // FONT_PROVIDER_H
