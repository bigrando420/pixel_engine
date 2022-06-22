#ifndef BASE_TEXT_EDIT_H
#define BASE_TEXT_EDIT_H

typedef enum TE_InputActionLayer
{
    TE_InputActionLayer_SingleLine,
    TE_InputActionLayer_MultiLine,
}
TE_InputActionLayer;

typedef U32 TE_InputActionFlags;
enum
{
    TE_InputActionFlag_KeepMark            = (1<<0),
    TE_InputActionFlag_Delete              = (1<<1),
    TE_InputActionFlag_Copy                = (1<<2),
    TE_InputActionFlag_Paste               = (1<<3),
    TE_InputActionFlag_ZeroDeltaOnSelect   = (1<<4),
    TE_InputActionFlag_PickSelectSide      = (1<<5),
    TE_InputActionFlag_WordScan            = (1<<6),
    TE_InputActionFlag_CapAtLine           = (1<<7),
    TE_InputActionFlag_VisualLines         = (1<<8),
};

typedef struct TE_InputAction TE_InputAction;
struct TE_InputAction
{
    TE_InputActionLayer layer;
    TE_InputActionFlags flags;
    Vec2S32 delta;
    U32 codepoint;
};

typedef struct TE_InputActionNode TE_InputActionNode;
struct TE_InputActionNode
{
    TE_InputActionNode *next;
    TE_InputAction action;
};

typedef struct TE_InputActionList TE_InputActionList;
struct TE_InputActionList
{
    TE_InputActionNode *first;
    TE_InputActionNode *last;
    U64 count;
};

typedef struct TE_Point TE_Point;
struct TE_Point
{
    S64 line;
    S64 column;
};

typedef struct TE_Range TE_Range;
struct TE_Range
{
    TE_Point min;
    TE_Point max;
};

typedef struct TE_Op TE_Op;
struct TE_Op
{
    B32 taken;
    String8 replace;
    TE_Range range;
    TE_Point cursor;
    TE_Point mark;
    S64 new_preferred_column;
    B32 copy;
    B32 paste;
};

////////////////////////////////
//~ rjf: Points + Ranges

engine_function TE_Point TE_PointMake(S64 line, S64 column);
engine_function B32 TE_PointMatch(TE_Point a, TE_Point b);
engine_function B32 TE_PointLessThan(TE_Point a, TE_Point b);
engine_function TE_Point TE_MinPoint(TE_Point a, TE_Point b);
engine_function TE_Point TE_MaxPoint(TE_Point a, TE_Point b);
engine_function TE_Range TE_RangeMake(TE_Point min, TE_Point max);

////////////////////////////////
//~ rjf: Input Actions + Operations

engine_function B32 TE_CharIsScanBoundary(U8 c);
engine_function int TE_ScanWord(String8 string, S64 start, B32 forward);
engine_function void TE_InputActionListPush(M_Arena *arena, TE_InputActionList *list, TE_InputAction action);
engine_function String8 TE_ReplacedRangeStringFromString(M_Arena *arena, String8 string, Rng1S64 range, String8 replace);
engine_function TE_Op TE_SingleLineOpFromInputAction(M_Arena *arena, TE_InputAction action, String8 line, TE_Point cursor, TE_Point mark);
#if 0
engine_function TE_Op TE_LocalMultiLineOpFromInputAction(M_Arena *arena, TE_InputAction action, String8 prev_line, String8 line, String8 next_line, U64 line_count, TE_Point cursor, TE_Point mark, S64 preferred_column);
#endif

#endif // BASE_TEXT_EDIT_H
