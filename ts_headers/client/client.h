#ifndef CLIENT_H
#define CLIENT_H

////////////////////////////////
//~ rjf: Generated Code

#include "generated/client.meta.h"

////////////////////////////////
//~ rjf: Handles

typedef struct C_Handle C_Handle;
struct C_Handle
{
    U64 u64[2];
};

typedef struct C_HandleNode C_HandleNode;
struct C_HandleNode
{
    C_HandleNode *next;
    C_Handle handle;
};

typedef struct C_HandleList C_HandleList;
struct C_HandleList
{
    C_HandleNode *first;
    C_HandleNode *last;
    U64 count;
};

////////////////////////////////
//~ rjf: Entities

typedef U64 C_EntityID;

typedef U64 C_EntityFlags;
enum
{
    C_EntityFlag_HasTextPoint = (1<<0),
};

typedef struct C_Entity C_Entity;
struct C_Entity
{
    // rjf: tree links
    C_Entity *first;
    C_Entity *last;
    C_Entity *next;
    C_Entity *prev;
    C_Entity *parent;
    
    // rjf: allocation data
    C_EntityID id;
    U64 generation;
    
    // rjf: allocation-less equipment
    C_EntityKind kind;
    C_EntityFlags flags;
    TE_Point txt_point;
    
    // rjf: [allocated equipment] name
    M_Arena *name_arena;
    String8 name;
};

typedef struct C_EntityRec C_EntityRec;
struct C_EntityRec
{
    C_Entity *next;
    int push_count;
    int pop_count;
};

////////////////////////////////
//~ rjf: Views

typedef struct C_View C_View;
struct C_View
{
    C_View *next;
    C_View *prev;
    U64 generation;
    M_Arena *arena;
    String8 command;
    C_Entity *entity;
    Vec2F32 view_off;
    void *plugin_state;
};

////////////////////////////////
//~ rjf: Panels

typedef struct C_Panel C_Panel;
struct C_Panel
{
    // rjf: tree links & data
    C_Panel *first;
    C_Panel *last;
    C_Panel *next;
    C_Panel *prev;
    C_Panel *parent;
    U64 child_count;
    
    // rjf: allocation data
    U64 generation;
    
    // rjf: split data
    Axis2 split_axis;
    F32 pct_of_parent;
    
    // rjf: leaf data
    TE_Point cursor;
    TE_Point mark;
    B32 input_is_focused;
    String8 input;
    U8 input_buffer[4096];
    String8 error;
    U8 error_buffer[4096];
    F32 error_t;
    C_View *first_view;
    C_View *last_view;
    U64 view_count;
    C_View *selected_view;
    C_View *transient_view;
    
    // rjf: per-frame data
    UI_Box *content_box;
};

typedef struct C_PanelRec C_PanelRec;
struct C_PanelRec
{
    C_Panel *next;
    int push_count;
    int pop_count;
};

////////////////////////////////
//~ rjf: Commands

typedef struct C_CmdRegister C_CmdRegister;
struct C_CmdRegister
{
    C_Panel *panel;
    C_View *view;
};

typedef struct C_Cmd C_Cmd;
struct C_Cmd
{
    C_CmdRegister reg;
    String8 string;
};

typedef struct C_CmdNode C_CmdNode;
struct C_CmdNode
{
    C_CmdNode *next;
    C_CmdNode *prev;
    C_Cmd cmd;
};

typedef struct C_CmdList C_CmdList;
struct C_CmdList
{
    C_CmdNode *first;
    C_CmdNode *last;
    U64 count;
};

typedef struct C_CmdKindInfo C_CmdKindInfo;
struct C_CmdKindInfo
{
    String8 name;
    String8 description;
};

////////////////////////////////
//~ rjf: Plugins

typedef void *C_PluginTickGlobalFunction(M_Arena *arena, void *global_state, OS_EventList *events, C_CmdList *cmds);
typedef void *C_PluginTickViewFunction(M_Arena *arena, void *per_view_state, APP_Window *window, OS_EventList *events, C_CmdList *cmds, Rng2F32 rect);
typedef void C_PluginCloseViewFunction(void *per_view_state);

typedef struct C_PluginCmdKindNode C_PluginCmdKindNode;
struct C_PluginCmdKindNode
{
    C_PluginCmdKindNode *hash_next;
    String8 name;
    String8 description;
};

typedef struct C_Plugin C_Plugin;
struct C_Plugin
{
    // rjf: "retained mode" table links
    C_Plugin *hash_next;
    C_Plugin *hash_prev;
    
