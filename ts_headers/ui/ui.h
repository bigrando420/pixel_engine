#ifndef UI_H
#define UI_H

////////////////////////////////
//~ rjf: Generated Code

#include "generated/ui.meta.h"

////////////////////////////////
//~ rjf: Keys

typedef struct UI_Key UI_Key;
struct UI_Key
{
    U64 u64[1];
};

////////////////////////////////
//~ rjf: Sizes

typedef enum UI_SizeKind
{
    UI_SizeKind_Null,
    UI_SizeKind_Pixels,      // size is computed via a preferred pixel value
    UI_SizeKind_TextContent, // size is computed via the dimensions of box's rendered string
    UI_SizeKind_ParentPct,   // size is computed via a well-determined parent or grandparent size
    UI_SizeKind_ChildrenSum, // size is computed via summing well-determined sizes of children
}
UI_SizeKind;

typedef struct UI_Size UI_Size;
struct UI_Size
{
    UI_SizeKind kind;
    F32 value;
    F32 strictness;
};

////////////////////////////////
//~ rjf: Box Types

typedef U32 UI_BoxFlags;
enum
{
    //- rjf: interaction
    UI_BoxFlag_Clickable          = (1<<0),
    UI_BoxFlag_ViewScroll         = (1<<1),
    
    //- rjf: layout
    UI_BoxFlag_FloatingX          = (1<<2),
    UI_BoxFlag_FloatingY          = (1<<3),
    UI_BoxFlag_FixedWidth         = (1<<4),
    UI_BoxFlag_FixedHeight        = (1<<5),
    UI_BoxFlag_AllowOverflowX     = (1<<6),
    UI_BoxFlag_AllowOverflowY     = (1<<7),
    
    //- rjf: appearance / animation
    UI_BoxFlag_DrawBackground     = (1<<8),
    UI_BoxFlag_DrawBorder         = (1<<9),
    UI_BoxFlag_DrawText           = (1<<10),
    UI_BoxFlag_DrawFancyText      = (1<<11),
    UI_BoxFlag_DrawTexture        = (1<<12),
    UI_BoxFlag_DrawHotEffects     = (1<<13),
    UI_BoxFlag_DrawActiveEffects  = (1<<14),
    UI_BoxFlag_DrawBucket         = (1<<15),
    UI_BoxFlag_DrawBucketRelative = (1<<16),
    UI_BoxFlag_TextAlignCenter    = (1<<17),
    UI_BoxFlag_Clip               = (1<<18),
    UI_BoxFlag_AnimatePosX        = (1<<19),
    UI_BoxFlag_AnimatePosY        = (1<<20),
    UI_BoxFlag_UseFullString      = (1<<21),
};

typedef struct UI_Box UI_Box;
struct UI_Box
{
    // rjf: persistent links
    UI_Box *hash_next;
    UI_Box *hash_prev;
    
    // rjf: per-frame links/data
    UI_Box *first;
    UI_Box *last;
    UI_Box *next;
    UI_Box *prev;
    UI_Box *parent;
    U64 child_count;
    
    // rjf: per-frame equipment
    UI_Key key;
    UI_BoxFlags flags;
    String8 string;
    FancyStringList fancy_strings;
    String8List data;
    Vec2F32 fixed_position;
    Vec2F32 fixed_size;
    UI_Size pref_size[Axis2_COUNT];
    F32 min_size[Axis2_COUNT];
    Axis2 child_layout_axis;
    OS_CursorKind hover_cursor;
    DR_Bucket draw_bucket;
    DR_Bucket draw_bucket_relative;
    Vec4F32 background_color;
    Vec4F32 text_color;
    Vec4F32 border_color;
    R_Font font;
    R_Handle texture;
    Rng2F32 texture_src;
    F32 corner_radius;
    Vec2F32 *view_off_storage;
    
