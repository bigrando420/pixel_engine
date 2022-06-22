#ifndef DRAW_H
#define DRAW_H

typedef struct DR_Bucket DR_Bucket;
struct DR_Bucket
{
    R_CmdList cmds;
};

typedef struct DR_State DR_State;
struct DR_State
{
    M_Arena *arena;
    M_Arena *frame_cmd_arena;
    M_Arena *frame_data_arena;
    R_Backend backend;
    R_Handle os_eqp;
    R_Handle white_texture;
};

engine_global DR_State *dr_state;

engine_function void DR_Init(R_Backend backend, R_Handle os_eqp);
engine_function void DR_BeginFrame(void);
engine_function void DR_EndFrame(void);
engine_function R_Cmd *DR_PushCmd_KCA(DR_Bucket *bucket, R_CmdKind kind, String8 cpu_data, R_Handle albedo_texture);
engine_function void DR_Rect_VCB(DR_Bucket *bucket, Rng2F32 rect, Vec4F32 color00, Vec4F32 color01, Vec4F32 color10, Vec4F32 color11, F32 corner_radius, F32 border_thickness);
engine_function void DR_Rect_CB(DR_Bucket *bucket, Rng2F32 rect, Vec4F32 color, F32 corner_radius, F32 border_thickness);
engine_function void DR_Rect_C(DR_Bucket *bucket, Rng2F32 rect, Vec4F32 color, F32 corner_radius);
engine_function void DR_Rect_B(DR_Bucket *bucket, Rng2F32 rect, Vec4F32 color, F32 border_thickness);
engine_function void DR_Rect(DR_Bucket *bucket, Rng2F32 rect, Vec4F32 color);
engine_function void DR_Sprite(DR_Bucket *bucket, Vec4F32 color, Rng2F32 rect, Rng2F32 src_rect, R_Handle texture);
engine_function void DR_Text(DR_Bucket *bucket, Vec4F32 color, Vec2F32 p, R_Font font, String8 text);
engine_function void DR_TextF(DR_Bucket *bucket, Vec4F32 color, Vec2F32 p, R_Font font, char *fmt, ...);
engine_function void DR_CmdList(DR_Bucket *bucket, R_CmdList cmds);
engine_function void DR_Submit(R_Handle window_eqp, R_CmdList cmds);

#endif // DRAW_H
