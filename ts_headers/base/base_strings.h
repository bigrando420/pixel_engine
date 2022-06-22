#ifndef BASE_STRINGS_H
#define BASE_STRINGS_H

#define STB_SPRINTF_DECORATE(name) ts_stbsp_##name
#include "third_party/ts_stb_sprintf.h"

typedef struct String8 String8;
struct String8
{
    U8 *str;
    U64 size;
};

typedef struct String16 String16;
struct String16
{
    U16 *str;
    U64 size;
};

typedef struct String32 String32;
struct String32
{
    U32 *str;
    U64 size;
};

typedef struct String8Node String8Node;
struct String8Node
{
    String8Node *next;
    String8 string;
};

typedef struct String8List String8List;
struct String8List
{
    String8Node *first;
    String8Node *last;
    U64 node_count;
    U64 total_size;
};

typedef struct StringJoin StringJoin;
struct StringJoin
{
    String8 pre;
    String8 sep;
    String8 post;
};

typedef U32 MatchFlags;
enum
{
    MatchFlag_CaseInsensitive  = (1<<0),
    MatchFlag_RightSideSloppy  = (1<<1),
    MatchFlag_SlashInsensitive = (1<<2),
    MatchFlag_FindLast         = (1<<3),
    MatchFlag_KeepEmpties      = (1<<4),
};

typedef struct DecodedCodepoint DecodedCodepoint;
struct DecodedCodepoint
{
    U32 codepoint;
    U32 advance;
};

typedef enum IdentifierStyle
{
    IdentifierStyle_UpperCamelCase,
    IdentifierStyle_LowerCamelCase,
    IdentifierStyle_UpperCase,
    IdentifierStyle_LowerCase,
}
IdentifierStyle;

typedef struct ColoredRange1U64 ColoredRange1U64;
struct ColoredRange1U64
{
    Vec4F32 color;
    Rng1U64 range;
};

typedef struct ColoredRange1U64Node ColoredRange1U64Node;
struct ColoredRange1U64Node
{
    ColoredRange1U64Node *next;
    ColoredRange1U64 v;
};

typedef struct ColoredRange1U64List ColoredRange1U64List;
struct ColoredRange1U64List
{
    ColoredRange1U64Node *first;
    ColoredRange1U64Node *last;
    U64 count;
};

typedef struct FancyString FancyString;
struct FancyString
{
    Vec4F32 color;
    String8 string;
};

typedef struct FancyStringNode FancyStringNode;
struct FancyStringNode
{
    FancyStringNode *next;
    FancyString string;
};

typedef struct FancyStringList FancyStringList;
struct FancyStringList
{
    FancyStringNode *first;
    FancyStringNode *last;
    U64 count;
};

////////////////////////////////
//~ rjf: Char Functions

engine_function B32 CharIsAlpha(U8 c);
engine_function B32 CharIsAlphaUpper(U8 c);
engine_function B32 CharIsAlphaLower(U8 c);
engine_function B32 CharIsDigit(U8 c);
engine_function B32 CharIsSymbol(U8 c);
engine_function B32 CharIsSpace(U8 c);
engine_function U8  CharToUpper(U8 c);
engine_function U8  CharToLower(U8 c);
engine_function U8  CharToForwardSlash(U8 c);

////////////////////////////////
//~ rjf: String Functions

//- rjf: Helpers
engine_function U64 CalculateCStringLength(char *cstr);

//- rjf: Basic Constructors
engine_function String8 Str8(U8 *str, U64 size);
#define Str8C(cstring) Str8((U8 *)(cstring), CalculateCStringLength(cstring))
#define Str8Lit(s) Str8((U8 *)(s), sizeof(s)-1)
#define Str8LitComp(s) {(U8 *)(s), sizeof(s)-1}
engine_function String8 Str8Range(U8 *first, U8 *one_past_last);
engine_function String16 Str16(U16 *str, U64 size);
engine_function String16 Str16C(U16 *ptr);
#define Str8Struct(ptr) Str8((U8 *)(ptr), sizeof(*(ptr)))

//- rjf: Substrings
engine_function String8 Substr8(String8 str, Rng1U64 rng);
engine_function String8 Str8Skip(String8 str, U64 min);
engine_function String8 Str8Chop(String8 str, U64 nmax);
engine_function String8 Prefix8(String8 str, U64 size);
engine_function String8 Suffix8(String8 str, U64 size);

