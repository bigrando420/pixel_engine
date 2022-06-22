/* date = February 20th 2022 11:51 am */

#ifndef THEME_H
#define THEME_H

typedef struct TM_ThemeColorNode TM_ThemeColorNode;
struct TM_ThemeColorNode
{
    TM_ThemeColorNode *hash_next;
    String8 string;
    Vec4F32 color;
};

typedef struct TM_Theme TM_Theme;
struct TM_Theme
{
    U64 table_size;
    TM_ThemeColorNode **table;
};

engine_function TM_Theme TM_ThemeMake(M_Arena *arena);
engine_function U64 TM_HashFromString(String8 string);
engine_function void TM_ThemeSetColor(M_Arena *arena, TM_Theme *theme, String8 string, Vec4F32 color);
engine_function TM_ThemeColorNode *TM_ThemeColorNodeFromString(TM_Theme *theme, String8 string);
engine_function Vec4F32 TM_ThemeColorFromString(TM_Theme *theme, String8 string);

#endif // THEME_H
