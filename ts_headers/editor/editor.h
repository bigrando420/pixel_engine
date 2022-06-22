/* date = March 30th 2022 0:35 am */

#ifndef EDITOR_H
#define EDITOR_H

#include "generated/editor.meta.h"

////////////////////////////////
//~ rjf: Handles

typedef struct ED_Handle ED_Handle;
struct ED_Handle
{
    U64 u64[2];
};

////////////////////////////////
//~ rjf: Cursor Types

typedef U32 ED_ModeFlags;
enum
{
    ED_ModeFlag_Insert      = (1<<0),
    ED_ModeFlag_MultiCursor = (1<<1),
};

typedef struct ED_CursorState ED_CursorState;
struct ED_CursorState
{
    ED_CursorState *next;
    ED_CursorState *prev;
    ED_Handle cursor;
    ED_Handle mark;
    S64 preferred_column;
};

typedef struct ED_CursorStateList ED_CursorStateList;
struct ED_CursorStateList
{
    ED_CursorState *first;
    ED_CursorState *last;
    U64 count;
};

////////////////////////////////
//~ rjf: Text Layout Cache

typedef struct ED_LineLayout ED_LineLayout;
struct ED_LineLayout
{
    S64 line_num;
    F32 scale;
    Rng1U64List wrap_ranges;
};

typedef struct ED_TextLayoutCache ED_TextLayoutCache;
struct ED_TextLayoutCache
{
    M_Arena *arena;
    M_Arena *line_layout_arena;
    U64 line_layout_count;
    U64 line_layout_cap;
    ED_LineLayout *line_layouts;
    U64 line_count;
    F32 total_height;
    F32 line_height;
    Rng1U64Node *first_free_rng_node;
};

////////////////////////////////
//~ rjf: Tokens

typedef struct ED_Token ED_Token;
struct ED_Token
{
    ED_TokenKind kind;
    Rng1U64 range;
};

typedef struct ED_TokenNode ED_TokenNode;
struct ED_TokenNode
{
    ED_TokenNode *next;
    ED_Token token;
};

typedef struct ED_TokenList ED_TokenList;
struct ED_TokenList
{
    ED_TokenNode *first;
    ED_TokenNode *last;
    U64 count;
};

////////////////////////////////
//~ rjf: Token Patterns

typedef U32 ED_TokenPatternPieceFlags;
enum
{
    ED_TokenPatternPieceFlag_Name      = (1<<0),
    ED_TokenPatternPieceFlag_AllowMany = (1<<1),
    ED_TokenPatternPieceFlag_Optional  = (1<<2),
};

typedef struct ED_TokenPatternPiece ED_TokenPatternPiece;
struct ED_TokenPatternPiece
{
    ED_TokenPatternPiece *next;
    ED_TokenPatternPiece *alt;
    ED_TokenPatternPieceFlags flags;
    ED_TokenKind kind;
    String8 string;
};

typedef struct ED_TokenPattern ED_TokenPattern;
struct ED_TokenPattern
{
    ED_TokenPatternPiece *first;
    ED_TokenPatternPiece *last;
    U64 count;
};

typedef struct ED_TokenPatternMatchResult ED_TokenPatternMatchResult;
struct ED_TokenPatternMatchResult
{
    B32 valid;
    ED_TokenNode *name;
    ED_TokenNode *first;
    ED_TokenNode *last;
};

////////////////////////////////
//~ rjf: Token Ranges

typedef struct ED_TokenRangePoint ED_TokenRangePoint;
struct ED_TokenRangePoint
{
    ED_TokenRangeKind kind;
    TE_Point point;
    U64 id;
    B32 starter;
};

typedef struct ED_TokenRangePointArray ED_TokenRangePointArray;
struct ED_TokenRangePointArray
{
    ED_TokenRangePoint *v;
    U64 count;
};

typedef struct ED_TokenRange ED_TokenRange;
struct ED_TokenRange
{
    ED_TokenRangeKind kind;
    TE_Range range;
};

typedef struct ED_TokenRangeNodeLoose ED_TokenRangeNodeLoose;
struct ED_TokenRangeNodeLoose
{
    ED_TokenRangeNodeLoose *first;
    ED_TokenRangeNodeLoose *last;
    ED_TokenRangeNodeLoose *next;
    ED_TokenRangeNodeLoose *prev;
    ED_TokenRangeNodeLoose *parent;
    ED_TokenRange range;
};

////////////////////////////////
//~ rjf: Undo/Redo State Types

typedef enum ED_ChangeDirection
{
    ED_ChangeDirection_Undo,
    ED_ChangeDirection_Redo,
    ED_ChangeDirection_COUNT
}
ED_ChangeDirection;
#define ED_ChangeDirectionFlip(d) (!(d))

