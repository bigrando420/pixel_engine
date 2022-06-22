/* date = February 20th 2022 10:33 pm */

#ifndef BASE_STRING_PATTERN_H
#define BASE_STRING_PATTERN_H

typedef U32 SP_PieceFlags;
enum
{
    SP_PieceFlag_AllowMany = (1<<0),
    SP_PieceFlag_Optional  = (1<<1),
};

typedef struct SP_Atom SP_Atom;
struct SP_Atom
{
    SP_Atom *next;
    String8 string;
    U32 min_codepoint;
    U32 max_codepoint;
    B32 allow_all;
};

typedef struct SP_Piece SP_Piece;
struct SP_Piece
{
    SP_Piece *next;
    SP_Atom *first;
    SP_Atom *last;
    U64 atom_count;
    SP_PieceFlags flags;
};

typedef struct SP_Pattern SP_Pattern;
struct SP_Pattern
{
    SP_Piece *first_piece;
    SP_Piece *last_piece;
    U64 first_byte_accelerator[4];
    U64 piece_count;
};

typedef struct SP_PatternNode SP_PatternNode;
struct SP_PatternNode
{
    SP_PatternNode *next;
    SP_Pattern pattern;
};

typedef struct SP_PatternList SP_PatternList;
struct SP_PatternList
{
    SP_PatternNode *first;
    SP_PatternNode *last;
    U64 count;
};

////////////////////////////////
//~ rjf: Functions

engine_function U64 SP_PieceMatchAdvanceFromString(String8 string, SP_Piece *piece);
engine_function U64 SP_PatternMatchAdvanceFromString(String8 string, SP_Pattern pattern);
engine_function void SP_PatternListPush(M_Arena *arena, SP_PatternList *list, SP_Pattern pattern);

#endif // BASE_STRING_PATTERN_H
