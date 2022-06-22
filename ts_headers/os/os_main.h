#ifndef OS_MAIN_H
#define OS_MAIN_H

#define M_IMPL_Reserve  OS_Reserve
#define M_IMPL_Release  OS_Release
#define M_IMPL_Commit   OS_Commit
#define M_IMPL_Decommit OS_Decommit

typedef struct OS_Handle OS_Handle;
struct OS_Handle
{
    U64 u64[1];
};

typedef U64 OS_Timestamp;

typedef struct OS_FileIter OS_FileIter;
struct OS_FileIter
{
    U8 opaque[1024];
};

typedef U32 OS_FileFlags;
enum
{
    OS_FileFlag_Directory = (1<<0),
};

typedef void OS_ThreadFunction(void *params);

typedef struct OS_FileAttributes OS_FileAttributes;
struct OS_FileAttributes
{
    OS_FileFlags flags;
    U64 size;
    OS_Timestamp last_modified;
};

typedef struct OS_FileInfo OS_FileInfo;
struct OS_FileInfo
{
    String8 name;
    OS_FileAttributes attributes;
};

typedef struct OS_FileLoadResult OS_FileLoadResult;
struct OS_FileLoadResult
{
    B32 success;
    String8 string;
};

typedef enum OS_SystemPath
{
    OS_SystemPath_Null,
    OS_SystemPath_Initial,
    OS_SystemPath_Current,
    OS_SystemPath_Binary,
    OS_SystemPath_AppData,
    OS_SystemPath_COUNT,
} OS_SystemPath;

typedef struct OS_ProcessStatus OS_ProcessStatus;
struct OS_ProcessStatus
{
    B8 launch_failed;
    B8 running;
    B8 read_failed;
    B8 kill_failed;
    B8 was_killed;
    U32 exit_code;
};

////////////////////////////////
//~ rjf: Helpers

engine_function OS_Handle OS_HandleZero(void);
engine_function B32 OS_HandleMatch(OS_Handle a, OS_Handle b);
engine_function String8 OS_NormalizedPathFromString(M_Arena *arena, String8 string);

////////////////////////////////
//~ rjf: @os_per_backend General Program API

engine_function void    OS_Init(void);
engine_function void    OS_Abort(void);
engine_function String8 OS_GetSystemPath(M_Arena *arena, OS_SystemPath path);

////////////////////////////////
//~ rjf: @os_per_backend Memory

// TODO(rjf): I should be able to mark pages as read-only, or executable, etc.

engine_function U64   OS_PageSize(void);
engine_function void *OS_Reserve(U64 size);
engine_function void  OS_Release(void *ptr);
engine_function void  OS_Commit(void *ptr, U64 size);
engine_function void  OS_Decommit(void *ptr, U64 size);

////////////////////////////////
//~ rjf: @os_per_backend Libraries

engine_function OS_Handle     OS_LibraryOpen(String8 path);
engine_function void          OS_LibraryClose(OS_Handle handle);
engine_function VoidFunction *OS_LibraryLoadFunction(OS_Handle handle, String8 name);

////////////////////////////////
//~ rjf: @os_per_backend File System

engine_function OS_FileLoadResult OS_LoadEntireFile(M_Arena *arena, String8 path);
engine_function B32               OS_SaveToFile(String8 path, String8List data);
engine_function void              OS_DeleteFile(String8 path);
engine_function void              OS_MoveFile(String8 path, String8 new_path);
engine_function B32               OS_CopyFile(String8 path, String8 new_path);
engine_function B32               OS_MakeDirectory(String8 path);
engine_function OS_FileIter *     OS_FileIterBegin(M_Arena *arena, String8 path);
engine_function B32               OS_FileIterNext(M_Arena *arena, OS_FileIter *it, OS_FileInfo *out_info);
engine_function void              OS_FileIterEnd(OS_FileIter *it);
engine_function OS_FileAttributes OS_FileAttributesFromPath(String8 path);

////////////////////////////////
//~ rjf: @os_per_backend Time

engine_function U64 OS_TimeMicroseconds(void);
engine_function U64 OS_TimeCycles(void);
engine_function void OS_Sleep(U64 milliseconds);

////////////////////////////////
//~ rjf: @os_per_backend Threads & Synchronization Primitives

engine_function U64 OS_GetTID(void);
engine_function OS_Handle OS_ThreadStart(void *params, OS_ThreadFunction *func);
engine_function void OS_ThreadJoin(OS_Handle thread);
engine_function void OS_ThreadDetach(OS_Handle thread);

engine_function OS_Handle OS_SemaphoreAlloc(U32 max_count);
engine_function void OS_SemaphoreRelease(OS_Handle handle);
engine_function void OS_SemaphoreWait(OS_Handle handle, U32 max_milliseconds);
engine_function void OS_SemaphoreSignal(OS_Handle handle);

////////////////////////////////
//~ rjf: @os_per_backend Child Processes

engine_function OS_Handle        OS_ProcessLaunch(String8 command, String8 working_directory);
engine_function void             OS_ProcessRelease(OS_Handle handle);
engine_function String8          OS_ProcessReadOutput(M_Arena *arena, OS_Handle process);
engine_function void             OS_ProcessKill(OS_Handle process);
engine_function U64              OS_PIDFromProcess(OS_Handle process);
engine_function OS_ProcessStatus OS_StatusFromProcess(OS_Handle process);

////////////////////////////////
//~ rjf: @os_per_backend Miscellaneous

engine_function void OS_GetEntropy(void *data, U64 size);
engine_function void OS_OpenCodeFileInDevTools(String8 path, int line);

////////////////////////////////
//~ rjf: @os_per_backend System Info

engine_function F32 OS_CaretBlinkTime(void);
engine_function F32 OS_DoubleClickTime(void);
engine_function U64 OS_LogicalProcessorCount(void);

////////////////////////////////
//~ rjf: Entry Point

#if !defined(OS_NO_ENTRY_POINT)
function void APP_EntryPoint(void);
#endif

#endif // OS_MAIN_H
