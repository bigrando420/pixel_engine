#ifndef COMMANDS_H
#define COMMANDS_H

typedef enum CMD_NodeKind
{
    CMD_NodeKind_Null,
    CMD_NodeKind_Root,
    CMD_NodeKind_Argument,
    CMD_NodeKind_Value,
    CMD_NodeKind_List,
    CMD_NodeKind_COUNT
}
CMD_NodeKind;

typedef struct CMD_Node CMD_Node;
struct CMD_Node
{
    CMD_NodeKind kind;
    CMD_Node *first;
    CMD_Node *last;
    CMD_Node *next;
    CMD_Node *prev;
    CMD_Node *parent;
    String8 string;
    Rng1U64 range_in_input;
};

////////////////////////////////
//~ rjf: Command Parser

engine_global CMD_Node cmd_node_nil;

#define CMD_NodeSetNil(p) ((p) = &cmd_node_nil)
engine_function B32 CMD_NodeIsNil(CMD_Node *n);
engine_function CMD_Node *CMD_PushNode(M_Arena *arena, CMD_NodeKind kind, String8 string, Rng1U64 range_in_input);
engine_function CMD_Node *CMD_PushChild(CMD_Node *parent, CMD_Node *child);
engine_function CMD_Node *CMD_ParseNodeFromString(M_Arena *arena, String8 string);
engine_function U64 CMD_IndexFromStringTable(String8 command, U64 string_table_count, String8 *string_table);

#endif // COMMANDS_H