    // rjf: data that is computed per-frame & kept around
    U64 first_touched_frame_index;
    U64 last_touched_frame_index;
    Vec2F32 fixed_position_animated;
    Rng2F32 rect;
    Vec2F32 position_delta;
    F32 hot_t;
    F32 active_t;
    Vec2F32 view_off;
    Vec2F32 target_view_off;
    Vec2F32 view_bounds;
};

typedef struct UI_BoxRec UI_BoxRec;
struct UI_BoxRec
{
    UI_Box *next;
    int push_count;
    int pop_count;
};

typedef struct UI_InteractResult UI_InteractResult;
struct UI_InteractResult
{
    UI_Box *box;
    B32 clicked;
    B32 pressed;
    B32 released;
    B32 dragging;
    B32 hovering;
    Vec2F32 mouse;
    Vec2F32 drag_delta;
};

////////////////////////////////
//~ rjf: Running UI "Machine" State

typedef struct UI_BoxHashSlot UI_BoxHashSlot;
struct UI_BoxHashSlot
{
    UI_Box *hash_first;
    UI_Box *hash_last;
};

typedef struct UI_State UI_State;
struct UI_State
{
    // rjf: permanent state
    M_Arena *arena;
    M_Arena *frame_arenas[2];
    UI_Box *first_free_box;
    
    // rjf: persistent box cache
    U64 box_table_size;
    UI_BoxHashSlot *box_table;
    
    // rjf: persistent across-frame state
    U64 frame_index;
    UI_Key hot_box_key;
    UI_Key active_box_key;
    Vec2F32 drag_start_mouse;
    U64 drag_data[64];
    B32 drag_started;
    
    // rjf: text editing state
    TE_Point cursor;
    TE_Point mark;
    F32 caret_blink_t;
    
    // rjf: per-frame state
    OS_Handle window;
    OS_EventList *events;
    UI_Box *root;
    UI_Box *tooltip_root;
    F32 animation_dt;
    
    // rjf: per-frame stacks (build phase)
    U64 parent_stack_size;
    UI_Box *parent_stack[64];
    UI_Box *parent_stack_active;
    U64 view_off_storage_stack_size;
    Vec2F32 *view_off_storage_stack[64];
    Vec2F32 *view_off_storage_stack_active;
    UI_StackDecls;
};

////////////////////////////////
//~ rjf: Sizes

engine_function UI_Size UI_SizeMake(UI_SizeKind kind, F32 value, F32 strictness);
#define UI_Pixels(value, strictness) UI_SizeMake(UI_SizeKind_Pixels, value, strictness)
#define UI_TextDim(padding, strictness) UI_SizeMake(UI_SizeKind_TextContent, padding, strictness)
#define UI_Pct(value, strictness) UI_SizeMake(UI_SizeKind_ParentPct, value, strictness)
#define UI_ChildrenSum(strictness) UI_SizeMake(UI_SizeKind_ChildrenSum, 0.f, strictness)

////////////////////////////////
//~ rjf: Keys

engine_function U64 UI_HashFromString(U64 seed, String8 string);
engine_function UI_Key UI_KeyNull(void);
engine_function String8 UI_HashPartFromBoxString(String8 string);
engine_function String8 UI_DisplayStringFromBox(UI_Box *box);
engine_function UI_Key UI_KeyFromString(UI_Key seed_key, String8 string);
engine_function B32 UI_KeyMatch(UI_Key a, UI_Key b);

////////////////////////////////
//~ rjf: Boxes

engine_global UI_Box ui_g_nil_box;
#define UI_BoxSetNil(b) ((b) = &ui_g_nil_box)
engine_function B32 UI_BoxIsNil(UI_Box *box);
engine_function UI_BoxRec UI_BoxRecDF(UI_Box *box, U64 sib_member_off, U64 child_member_off);
engine_function UI_BoxRec UI_BoxRecDF_Pre(UI_Box *box);
engine_function UI_BoxRec UI_BoxRecDF_Post(UI_Box *box);

////////////////////////////////
//~ rjf: Global (Not Per-State) Queries

engine_function B32 UI_DragIsActive(void);

