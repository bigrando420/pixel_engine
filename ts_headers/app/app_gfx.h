#ifndef APP_GFX_H
#define APP_GFX_H

////////////////////////////////
//~ rjf: Window User Hooks

typedef void *APP_WindowUserOpenFunction(struct APP_Window *window);
typedef void APP_WindowUserCloseFunction(struct APP_Window *window, void *data);
typedef void APP_WindowUserUpdateFunction(struct APP_Window *window, OS_EventList *events, void *data);

typedef struct APP_WindowUserHooks APP_WindowUserHooks;
struct APP_WindowUserHooks
{
    APP_WindowUserOpenFunction *Open;
    APP_WindowUserCloseFunction *Close;
    APP_WindowUserUpdateFunction *Update;
};

typedef struct APP_WindowUser APP_WindowUser;
struct APP_WindowUser
{
    void *data;
    APP_WindowUserHooks hooks;
};

////////////////////////////////
//~ rjf: Window Types

typedef struct APP_Window APP_Window;
struct APP_Window
{
    APP_Window *next;
    APP_Window *prev;
    OS_Handle handle;
    R_Handle window_equip;
    B32 initialized;
    APP_WindowUser user;
};

////////////////////////////////
//~ rjf: Views

typedef void *APP_ViewOpenFunction(void);
typedef void APP_ViewCloseFunction(void *data);
typedef void APP_ViewUpdateFunction(OS_EventList *events, Rng2F32 rect, void *data);

typedef struct APP_View APP_View;
struct APP_View
{
    void *state;
    APP_ViewOpenFunction *Open;
    APP_ViewCloseFunction *Close;
    APP_ViewUpdateFunction *Update;
};

////////////////////////////////
//~ rjf: Main State

typedef struct APP_State APP_State;
struct APP_State
{
    M_Arena *permanent_arena;
    M_Arena *frame_arenas[2];
    U64 frame_index;
    F32 target_fps;
    R_Backend r_backend;
    R_Handle r_os_equip;
    FP_Backend fp_backend;
    APP_Window *first_window;
    APP_Window *last_window;
    APP_Window *free_window;
    R_Font default_font;
    R_Font icons_font;
    OS_CursorKind cursor_kind;
    OS_CursorKind cursor_kind__last_frame;
};

////////////////////////////////
//~ rjf: Globals

engine_global APP_State *app_state;
engine_global APP_View app_g_nil_view;

////////////////////////////////
//~ rjf: Main API

engine_function void APP_Init(void);
engine_function B32 APP_Quit(void);
engine_function void APP_BeginFrame(void);
engine_function void APP_EndFrame(void);

////////////////////////////////
//~ rjf: Renderer Wrapper

engine_function R_Handle APP_LoadTexture(String8 path);
engine_function R_Font APP_LoadFont(F32 size, String8 path);
engine_function Vec2F32 APP_SizeFromTexture(R_Handle handle);

////////////////////////////////
//~ rjf: Views

engine_function APP_View *APP_NilView(void);
engine_function B32 APP_ViewIsNil(APP_View *view);
engine_function void *APP_ViewOpenStub(void);
engine_function void APP_ViewCloseStub(void *data);
engine_function void APP_ViewUpdateStub(OS_EventList *events, Rng2F32 rect, void *data);

////////////////////////////////
//~ rjf: Frame Info

engine_function U64 APP_FrameIndex(void);
engine_function F32 APP_DT(void);
engine_function void APP_Cursor(OS_CursorKind cursor);

////////////////////////////////
//~ rjf: Animation Helpers

engine_function void APP_AnimateF32_Exp(F32 *x, F32 target, F32 rate);

////////////////////////////////
//~ rjf: Resources

engine_function R_Font APP_DefaultFont(void);
engine_function R_Font APP_IconsFont(void);

////////////////////////////////
//~ rjf: Arenas

engine_function M_Arena *APP_PermanentArena(void);
engine_function M_Arena *APP_FrameArena(void);

////////////////////////////////
//~ rjf: Windows

engine_function APP_Window *APP_WindowOpen(String8 title, Vec2S64 size, APP_WindowUserHooks hooks);
engine_function void APP_WindowClose(APP_Window *window);
engine_function APP_Window *APP_WindowFromHandle(OS_Handle handle);

#endif // APP_GFX_H
