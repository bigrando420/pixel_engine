#ifndef BASE_MEMORY_H
#define BASE_MEMORY_H

////////////////////////////////
//~ rjf: Limits

#if !defined(M_COMMIT_SIZE)
#define M_COMMIT_SIZE Kilobytes(4)
#endif

#if !defined(M_DECOMMIT_THRESHOLD)
#define M_DECOMMIT_THRESHOLD Kilobytes(64)
#endif

////////////////////////////////
//~ rjf: Reservation

typedef struct M_Node M_Node;
struct M_Node
{
    M_Node *first;
    M_Node *last;
    M_Node *next;
    M_Node *prev;
    M_Node *parent;
    U64 size;
};

////////////////////////////////
//~ rjf: Arena

typedef struct M_Arena M_Arena;
struct M_Arena
{
    M_Node node;
    U64 commit_pos;
    U64 pos;
    U64 align;
};

////////////////////////////////
//~ rjf: Arena Helpers

typedef struct M_Temp M_Temp;
struct M_Temp
{
    M_Arena *arena;
    U64 pos;
};

////////////////////////////////
//~ rjf: Heap

typedef struct M_HeapChunk M_HeapChunk;
struct M_HeapChunk
{
    M_HeapChunk *next;
    U64 size;
};

typedef struct M_HeapSlot M_HeapSlot;
struct M_HeapSlot
{
    M_HeapChunk *first_free;
};

typedef struct M_Heap M_Heap;
struct M_Heap
{
    M_Arena *arena;
    U64 slot_table_size;
    M_HeapSlot *slot_table;
};

////////////////////////////////
//~ rjf: Globals

engine_global M_Node m_g_nil_node;

////////////////////////////////
//~ rjf: Base Node Functions

#define M_CheckNilNode(p) (M_NodeIsNil(p))
#define M_SetNilNode(p) ((p) = M_NilNode())
engine_function M_Node * M_NilNode(void);
engine_function B32      M_NodeIsNil(M_Node *node);
engine_function M_Node * M_NodeAlloc(U64 cap);
engine_function void     M_NodeRelease(M_Node *node);
engine_function void     M_NodeInsertChild(M_Node *parent, M_Node *prev_child, M_Node *new_child);
engine_function void     M_NodeRemoveChild(M_Node *parent, M_Node *node);

////////////////////////////////
//~ rjf: Arena Functions

engine_function M_Arena *M_ArenaAlloc(U64 cap);
engine_function M_Arena *M_ArenaAllocDefault(void);
engine_function void     M_ArenaRelease(M_Arena *arena);
engine_function void *   M_ArenaPushAligned(M_Arena *arena, U64 size, U64 align);
engine_function void *   M_ArenaPush(M_Arena *arena, U64 size);
engine_function void *   M_ArenaPushZero(M_Arena *arena, U64 size);
engine_function void     M_ArenaSetPosBack(M_Arena *arena, U64 pos);
engine_function void     M_ArenaSetAutoAlign(M_Arena *arena, U64 align);
engine_function void     M_ArenaPop(M_Arena *arena, U64 size);
engine_function void     M_ArenaClear(M_Arena *arena);
engine_function U64      M_ArenaGetPos(M_Arena *arena);
#define PushArray(a,T,c)     (T*)M_ArenaPush((a), sizeof(T)*(c))
#define PushArrayZero(a,T,c) (T*)M_ArenaPushZero((a), sizeof(T)*(c))
#define PushStruct(a,T)      (T*)M_ArenaPushZero((a), sizeof(T))

////////////////////////////////
//~ rjf: Temp

engine_function M_Temp M_BeginTemp(M_Arena *arena);
engine_function void M_EndTemp(M_Temp temp);

////////////////////////////////
//~ rjf: Heap Functions

engine_function M_Heap *M_HeapAlloc(U64 cap);
engine_function void M_HeapRelease(M_Heap *heap);
engine_function void *M_HeapAllocData(M_Heap *heap, U64 size);
engine_function void M_HeapReleaseData(M_Heap *heap, void *ptr);
engine_function void *M_HeapReAllocData(M_Heap *heap, void *ptr, U64 new_size);

#endif // BASE_MEMORY_H
