#ifndef BASE_VALUE_INTERN_H
#define BASE_VALUE_INTERN_H

////////////////////////////////
//~ rjf: Types

typedef union VIN_Literal VIN_Literal;
union VIN_Literal
{
    U64 u64;
    S64 s64;
    F32 f32;
    Rng2F32 rng2f32;
};

typedef struct VIN_Value VIN_Value;
struct VIN_Value
{
    U64 storage[16];
};

typedef struct VIN_Node VIN_Node;
struct VIN_Node
{
    VIN_Node *next;
    String8 key;
    VIN_Value value;
};

typedef struct VIN_State VIN_State;
struct VIN_State
{
    M_Arena *arena;
    U64 table_size;
    VIN_Node **table;
};

////////////////////////////////
//~ rjf: Globals

engine_global VIN_Node vin_g_nil_node;
engine_global VIN_State *vin_state;

////////////////////////////////
//~ rjf: Functions

engine_function void VIN_Init(void);
engine_function U64 VIN_HashFromString(String8 string);
engine_function VIN_Value *VIN_ValueFromString(VIN_Literal default_value, String8 string);
engine_function VIN_Value *VIN_ValueFromStringF(VIN_Literal default_value, char *fmt, ...);

#define VIN_Val(type, default_val, name) (type *)(&VIN_ValueFromString((default_val), (name))->storage[0])
#define VIN_ValF(type, default_val, ...) (type *)(&VIN_ValueFromStringF((default_val), __VA_ARGS__)->storage[0])

#endif // BASE_VALUE_INTERN_H