typedef struct ED_CursorStateOp ED_CursorStateOp;
struct ED_CursorStateOp
{
    ED_CursorStateOp *next;
    TE_Point cursor;
    TE_Point mark;
};

typedef struct ED_TextOp ED_TextOp;
struct ED_TextOp
{
    ED_TextOp *next;
    String8 replaced;
    TE_Range range;
};

typedef struct ED_ChangeRecord ED_ChangeRecord;
struct ED_ChangeRecord
{
    ED_ChangeRecord *next;
    U64 arena_pos;
    U64 glue_code;
    ED_TextOp *first_text_op;
    ED_TextOp *last_text_op;
    ED_CursorStateOp *first_cs_op;
    ED_CursorStateOp *last_cs_op;
};

////////////////////////////////
//~ rjf: Languages

typedef struct ED_LangNode ED_LangNode;
struct ED_LangNode
{
    ED_LangNode *next;
    String8 extension;
    String8 path;
    U64 generation;
};

typedef struct ED_LangTokenStringNode ED_LangTokenStringNode;
struct ED_LangTokenStringNode
{
    ED_LangTokenStringNode *next;
    String8 string;
};

typedef struct ED_LangTokenStringTable ED_LangTokenStringTable;
struct ED_LangTokenStringTable
{
    U64 table_size;
    ED_LangTokenStringNode **table;
};

typedef struct ED_LangRuleSet ED_LangRuleSet;
struct ED_LangRuleSet
{
    U64 generation;
    SP_Pattern token_patterns[ED_TokenKind_COUNT];
    ED_LangTokenStringTable token_tables[ED_TokenKind_COUNT];
    B32 token_range_point_kind_is_nestable[ED_TokenRangeKind_COUNT];
};

////////////////////////////////
//~ rjf: Code Index

typedef enum ED_IndexNodeKind
{
    ED_IndexNodeKind_Null,
    ED_IndexNodeKind_Struct,
    ED_IndexNodeKind_Union,
    ED_IndexNodeKind_Enum,
    ED_IndexNodeKind_Typedef,
    ED_IndexNodeKind_Macro,
    ED_IndexNodeKind_Proc,
    ED_IndexNodeKind_Decl,
    
    ED_IndexNodeKind_File,
    ED_IndexNodeKind_Scope,
    ED_IndexNodeKind_COUNT,
}
ED_IndexNodeKind;

typedef struct ED_IndexNode ED_IndexNode;
struct ED_IndexNode
{
    ED_IndexNode *hash_next;
    ED_IndexNode *next;
    ED_IndexNode *prev;
    ED_IndexNode *first;
    ED_IndexNode *last;
    ED_IndexNode *parent;
    ED_IndexNodeKind kind;
    String8 string;
    ED_Handle entity;
    TE_Point location;
};

////////////////////////////////
//~ rjf: Mutable Text Buffer Types

typedef struct ED_Line ED_Line;
struct ED_Line
{
    // rjf: immediate state
    U64 cap;
    String8 string;
    U64 generation;
    
    // rjf: cached tokens
    U64 cached_tokens_generation;
    U64 cached_tokens_rule_generation;
    ED_TokenList cached_tokens;
};

typedef U32 ED_BufferFlags;
enum
{
    ED_BufferFlag_Ready = (1<<0),
};

typedef struct ED_Buffer ED_Buffer;
struct ED_Buffer
{
    // rjf: allocation & preparedness info
    ED_Buffer *next;
    U64 alloc_generation;
    ED_BufferFlags flags;
    
    // rjf: index of edit
    U64 edit_index;
    
    // rjf: metadata about buffer
    ED_LineEndingEncoding line_ending_encoding;
    
    // rjf: flat, contiguous array of per-line state
    M_Arena *line_arena;
    ED_Line *lines;
    U64 line_count;
    U64 line_cap;
    
    // rjf: token cache
    M_Arena *token_cache_arena;
    ED_TokenNode *first_free_token_node;
    S64 first_invalid_line_num;
    
    // rjf: token range point cache
    U64 token_range_point_id_gen;
    M_Arena *token_range_point_arena;
    ED_TokenRangePointArray token_range_points;
    U64 token_range_point_cap;
    
    // rjf: token range tree
    M_Arena *token_range_tree_arena;
    ED_TokenRangeNodeLoose *token_range_tree_root;
    
    // rjf: change stack
    M_Arena *change_stack_arenas[ED_ChangeDirection_COUNT];
    ED_ChangeRecord *change_stack_top[ED_ChangeDirection_COUNT];
    
    // rjf: text heap
    M_Heap *txt_heap;
};

////////////////////////////////
//~ rjf: Main State Types

