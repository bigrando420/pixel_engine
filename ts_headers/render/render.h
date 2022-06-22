#ifndef RENDER_TYPES_H
#define RENDER_TYPES_H

typedef enum R_Texture2DFormat
{
    R_Texture2DFormat_Null,
    R_Texture2DFormat_R8,
    R_Texture2DFormat_RGBA8,
    R_Texture2DFormat_COUNT
}
R_Texture2DFormat;

typedef union R_Handle R_Handle;
union R_Handle
{
    U64 u64[4];
    U32 u32[8];
};

typedef struct R_Glyph R_Glyph;
struct R_Glyph
{
    Rng2F32 src;
    Vec2F32 offset;
    Vec2F32 size;
    F32 advance;
};

typedef struct R_Font R_Font;
struct R_Font
{
    M_Arena *arena;
    U32 direct_map_first;
    U32 direct_map_opl;
    R_Glyph *direct_map;
    // TODO(rjf): indirect map
    F32 line_advance;
    R_Handle texture;
};

typedef enum R_CmdKind
{
    R_CmdKind_Null,
    R_CmdKind_Rects,
    R_CmdKind_Triangles,
    R_CmdKind_SetClip,
    R_CmdKind_Scratch,
    R_CmdKind_COUNT
}
R_CmdKind;

typedef struct R_Cmd R_Cmd;
struct R_Cmd
{
    R_CmdKind kind;
    R_Handle gpu_data;
    String8 cpu_data;
    R_Handle albedo_texture;
    R_Handle sdf_mask_texture;
};

typedef struct R_CmdNode R_CmdNode;
struct R_CmdNode
{
    R_CmdNode *next;
    R_Cmd cmd;
};

typedef struct R_CmdList R_CmdList;
struct R_CmdList
{
    R_CmdNode *first;
    R_CmdNode *last;
    U64 count;
};

typedef R_Handle R_EquipOSFunction(void);
typedef R_Handle R_EquipWindowFunction(R_Handle os_eqp, OS_Handle window);
typedef void R_UnequipWindowFunction(R_Handle os_eqp, R_Handle window_eqp);
typedef R_Handle R_ReserveTexture2DFunction(R_Handle os_eqp, Vec2S64 size, R_Texture2DFormat fmt);
typedef void R_ReleaseTexture2DFunction(R_Handle os_eqp, R_Handle texture);
typedef void R_FillTexture2DFunction(R_Handle os_eqp, R_Handle texture, Rng2S64 subrect, String8 data);
typedef Vec2F32 R_SizeFromTexture2DFunction(R_Handle texture);
typedef void R_StartFunction(R_Handle os_eqp, R_Handle window_eqp, Vec2S64 resolution);
typedef void R_SubmitFunction(R_Handle os_eqp, R_Handle window_eqp, R_CmdList commands);
typedef void R_FinishFunction(R_Handle os_eqp, R_Handle window_eqp);

typedef struct R_Backend R_Backend;
struct R_Backend
{
    OS_Handle library;
    R_EquipOSFunction *EquipOS;
    R_EquipWindowFunction *EquipWindow;
    R_UnequipWindowFunction *UnequipWindow;
    R_ReserveTexture2DFunction *ReserveTexture2D;
    R_ReleaseTexture2DFunction *ReleaseTexture2D;
    R_FillTexture2DFunction *FillTexture2D;
    R_SizeFromTexture2DFunction *SizeFromTexture2D;
    R_StartFunction *Start;
    R_SubmitFunction *Submit;
    R_FinishFunction *Finish;
};

////////////////////////////////
//~ rjf: Constants

#define R_Rect_FloatsPerInstance (32)
#define R_Triangle_FloatsPerInstance (18)

////////////////////////////////
//~ rjf: Helpers

// TODO(rjf): sort

engine_function R_Glyph R_GlyphFromFontCodepoint(R_Font font, U32 codepoint);
engine_function Vec2F32 R_AdvanceFromString(R_Font font, String8 string);
engine_function String8 R_PushRectData(M_Arena *arena, Rng2F32 dst, Rng2F32 src, Vec4F32 color00, Vec4F32 color01, Vec4F32 color10, Vec4F32 color11, F32 corner_radius, F32 border_thickness);
engine_function String8 R_PushTriangleData(M_Arena *arena, Vec2F32 p0, Vec4F32 c0, Vec2F32 p1, Vec4F32 c1, Vec2F32 p2, Vec4F32 c2);
engine_function String8 R_PushSetClipData(M_Arena *arena, Rng2F32 clip);
engine_function void R_CmdCopy(R_Cmd *dst, R_Cmd *src);
engine_function R_CmdNode *R_CmdListPush(M_Arena *arena, R_CmdList *list, R_Cmd *cmd);
engine_function void R_CmdListJoin(R_CmdList *list, R_CmdList *to_push);
engine_function U64 R_BytesPerPixelFromTexture2DFormat(R_Texture2DFormat fmt);
engine_function R_Backend R_BackendLoad(String8 path);
engine_function void R_BackendUnload(R_Backend backend);
engine_function R_Handle R_HandleZero(void);
engine_function B32 R_HandleMatch(R_Handle a, R_Handle b);
engine_function B32 R_HandleIsNull(R_Handle handle);
engine_function R_Handle R_LoadTexture(R_Backend backend, R_Handle os_equip, String8 path);
#if 0
engine_function R_Font R_LoadFont(R_Backend backend, R_Handle os_equip, F32 size, String8 path);
#endif

#endif // RENDER_TYPES_H