////////////////////////////////
//~ rjf: State Building / Selecting

engine_function UI_State *UI_StateAlloc(void);
engine_function void      UI_StateRelease(UI_State *state);
engine_function UI_State *UI_GetSelectedState(void);
engine_function UI_Box *  UI_RootFromState(UI_State *state);

////////////////////////////////
//~ rjf: Implicit State Accessors/Mutators

engine_function M_Arena *         UI_FrameArena(void);
engine_function OS_Handle         UI_Window(void);
engine_function OS_EventList *    UI_Events(void);
engine_function F32               UI_CaretBlinkT(void);
engine_function void              UI_ResetCaretBlinkT(void);
engine_function void              UI_StoreDragData_Vec2F32(Vec2F32 v);
engine_function Vec2F32           UI_GetDragData_Vec2F32(void);
engine_function UI_Key            UI_HotKey(void);

////////////////////////////////
//~ rjf: Top-Level Building API

engine_function void              UI_BeginBuild(UI_State *state, OS_EventList *events, OS_Handle window, F32 animation_dt);
engine_function void              UI_EndBuild(void);
engine_function void              UI_LayoutStandalone_InPlaceRecursive(UI_Box *root, Axis2 axis);
engine_function void              UI_LayoutUpwardsDependent_InPlaceRecursive(UI_Box *root, Axis2 axis);
engine_function void              UI_LayoutDownwardsDependent_InPlaceRecursive(UI_Box *root, Axis2 axis);
engine_function void              UI_LayoutEnforceConstraints_InPlaceRecursive(UI_Box *root, Axis2 axis);
engine_function void              UI_LayoutPosition_InPlaceRecursive(UI_Box *root, Axis2 axis);

////////////////////////////////
//~ rjf: Box Building API

engine_function UI_Box *          UI_PushParent(UI_Box *box);
engine_function UI_Box *          UI_PopParent(void);
engine_function Vec2F32 *         UI_PushViewOffStorage(Vec2F32 *v);
engine_function Vec2F32 *         UI_PopViewOffStorage(void);
engine_function void              UI_BeginTooltip(void);
engine_function void              UI_EndTooltip(void);
engine_function UI_Box *          UI_ActiveParent(void);
engine_function UI_Box *          UI_BoxFromKey(UI_Key key);
engine_function UI_Box *          UI_BoxMake(UI_BoxFlags flags, String8 string);
engine_function UI_Box *          UI_BoxMakeF(UI_BoxFlags flags, char *fmt, ...);
engine_function void              UI_BoxEquipString(UI_Box *box, String8 string);
engine_function void              UI_BoxEquipFancyStringList(UI_Box *box, FancyStringList list);
engine_function void              UI_BoxEquipData(UI_Box *box, String8 data);
engine_function void              UI_BoxEquipChildLayoutAxis(UI_Box *box, Axis2 axis);
engine_function void              UI_BoxEquipHoverCursor(UI_Box *box, OS_CursorKind cursor);
engine_function void              UI_BoxEquipTexture(UI_Box *box, R_Handle texture, Rng2F32 src);
engine_function void              UI_BoxEquipDrawBucket(UI_Box *box, DR_Bucket *draw_list);
engine_function void              UI_BoxEquipDrawBucketRelative(UI_Box *box, DR_Bucket *draw_list);
engine_function Vec2F32           UI_BoxTextPosition(UI_Box *box);
engine_function U64               UI_BoxCharPosFromXY(UI_Box *box, Vec2F32 xy);

////////////////////////////////
//~ rjf: Stacks