typedef struct ED_View ED_View;
struct ED_View
{
    M_Arena *arena;
    ED_ModeFlags mode_flags;
    ED_CursorStateList cursor_states;
    ED_CursorState *first_free_cursor_state;
    ED_TextLayoutCache *text_layout_cache;
};

typedef struct ED_Global ED_Global;
struct ED_Global
{
    M_Arena *arena;
    ED_Buffer nil_buffer;
    ED_Buffer *free_buffer;
    U64 language_table_size;
    ED_LangNode **language_table;
};

////////////////////////////////
//~ rjf: Cursor State

function void ED_CursorStateSetCursor(ED_CursorState *cs, TE_Point point);
function void ED_CursorStateSetMark(ED_CursorState *cs, TE_Point point);
function TE_Point ED_CursorFromCursorState(ED_CursorState *cs);
function TE_Point ED_MarkFromCursorState(ED_CursorState *cs);
function ED_CursorState *ED_MainCursorStateFromView(ED_View *view);
function ED_CursorState *ED_ViewAllocCursorState(ED_View *view);

////////////////////////////////
//~ rjf: Lexing

function ED_TokenKind ED_TokenKindFromKey(String8 key);
function ED_Token ED_TokenMake(ED_TokenKind kind, Rng1U64 range);
function void ED_TokenListPushNode(ED_TokenList *list, ED_TokenNode *node);
function void ED_TokenListPush(M_Arena *arena, ED_TokenList *list, ED_Token token);
function ED_TokenList ED_TokenListFromString(M_Arena *arena, ED_LangRuleSet *lang_rules, String8 string, U64 start);
function ED_Token ED_TokenFromPointInList(U64 point, ED_TokenList list);

////////////////////////////////
//~ rjf: Token Patterns

function ED_TokenPatternPiece *ED_TokenPatternPieceFromMDNode(M_Arena *arena, MD_Node *node);
function ED_TokenPattern *ED_TokenPatternFromMDNode(M_Arena *arena, MD_Node *node);
function ED_TokenPatternMatchResult ED_TokenPatternMatch(ED_TokenList tokens, ED_TokenPattern *pattern);

////////////////////////////////
//~ rjf: Token Ranges

function ED_TokenRangeNodeLoose *ED_TokenRangeTreeFromPointArray_Loose(M_Arena *arena, TE_Point max_point, ED_TokenRangePointArray pts);
function ED_TokenRangeNodeLoose *ED_EnclosingTokenRangeNodeFromPoint_Loose(ED_TokenRangeNodeLoose *root, TE_Point point);

////////////////////////////////
//~ rjf: Language Rule Sets

function ED_LangRuleSet *ED_LangRuleSetFromExtension(String8 extension);

////////////////////////////////
//~ rjf: Buffers

//- rjf: basic type functions
#define ED_BufferSetNil(b) ((b) = ED_BufferNil())
function ED_Buffer *ED_BufferNil(void);
function B32 ED_BufferIsNil(ED_Buffer *buffer);
function ED_Handle ED_HandleFromBuffer(ED_Buffer *buffer);
function ED_Buffer *ED_BufferFromHandle(ED_Handle handle);

//- rjf: buffer info accessors
function TE_Range ED_TextRangeFromBuffer(ED_Buffer *buffer);
function U64 ED_LineCountFromBuffer(ED_Buffer *buffer);
function String8 ED_StringFromBufferLineNum(M_Arena *arena, ED_Buffer *buffer, S64 line_num);
function U64 ED_SizeFromBufferLineNum(ED_Buffer *buffer, S64 line_num);
function Rng1U64 ED_ContributingTokenRangePointRangeFromBufferLineNum_GroundTruth(ED_Buffer *buffer, S64 line_num);
function ED_Line *ED_BufferLineFromNum(ED_Buffer *buffer, S64 line_num);

//- rjf: tokens & token ranges
function ED_TokenList ED_TokenListFromBufferLineNum(ED_LangRuleSet *lang_rules, ED_Buffer *buffer, S64 line_num);
function ED_TokenRangeNodeLoose *ED_TokenRangeTreeFromBuffer_Loose(ED_Buffer *buffer);

//- rjf: text mutation and baking
function void ED_BufferEquipLineEndingEncoding(ED_Buffer *buffer, ED_LineEndingEncoding encoding);
function void ED_BufferLineReplaceRange(ED_LangRuleSet *lang_rules, ED_Buffer *buffer, ED_Line *line, Rng1S64 range, String8 string);
function TE_Range ED_BufferReplaceRange(ED_LangRuleSet *lang_rules, ED_Buffer *buffer, TE_Range range, String8 string);
function String8List ED_BakedStringListFromBufferRange(M_Arena *arena, ED_Buffer *buffer, TE_Range range);
function String8List ED_BakedStringListFromBuffer(M_Arena *arena, ED_Buffer *buffer);

