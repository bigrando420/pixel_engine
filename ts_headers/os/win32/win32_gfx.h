#ifndef WIN32_GFX_H
#define WIN32_GFX_H

////////////////////////////////
//~ rjf: Types

typedef struct W32_Window W32_Window;
struct W32_Window
{
    W32_Window *next;
    W32_Window *prev;
    HWND hwnd;
    HDC hdc;
    B32 last_window_placement_initialized;
    WINDOWPLACEMENT last_window_placement;
};

typedef UINT W32_GetDpiForWindowType(HWND hwnd);

////////////////////////////////
//~ rjf: Globals

engine_global W32_GetDpiForWindowType* w32_GetDpiForWindow;
engine_global W32_Window *w32_first_window;
engine_global W32_Window *w32_last_window;
engine_global W32_Window *w32_free_window;
engine_global OS_CursorKind w32_cursor_kind;
engine_global HWND w32_global_hwnd;
engine_global HDC w32_global_hdc;
engine_global M_Arena *w32_events_arena;
engine_global OS_EventList *w32_events_list;
engine_global WNDCLASSW w32_window_class;
engine_global F32 w32_refresh_rate;
engine_global OS_RepaintFunction *w32_repaint;

////////////////////////////////
//~ rjf: Helpers

engine_function OS_Handle W32_HandleFromWindow(W32_Window *window);
engine_function W32_Window *W32_WindowFromHandle(OS_Handle handle);
engine_function W32_Window *W32_WindowOpen(String8 title, Vec2S64 size);
engine_function void W32_WindowClose(W32_Window *window);
engine_function OS_Modifiers W32_GetModifiers(void);

////////////////////////////////
//~ rjf: Window Proc

function LRESULT W32_WindowProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);

#endif // WIN32_GFX_H
