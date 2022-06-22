#ifndef CONFIG_H
#define CONFIG_H

typedef struct CFG_CachedFile CFG_CachedFile;
struct CFG_CachedFile
{
    CFG_CachedFile *next;
    M_Arena *arena;
    String8 path;
    MD_Node *root;
    OS_Timestamp last_modified_time;
    U64 last_checked_frame;
    U64 generation;
    void *user_data;
};

typedef struct CFG_NodeFilePair CFG_NodeFilePair;
struct CFG_NodeFilePair
{
    MD_Node *node;
    CFG_CachedFile *file;
};

typedef struct CFG_State CFG_State;
struct CFG_State
{
    M_Arena *arena;
    U64 table_size;
    CFG_CachedFile **table;
    U64 frame_index;
    
    M_Arena *config_path_arena;
    String8 config_path;
    M_Arena *project_path_arena;
    String8 project_path;
};

engine_function U64 CFG_HashFromString(String8 string);
engine_function CFG_CachedFile *CFG_CachedFileFromPath(String8 path);
engine_function CFG_NodeFilePair CFG_NodeFilePairFromKey(String8 key, B32 alt_file_possible);
engine_function MD_Node *CFG_NodeFromKey(String8 key, B32 alt_file_possible);
engine_function B32 CFG_B32FromKey(String8 key);
engine_function S64 CFG_S64FromKey(String8 key);
engine_function String8 CFG_ConfigPath(void);
engine_function String8 CFG_ProjectPath(void);
engine_function void CFG_SetConfigPath(String8 path);
engine_function void CFG_SetProjectPath(String8 path);
engine_function void CFG_BeginFrame(void);
engine_function void CFG_EndFrame(void);
engine_function void CFG_Init(void);

#endif // CONFIG_H