//- rjf: searching
function TE_Point ED_BufferFindNeedle(ED_Buffer *buffer, TE_Point start, B32 forward, String8 needle, MatchFlags flags);

//- rjf: auto-indentation
function void ED_BufferAutoIndentLineNumRange(ED_LangRuleSet *lang_rules, ED_Buffer *buffer, Rng1S64 rng);

//- rjf: main allocation/releasing
function ED_Buffer *ED_BufferAlloc(ED_LangRuleSet *lang_rules, String8 text);
function void ED_BufferRelease(ED_Buffer *buffer);

////////////////////////////////
//~ rjf: Entities With Buffers

function ED_LangRuleSet *ED_LangRuleSetFromEntity(C_Entity *entity);

////////////////////////////////
//~ rjf: Undo/Redo Operations

function M_Arena *ED_ChangeStackArenaFromBufferDirection(ED_Buffer *buffer, ED_ChangeDirection direction);
function ED_ChangeRecord *ED_ChangeStackTopFromBufferDirection(ED_Buffer *buffer, ED_ChangeDirection direction);
function ED_ChangeRecord *ED_PushChangeRecord(ED_Buffer *buffer, ED_ChangeDirection direction, U64 glue_code);
function void ED_PushTextOp(M_Arena *arena, ED_ChangeRecord *record, TE_Range range, String8 replaced);
function void ED_PushCursorStateOp(M_Arena *arena, ED_ChangeRecord *record, TE_Point cursor, TE_Point mark);
function void ED_ApplyNextChangeRecord(ED_View *view, ED_LangRuleSet *lang_rules, ED_Buffer *buffer, ED_ChangeDirection direction);
function void ED_ClearChangeRecordStack(ED_Buffer *buffer, ED_ChangeDirection direction);

////////////////////////////////
//~ rjf: Text Layout Caches

function ED_TextLayoutCache *ED_TextLayoutCacheAlloc(void);
function void ED_TextLayoutCacheRelease(ED_TextLayoutCache *cache);
function void ED_TextLayoutCacheUpdate(ED_TextLayoutCache *cache, ED_Buffer *buffer, R_Font font, Rng1F32 view_range, F32 width_constraint);
function Rng1U64List ED_WrappedRangeListFromString(M_Arena *arena, String8 string, R_Font font, F32 width_constraint);
function U64 ED_TotalHeightFromTextLayoutCache(ED_TextLayoutCache *cache);
function S64 ED_LineNumFromPixelOffInCache_GroundTruth(F32 pixel_off, ED_TextLayoutCache *cache);
function Rng1F32 ED_PixelRangeFromLineNumInCache_GroundTruth(S64 line_num, ED_TextLayoutCache *cache);
function ED_LineLayout *ED_LineLayoutFromTLineNumInCache(S64 tline_num, ED_TextLayoutCache *cache);
function Rng1U64List ED_VRangeListFromTLineNumInCache(M_Arena *arena, S64 tline_num, ED_Buffer *buffer, ED_TextLayoutCache *cache);

////////////////////////////////
//~ rjf: Text Controls

function TE_Op ED_SingleLineOpFromInputAction(M_Arena *arena, TE_InputAction action, TE_Point cursor, TE_Point mark, S64 preferred_column, ED_Buffer *buffer, ED_TextLayoutCache *cache);
function TE_Op ED_MultiLineOpFromInputAction(M_Arena *arena, TE_InputAction action, TE_Point cursor, TE_Point mark, S64 preferred_column, ED_Buffer *buffer, ED_TextLayoutCache *cache);

////////////////////////////////
//~ rjf: Syntax Highlighting

function Vec4F32 ED_ThemeColorFromTokenKind(TM_Theme *theme, ED_TokenKind kind);

////////////////////////////////
//~ rjf: UI Helpers

function UI_InteractResult CW_CodeEdit(B32 keyboard_focused, CW_State *state, C_View *view, C_Entity *entity, Rng1S64 line_range, F32 space_for_margin, TM_Theme *theme, String8 find_string, String8 string);
function UI_InteractResult CW_CodeEditF(B32 keyboard_focused, CW_State *state, C_View *view, C_Entity *entity, Rng1S64 line_range, F32 space_for_margin, TM_Theme *theme, String8 find_string, char *fmt, ...);

////////////////////////////////
//~ rjf: Hooks

exported void *TickGlobal(M_Arena *arena, void *state_ptr, OS_EventList *events, C_CmdList *cmds);
exported void *TickView(M_Arena *arena, void *state_ptr, APP_Window *window, OS_EventList *events, C_CmdList *cmds, Rng2F32 rect);
exported void CloseView(void *state_ptr);

#endif // EDITOR_H