//- rjf: Matching
engine_function B32 Str8Match(String8 a, String8 b, MatchFlags flags);
engine_function U64 FindSubstr8(String8 haystack, String8 needle, U64 start_pos, MatchFlags flags);

//- rjf: Allocation
engine_function String8 PushStr8Copy(M_Arena *arena, String8 string);
engine_function String8 PushStr8FV(M_Arena *arena, char *fmt, va_list args);
engine_function String8 PushStr8F(M_Arena *arena, char *fmt, ...);
engine_function String8 PushStr8FillByte(M_Arena *arena, U64 size, U8 byte);

//- rjf: Use In Format Strings
#define Str8VArg(s) (int)(s).size, (s).str

//- rjf: String Lists
engine_function void Str8ListPushNode(String8List *list, String8Node *n);
engine_function void Str8ListPushNodeFront(String8List *list, String8Node *n);
engine_function void Str8ListPush(M_Arena *arena, String8List *list, String8 str);
engine_function void Str8ListPushFront(M_Arena *arena, String8List *list, String8 str);
engine_function void Str8ListConcat(String8List *list, String8List *to_push);
engine_function String8List StrSplit8(M_Arena *arena, String8 string, int split_count, String8 *splits);
engine_function String8 Str8ListJoin(M_Arena *arena, String8List list, StringJoin *optional_params);

//- rjf: String Re-Styling
engine_function String8 Str8Stylize(M_Arena *arena, String8 string, IdentifierStyle style, String8 separator);

////////////////////////////////
//~ rjf: Unicode Conversions

engine_function DecodedCodepoint DecodeCodepointFromUtf8(U8 *str, U64 max);
engine_function DecodedCodepoint DecodeCodepointFromUtf16(U16 *str, U64 max);
engine_function U32              Utf8FromCodepoint(U8 *out, U32 codepoint);
engine_function U32              Utf16FromCodepoint(U16 *out, U32 codepoint);
engine_function String8          Str8From16(M_Arena *arena, String16 str);
engine_function String16         Str16From8(M_Arena *arena, String8 str);
engine_function String8          Str8From32(M_Arena *arena, String32 str);
engine_function String32         Str32From8(M_Arena *arena, String8 str);

////////////////////////////////
//~ rjf: Skip/Chop Helpers

engine_function String8 Str8ChopLastPeriod(String8 str);
engine_function String8 Str8SkipLastSlash(String8 str);
engine_function String8 Str8SkipLastPeriod(String8 str);
engine_function String8 Str8ChopLastSlash(String8 str);
engine_function String8 Str8SkipWhitespace(String8 str);
engine_function String8 Str8ChopWhitespace(String8 str);
engine_function String8 Str8SkipChopWhitespace(String8 str);
engine_function String8 Str8SkipChopNewlines(String8 str);

////////////////////////////////
//~ rjf: Numeric Conversions

engine_function U64 U64FromStr8(String8 str, U32 radix);
engine_function S64 CStyleIntFromStr8(String8 str);
engine_function F64 F64FromStr8(String8 str);
engine_function String8 CStyleHexStringFromU64(M_Arena *arena, U64 x, B32 caps);

////////////////////////////////
//~ rjf: Colored Range Functions

engine_function ColoredRange1U64 ColoredRange1U64Make(Vec4F32 color, Rng1U64 range);
engine_function void ColoredRange1U64ListPush(M_Arena *arena, ColoredRange1U64List *list, ColoredRange1U64 v);

////////////////////////////////
//~ rjf: Fancy String Functions

engine_function FancyString FancyStringMake(Vec4F32 color, String8 string);
engine_function void FancyStringListPush(M_Arena *arena, FancyStringList *list, FancyString str);
engine_function FancyStringList FancyStringListFromStringAndColoredRangeList(M_Arena *arena, String8 string, ColoredRange1U64List colored_ranges, Vec4F32 default_color);

////////////////////////////////
//~ rjf: Helper For String => Enum Mapping

engine_function U64 IndexFromTableString(U64 table_size, String8 *table, String8 string, MatchFlags flags);

#endif // BASE_STRINGS_H