    // rjf: per-frame linear list link
    C_Plugin *next;
    
    // rjf: stable state
    M_Arena *arena;
    String8 name;
    void *global_state;
    C_PluginTickGlobalFunction *TickGlobal;
    C_PluginTickViewFunction *TickView;
    C_PluginCloseViewFunction *CloseView;
    OS_Handle library;
    OS_Timestamp last_modified_time;
    U64 cmd_kind_node_table_size;
    C_PluginCmdKindNode **cmd_kind_node_table;
};

typedef struct C_PluginSlot C_PluginSlot;
struct C_PluginSlot
{
    C_Plugin *first;
    C_Plugin *last;
};

////////////////////////////////
//~ rjf: Main Client Thread Job Queue Types

typedef void C_JobFunction(void *params);

typedef struct C_Job C_Job;
struct C_Job
{
    U64 params_size;
    C_JobFunction *func;
};

typedef struct C_Worker C_Worker;
struct C_Worker
{
    U8 *ring;
    U64 ring_size;
    U64 ring_write_pos;
    U64 ring_read_pos;
    OS_Handle thread;
    OS_Handle jobs_present_semaphore;
    OS_Handle job_done_semaphore;
};

////////////////////////////////
//~ rjf: Main State

typedef struct C_State C_State;
struct C_State
{
    M_Arena *arena;
    
    // rjf: view allocation info
    C_View *free_view;
    
    // rjf: panel allocation info
    C_Panel *free_panel;
    
    // rjf: entity allocation info
    C_Entity *free_entity;
    C_Entity *root_entity;
    U64 entity_id_gen;
    
    // rjf: plugin allocation info
    C_Plugin *free_plugin;
    C_Plugin *first_plugin;
    C_Plugin *last_plugin;
    U64 plugin_table_size;
    C_PluginSlot *plugin_table;
    
    // rjf: misc. frame/frontend state
    U64 frame_index;
    R_Handle logo;
    
    // rjf: workers
    U64 worker_count;
    C_Worker *workers;
    C_Job *free_job;
};

////////////////////////////////
//~ rjf: Globals

engine_global C_Entity c_g_nil_entity;
engine_global C_View c_g_nil_view;
engine_global C_Panel c_g_nil_panel;

////////////////////////////////
//~ rjf: Basic Helpers

engine_function U64 C_HashFromString(String8 string);
engine_function U64 C_HashFromString_CaseInsensitive(String8 string);

////////////////////////////////
//~ rjf: Handles

engine_function C_Handle C_HandleZero(void);
engine_function void C_HandleListPush(M_Arena *arena, C_HandleList *list, C_Handle handle);

////////////////////////////////
//~ rjf: Views

#define C_ViewSetNil(p) ((p) = &c_g_nil_view)
engine_function B32 C_ViewIsNil(C_View *view);
engine_function C_Handle C_HandleFromView(C_View *view);
engine_function C_View *C_ViewFromHandle(C_Handle handle);
engine_function C_View *C_ViewAlloc(void);
engine_function void C_ViewRelease(C_View *view);
engine_function void C_ViewSetCommandAndEntity(C_View *view, String8 string, C_Entity *entity);

////////////////////////////////
//~ rjf: Panels

#define C_PanelSetNil(p) ((p) = &c_g_nil_panel)
engine_function B32 C_PanelIsNil(C_Panel *panel);
engine_function C_Panel *C_PanelAlloc(void);
engine_function void C_PanelRelease(C_Panel *panel);
engine_function void C_PanelInsert(C_Panel *parent, C_Panel *prev_child, C_Panel *new_child);
engine_function void C_PanelRemove(C_Panel *parent, C_Panel *child);
engine_function C_PanelRec C_PanelRecDF(C_Panel *panel, U64 sib_off, U64 child_off);
#define C_PanelRecDF_Pre(panel) C_PanelRecDF(panel, OffsetOf(C_Panel, next), OffsetOf(C_Panel, first))
#define C_PanelRecDF_Post(panel) C_PanelRecDF(panel, OffsetOf(C_Panel, prev), OffsetOf(C_Panel, last))
engine_function Rng2F32 C_RectFromPanelChild(Rng2F32 parent_rect, C_Panel *parent, C_Panel *panel);
engine_function Rng2F32 C_RectFromPanel(Rng2F32 root_rect, C_Panel *root, C_Panel *panel);
engine_function void C_PanelFillInput(C_Panel *panel, String8 string);
engine_function void C_PanelError(C_Panel *panel, String8 string);
engine_function void C_PanelErrorF(C_Panel *panel, char *fmt, ...);
engine_function C_Handle C_HandleFromPanel(C_Panel *panel);
engine_function C_Panel *C_PanelFromHandle(C_Handle handle);
engine_function C_Entity *C_EntityFromCommandString(String8 command);
engine_function void C_PanelInsertView(C_Panel *panel, C_View *prev_view, C_View *view);
engine_function void C_PanelRemoveView(C_Panel *panel, C_View *view);