//- rjf: base
engine_function F32 UI_PushFixedX(F32 v);
engine_function F32 UI_PopFixedX(void);
engine_function F32 UI_PushFixedY(F32 v);
engine_function F32 UI_PopFixedY(void);
engine_function F32 UI_PushFixedWidth(F32 v);
engine_function F32 UI_PopFixedWidth(void);
engine_function F32 UI_PushFixedHeight(F32 v);
engine_function F32 UI_PopFixedHeight(void);
engine_function UI_Size UI_PushPrefWidth(UI_Size v);
engine_function UI_Size UI_PopPrefWidth(void);
engine_function UI_Size UI_PushPrefHeight(UI_Size v);
engine_function UI_Size UI_PopPrefHeight(void);
engine_function F32 UI_PushMinWidth(F32 v);
engine_function F32 UI_PopMinWidth(void);
engine_function F32 UI_PushMinHeight(F32 v);
engine_function F32 UI_PopMinHeight(void);
engine_function UI_BoxFlags UI_PushFlags(UI_BoxFlags v);
engine_function UI_BoxFlags UI_PopFlags(void);
engine_function Vec4F32 UI_PushBackgroundColor(Vec4F32 v);
engine_function Vec4F32 UI_PopBackgroundColor(void);
engine_function Vec4F32 UI_PushTextColor(Vec4F32 v);
engine_function Vec4F32 UI_PopTextColor(void);
engine_function Vec4F32 UI_PushBorderColor(Vec4F32 v);
engine_function Vec4F32 UI_PopBorderColor(void);
engine_function R_Font UI_PushFont(R_Font v);
engine_function R_Font UI_PopFont(void);
engine_function F32 UI_PushCornerRadius(F32 v);
engine_function F32 UI_PopCornerRadius(void);

//- rjf: helpers
engine_function Rng2F32 UI_PushRect(Rng2F32 rect);
engine_function Rng2F32 UI_PopRect(void);
engine_function UI_Size UI_PushPrefSize(Axis2 axis, UI_Size v);
engine_function UI_Size UI_PopPrefSize(Axis2 axis);

////////////////////////////////
//~ rjf: Macro Loop Wrappers

//- rjf: built-in stacks (not auto-generated)
#define UI_ViewOffStorage(ptr) DeferLoop(UI_PushViewOffStorage(ptr), UI_PopViewOffStorage())

//- rjf: stacks (base)
#define UI_FixedX(v)           DeferLoop(UI_PushFixedX(v), UI_PopFixedX())
#define UI_FixedY(v)           DeferLoop(UI_PushFixedY(v), UI_PopFixedY())
#define UI_PrefWidth(v)        DeferLoop(UI_PushPrefWidth(v), UI_PopPrefWidth())
#define UI_PrefHeight(v)       DeferLoop(UI_PushPrefHeight(v), UI_PopPrefHeight())
#define UI_MinWidth(v)         DeferLoop(UI_PushMinWidth(v), UI_PopMinWidth())
#define UI_MinHeight(v)        DeferLoop(UI_PushMinHeight(v), UI_PopMinHeight())
#define UI_Flags(f)            DeferLoop(UI_PushFlags(f), UI_PopFlags())
#define UI_BackgroundColor(v)  DeferLoop(UI_PushBackgroundColor(v), UI_PopBackgroundColor())
#define UI_TextColor(v)        DeferLoop(UI_PushTextColor(v), UI_PopTextColor())
#define UI_BorderColor(v)      DeferLoop(UI_PushBorderColor(v), UI_PopBorderColor())
#define UI_Font(f)             DeferLoop(UI_PushFont(f), UI_PopFont())
#define UI_CornerRadius(f)     DeferLoop(UI_PushCornerRadius(f), UI_PopCornerRadius())

//- rjf: stacks (compositions)
#define UI_WidthFill     UI_PrefWidth(UI_Pct(1.f, 0.f))
#define UI_HeightFill    UI_PrefHeight(UI_Pct(1.f, 0.f))
#define UI_Rect(r)       DeferLoop(UI_PushRect(r), UI_PopRect())

//- rjf: tooltip
#define UI_Tooltip DeferLoop(UI_BeginTooltip(), UI_EndTooltip())

////////////////////////////////
//~ rjf: Box Interaction

engine_function UI_InteractResult UI_BoxInteract(UI_Box *box);

#endif // UI_H
