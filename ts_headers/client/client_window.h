#ifndef CLIENT_WINDOW_H
#define CLIENT_WINDOW_H

////////////////////////////////
//~ rjf: Basic Types

typedef struct CW_KeyBinding CW_KeyBinding;
struct CW_KeyBinding
{
    OS_Key key;
    OS_Modifiers modifiers;
};

////////////////////////////////
//~ rjf: Main State

typedef struct CW_State CW_State;
struct CW_State
{
    // rjf: permanent resources
    M_Arena *arena;
    UI_State *ui_state;
    
    // rjf: panels
    C_Panel *panel_root;
    C_Panel *focused_panel;
    C_Panel *next_focused_panel;
    
    // rjf: command batch state
    C_CmdRegister cmd_reg;
    M_Arena *cmd_arena;
    C_CmdList cmd_batch;
    
    // rjf: ui data
    TM_Theme theme;
    TM_Theme target_theme;
    F32 drag_boundary_min_pct_start;
    F32 drag_boundary_max_pct_start;
};

////////////////////////////////
//~ rjf: View Hook Types

#define CW_VIEW_FUNCTION_SIG(name) void name(B32 keyboard_focused, CW_State *state, TE_InputActionList text_input_actions, C_Panel *panel, C_View *view, String8 cmd_str, CMD_Node *cmd_node, UI_Box *parent)
#define CW_VIEW_FUNCTION_NAME(name) CW_DoView_##name
#define CW_VIEW_FUNCTION_DEF(name) function CW_VIEW_FUNCTION_SIG(CW_VIEW_FUNCTION_NAME(name))
typedef CW_VIEW_FUNCTION_SIG(CW_ViewFunction);

////////////////////////////////
//~ rjf: Generated Code

#include "client/generated/client_window.meta.h"

////////////////////////////////
//~ rjf: Keybindings

typedef struct CW_Keybind CW_Keybind;
struct CW_Keybind
{
    CW_CmdKind cmd;
    OS_Key key;
    OS_Modifiers modifiers;
};

typedef struct CW_KeybindNode CW_KeybindNode;
struct CW_KeybindNode
{
    CW_KeybindNode *hash_next;
    CW_Keybind keybind;
};

typedef struct CW_KeybindTable CW_KeybindTable;
struct CW_KeybindTable
{
    U64 table_size;
    CW_KeybindNode **table;
};

////////////////////////////////
//~ rjf: Commands

function CW_CmdKind CW_CmdKindFromString(String8 string);
function void CW_DoCmdFastPath(CW_State *state, C_Panel *panel, CW_CmdKind kind);
function void CW_PushCmd(CW_State *state, String8 string);
function void CW_PushCmdF(CW_State *state, char *fmt, ...);
function C_View *CW_ViewFromPanel(C_Panel *panel);

////////////////////////////////
//~ rjf: Keybindings

function U64 CW_HashFromKeyAndModifiers(OS_Key key, OS_Modifiers modifiers);
function CW_Keybind CW_KeybindMake(CW_CmdKind cmd, OS_Key key, OS_Modifiers modifiers);
function CW_KeybindTable CW_KeybindTableMake(M_Arena *arena, U64 table_size);
function void CW_KeybindTableInsert(M_Arena *arena, CW_KeybindTable *table, CW_Keybind keybind);
function CW_CmdKind CW_KeybindTableLookup(CW_KeybindTable *table, OS_Key key, OS_Modifiers modifiers);

////////////////////////////////
//~ rjf: Custom UI Widgets

function UI_Box *CW_TabBegin(String8 string);
function UI_Box *CW_TabBeginF(char *fmt, ...);
function UI_InteractResult CW_TabEnd(void);
function UI_InteractResult CW_LineEdit(B32 keyboard_focused, TE_Point *cursor, TE_Point *mark, U64 string_max, String8 *out_string, String8 string);
function UI_InteractResult CW_LineEditF(B32 keyboard_focused, TE_Point *cursor, TE_Point *mark, U64 string_max, String8 *out_string, char *fmt, ...);

////////////////////////////////
//~ rjf: Drag/Drop

function B32 CW_ViewDragIsActive(void);
function C_View *CW_DraggedView(void);
function void CW_ViewDragBegin(C_Panel *panel, C_View *view);
function void CW_ViewDragEnd(C_Panel *drop_panel, C_View *prev_view);

////////////////////////////////
//~ rjf: Hooks

function APP_WindowUserHooks CW_WindowUserHooks(void);
function void *CW_Open(APP_Window *window);
function void CW_Close(APP_Window *window, void *data);
function void CW_Update(APP_Window *window, OS_EventList *events, void *data);

#endif // CLIENT_WINDOW_H