////////////////////////////////
//~ rjf: Entities

//- rjf: type engine_functions
#define C_EntitySetNil(p) ((p) = &c_g_nil_entity)
engine_function B32 C_EntityIsNil(C_Entity *entity);
engine_function C_Handle C_HandleFromEntity(C_Entity *entity);
engine_function C_Entity *C_EntityFromHandle(C_Handle handle);

//- rjf: allocation & tree building engine_functions
engine_function C_Entity *C_EntityAlloc(C_Entity *parent, C_EntityKind kind);
engine_function void C_EntityRelease(C_Entity *entity);
engine_function C_Entity *C_EntityChangeParent(C_Entity *entity, C_Entity *new_parent);

//- rjf: equipment
engine_function M_Arena *C_EntityEquipOrClearNameArena(C_Entity *entity);
engine_function void C_EntityEquipName(C_Entity *entity, String8 string);
engine_function void C_EntityEquipTextPoint(C_Entity *entity, TE_Point point);

//- rjf: introspection
engine_function C_Entity *C_EntityChildFromKindAndString(C_Entity *parent, C_EntityKind kind, String8 string, MatchFlags flags);
engine_function String8 C_FullPathFromEntity(M_Arena *arena, C_Entity *entity);

//- rjf: recursion iterators
engine_function C_EntityRec C_EntityRecDF(C_Entity *entity, MemberOffset child_off, MemberOffset sib_off);
#define C_EntityRecDF_Pre(entity)  (C_EntityRecDF((entity), MemberOff(C_Entity, first), MemberOff(C_Entity, next)))
#define C_EntityRecDF_Post(entity) (C_EntityRecDF((entity), MemberOff(C_Entity, last), MemberOff(C_Entity, prev)))

////////////////////////////////
//~ rjf: Entity Construction Helpers

engine_function C_Entity *C_OpenPath(String8 path);
engine_function C_Entity *C_OpenFile(String8 path);
engine_function C_Entity *C_EntityEquipPathName(C_Entity *entity, String8 path);

////////////////////////////////
//~ rjf: Plugins

engine_function void *C_PluginTickGlobalStub(M_Arena *arena, void *state_ptr, OS_EventList *events, C_CmdList *cmds);
engine_function void *C_PluginTickViewStub(M_Arena *arena, void *state_ptr, APP_Window *window, OS_EventList *events, C_CmdList *cmds, Rng2F32 rect);
engine_function void C_PluginCloseViewStub(void *state_ptr);
engine_function C_Plugin *C_PluginFromString(String8 string);
engine_function void C_SelectPlugin(C_Plugin *plugin);
engine_function void C_RegisterCommands(U64 count, C_CmdKindInfo *info);

////////////////////////////////
//~ rjf: State Accessors

engine_function R_Handle C_Logo(void);
engine_function C_Entity *C_RootEntity(void);
engine_function C_Entity *C_EntityFromID(C_EntityID id);
engine_function C_Entity *C_EntityFromIDString(String8 string);
engine_function C_Entity *C_EntityFromKindAndString(C_EntityKind kind, String8 string);

////////////////////////////////
//~ rjf: Jobs

engine_function void C_WorkerPushData(C_Worker *worker, U64 size, void *data);
#define C_WorkerPushStruct(worker, ptr) C_WorkerPushData((worker), sizeof(*(ptr)), (ptr))
#define C_WorkerPushStr8Copy(worker, string) C_WorkerPushData((worker), (string).size, (string).str)
engine_function C_Handle C_PushJob(String8 params, C_JobFunction *func);
engine_function void C_WorkerThreadFunction(void *p);

////////////////////////////////
//~ rjf: Main Hooks

engine_function void C_Init(void);
engine_function void C_BeginFrame(void);
engine_function void C_EndFrame(void);
engine_function void C_Quit(void);

#endif // CLIENT_H
