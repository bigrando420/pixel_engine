#ifndef WIN32_MAIN_H
#define WIN32_MAIN_H

////////////////////////////////
//~ rjf: Types

#pragma push_macro("function")
#undef function
#include <windows.h>
#include <windowsx.h>
#include <tlhelp32.h>
#include <Shlobj.h>
#pragma pop_macro("function")

#if LANG_CPP
#define VTblCall(obj, name, ...) (obj)->name(__VA_ARGS__)
#else
#define VTblCall(obj, name, ...) (obj)->lpVtbl->name((obj), __VA_ARGS__)
#endif

typedef union W32_Process W32_Process;
union W32_Process
{
    W32_Process *next;
    struct
    {
        HANDLE parent_read;
        PROCESS_INFORMATION info;
        OS_ProcessStatus status;
    };
};

typedef struct W32_Thread W32_Thread;
struct W32_Thread
{
    W32_Thread *next;
    HANDLE handle;
    DWORD thread_id;
    void *params;
    OS_ThreadFunction *func;
};

typedef struct W32_FileFindData W32_FileFindData;
struct W32_FileFindData
{
    HANDLE handle;
    B32 returned_first;
    WIN32_FIND_DATAW find_data;
};
StaticAssert(sizeof(W32_FileFindData) <= sizeof(OS_FileIter), W32_FileFindData_Size);

////////////////////////////////
//~ rjf: Globals

engine_global HINSTANCE w32_hinstance;
engine_global M_Arena *w32_perm_arena;
engine_global B32 w32_got_system_info;
engine_global SYSTEM_INFO w32_system_info;
engine_global W32_Process *w32_free_process;
engine_global SRWLOCK w32_mutex;
engine_global HMODULE w32_advapi_dll;
engine_global BOOL (*RtlGenRandom)(VOID *RandomBuffer, ULONG RandomBufferLength);
engine_global String8 w32_initial_path;
engine_global LARGE_INTEGER w32_counts_per_second;

////////////////////////////////
//~ rjf: Helpers

// TODO(rjf): sort these

function void W32_ReadWholeBlock(HANDLE file, void *data, U64 data_len);
function void W32_WriteWholeBlock(HANDLE file, String8List data);
function OS_FileAttributes W32_FileAttributesFromFindData(WIN32_FIND_DATAW find_data);
function W32_Process* W32_ProcessAlloc(void);
function void W32_ProcessFree(W32_Process *process);
function B32 W32_ProcessIsRunning(String8 process_name);

////////////////////////////////
//~ rjf: Threads

function OS_Handle W32_HandleFromThread(W32_Thread *thread);
function W32_Thread *W32_ThreadFromHandle(OS_Handle handle);

#endif // WIN32_MAIN_H
