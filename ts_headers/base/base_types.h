#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#if COMPILER_CL
#include <intrin.h>
#endif

////////////////////////////////
//~ rjf: Meow Hash

#include "third_party/meow_hash_x64_aesni.h"

////////////////////////////////
//~ rjf: PCG

#define PCG_HAS_128BIT_OPS 0
#if COMPILER_CL && COMPILER_CL_YEAR < 2015
#define inline
#include "third_party/pcg/include/pcg_variants.h"
#undef inline
#else
#include "third_party/pcg/include/pcg_variants.h"
#endif

////////////////////////////////
//~ rjf: Macros

#define global         static
#define function       static
#define local_persist  static

#if ENGINE
#define engine_global exported
#else
#define engine_global imported
#endif

#if ENGINE
#define engine_function exported
#else
#define engine_function imported
#endif

#if LANG_CPP
#if OS_WINDOWS
#define exported extern "C" __declspec(dllexport)
#else
#define exported extern "C"
#endif
#else
#if OS_WINDOWS
#define exported __declspec(dllexport)
#else
#define exported
#endif
#endif

#if LANG_CPP
#if OS_WINDOWS
#define imported extern "C" __declspec(dllimport)
#else
#define imported extern "C"
#endif
#else
#if OS_WINDOWS
#define imported __declspec(dllimport)
#else
#define imported
#endif
#endif

#if COMPILER_CL
#define per_thread __declspec(thread)
#elif COMPILER_CLANG
#define per_thread __thread
#elif COMPILER_GCC
#define per_thread __thread
#endif

#if COMPILER_CL && COMPILER_CL_YEAR < 2015
# define inline_function static
#else
# define inline_function inline static
#endif

#if COMPILER_CL && COMPILER_CL_YEAR < 2015
# define this_function_name "unknown"
#else
# define this_function_name __func__
#endif

#if COMPILER_CL && COMPILER_CL_YEAR < 2015
# define snprintf _snprintf
#endif

#define fallthrough

#define MemoryCopy memcpy
#define MemoryMove memmove
#define MemorySet  memset

#define MemoryCopyStruct(d,s) do { Assert(sizeof(*(d))==sizeof(*(s))); MemoryCopy((d),(s),sizeof(*(d))); } while(0)
#define MemoryCopyArray(d,s) do{ Assert(sizeof(d)==sizeof(s)); MemoryCopy((d),(s),sizeof(s)); }while(0)

#define MemoryZero(p,s) MemorySet((p), 0, (s))
#define MemoryZeroStruct(p) MemoryZero((p), sizeof(*(p)))
#define MemoryZeroArray(a) MemoryZero((a), sizeof(a))

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))
#define IntFromPtr(p) (U64)(((U8*)p) - 0)
#define PtrFromInt(i) (void*)(((U8*)0) + i)
#define Member(S,m) ((S*)0)->m
#define OffsetOf(S,m) IntFromPtr(&Member(S,m))
#define CastFromMember(S,m,p) (S*)(((U8*)p) - OffsetOf(S,m))
#define MemberFromOffset(type, ptr, off) (type)((((U8 *)ptr)+(off)))
#define UnusedVariable(name) (void)name

#define Bytes(n)      (n)
#define Kilobytes(n)  (n << 10)
#define Megabytes(n)  (n << 20)
#define Gigabytes(n)  (((U64)n) << 30)
#define Terabytes(n)  (((U64)n) << 40)

#define Thousand(x) ((x)*1000)
#define Million(x) ((x)*1000000)
#define Billion(x) ((x)*1000000000LL)

#define HasFlag(fl,fi) ((fl)&(fi))
#define SetFlag(fl,fi) ((fl)|=(fi))
#define RemFlag(fl,fi) ((fl)&=~(fi))
#define ToggleFlag(fl,fi) ((fl)^=(fi))

#define Swap(T,a,b) do{ T t__ = a; a = b; b = t__; }while(0)

// NOTE(rjf): Linked-List Helpers
#define CheckNull(p) ((p)==0)
#define SetNull(p) ((p)=0)

#define QueuePush_NZ(f,l,n,next,zchk,zset) (zchk(f)?\
(((f)=(l)=(n)), zset((n)->next)):\
((l)->next=(n),(l)=(n),zset((n)->next)))
#define QueuePushFront_NZ(f,l,n,next,zchk,zset) (zchk(f) ? (((f) = (l) = (n)), zset((n)->next)) :\
((n)->next = (f)), ((f) = (n)))
#define QueuePop_NZ(f,l,next,zset) ((f)==(l)?\
(zset(f),zset(l)):\
(f)=(f)->next)
#define StackPush_N(f,n,next) ((n)->next=(f),(f)=(n))
#define StackPop_NZ(f,next,zchk) (zchk(f)?0:((f)=(f)->next))

#define DLLInsert_NPZ(f,l,p,n,next,prev,zchk,zset) \
(zchk(f) ? (((f) = (l) = (n)), zset((n)->next), zset((n)->prev)) :\
zchk(p) ? (zset((n)->prev), (n)->next = (f), (zchk(f) ? (0) : ((f)->prev = (n))), (f) = (n)) :\
((zchk((p)->next) ? (0) : (((p)->next->prev) = (n))), (n)->next = (p)->next, (n)->prev = (p), (p)->next = (n),\
((p) == (l) ? (l) = (n) : (0))))
#define DLLPushBack_NPZ(f,l,n,next,prev,zchk,zset) DLLInsert_NPZ(f,l,l,n,next,prev,zchk,zset)
#define DLLRemove_NPZ(f,l,n,next,prev,zchk,zset) (((f)==(n))?\
((f)=(f)->next, (zchk(f) ? (zset(l)) : zset((f)->prev))):\
((l)==(n))?\
((l)=(l)->prev, (zchk(l) ? (zset(f)) : zset((l)->next))):\
((zchk((n)->next) ? (0) : ((n)->next->prev=(n)->prev)),\
(zchk((n)->prev) ? (0) : ((n)->prev->next=(n)->next))))


#define QueuePush(f,l,n)         QueuePush_NZ(f,l,n,next,CheckNull,SetNull)
#define QueuePushFront(f,l,n)    QueuePushFront_NZ(f,l,n,next,CheckNull,SetNull)
#define QueuePop(f,l)            QueuePop_NZ(f,l,next,SetNull)
#define StackPush(f,n)           StackPush_N(f,n,next)
#define StackPop(f)              StackPop_NZ(f,next,CheckNull)
#define DLLPushBack(f,l,n)       DLLPushBack_NPZ(f,l,n,next,prev,CheckNull,SetNull)
#define DLLPushFront(f,l,n)      DLLPushBack_NPZ(l,f,n,prev,next,CheckNull,SetNull)
#define DLLInsert(f,l,p,n)       DLLInsert_NPZ(f,l,p,n,next,prev,CheckNull,SetNull)
#define DLLRemove(f,l,n)         DLLRemove_NPZ(f,l,n,next,prev,CheckNull,SetNull)

// NOTE(rjf): Clamps, Mins, Maxes
#define Min(a, b) (((a)<(b))?(a):(b))
#define Max(a, b) (((a)>(b))?(a):(b))
#define ClampTop(x, a) Min(x,a)
#define ClampBot(a, x) Max(a,x)
#define Clamp(a, x, b) (((a)>(x))?(a):((b)<(x))?(b):(x))

// NOTE(rjf): Defer Loop
#define DeferLoop(start, end) for(int _i_ = ((start), 0); _i_ == 0; _i_ += 1, (end))
#define DeferLoopChecked(begin, end) for(int _i_ = 2 * !(begin); (_i_ == 2 ? ((end), 0) : !_i_); _i_ += 1, (end))

#if OS_WINDOWS
#pragma section(".roglob", read)
#define read_only __declspec(allocate(".roglob"))
#else
// TODO(rjf): figure out if this benefit is possible on non-Windows
#define read_only
#endif

// NOTE(rjf): Synchronization Atomics
#if OS_WINDOWS
#define AtomicAdd64(ptr, v) _InterlockedExchangeAdd64((ptr), (v))
#elif
#error AtomicAdd64 is not defined for this platform.
#endif

////////////////////////////////
//~ rjf: Base-Types

typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef S8       B8;
typedef S16      B16;
typedef S32      B32;
typedef S64      B64;
typedef float    F32;
typedef double   F64;
typedef void VoidFunction(void);

////////////////////////////////
//~ rjf: Limits

read_only global U8 U8Max = 0xFF;
read_only global U8 U8Min = 0;

read_only global U16 U16Max = 0xFFFF;
read_only global U16 U16Min = 0;

read_only global U32 U32Max = 0xFFFFFFFF;
read_only global U32 U32Min = 0;

read_only global U64 U64Max = 0xFFFFFFFFFFFFFFFF;
read_only global U64 U64Min = 0;

read_only global S8 S8Max = 0x7F;
read_only global S8 S8Min = -1 - 0x7F;

read_only global S16 S16Max = 0x7FFF;
read_only global S16 S16Min = -1 - 0x7FFF;

read_only global S32 S32Max = 0x7FFFFFFF;
read_only global S32 S32Min = -1 - 0x7FFFFFFF;

read_only global S64 S64Max = 0x7FFFFFFFFFFFFFFF;
read_only global S64 S64Min = -1 - 0x7FFFFFFFFFFFFFFF;

read_only global U32 SignF32 = 0x80000000;
read_only global U32 ExponentF32 = 0x7F800000;
read_only global U32 MantissaF32 = 0x7FFFFF;

////////////////////////////////
//~ rjf: Constants

read_only global F32 F32Max = 3.4028234664e+38;
read_only global F32 F32Min = -3.4028234664e+38;
read_only global F32 F32SmallestPositive = 1.1754943508e-38;
read_only global F32 F32Epsilon = 5.96046448e-8;
read_only global F32 F32Tau = 6.28318530718f;
read_only global F32 F32Pi = 3.14159265359f;

////////////////////////////////
//~ rjf: Base Enums

typedef enum Side
{
    Side_Invalid = -1,
    Side_Min,
    Side_Max,
    Side_COUNT
}
Side;

typedef enum Axis2
{
    Axis2_X,
    Axis2_Y,
    Axis2_COUNT
}
Axis2;

typedef enum Axis3
{
    Axis3_X,
    Axis3_Y,
    Axis3_Z,
    Axis3_COUNT
}
Axis3;

typedef enum Axis4
{
    Axis4_X,
    Axis4_Y,
    Axis4_Z,
    Axis4_COUNT
}
Axis4;

typedef enum DimensionAxis
{
    Dimension_X,
    Dimension_Y,
    Dimension_Z,
    Dimension_W,
}
DimensionAxis;

typedef enum Comparison
{
    Comparison_EqualTo,
    Comparison_NotEqualTo,
    Comparison_LessThan,
    Comparison_LessThanOrEqualTo,
    Comparison_GreaterThan,
    Comparison_GreaterThanOrEqualTo,
}
Comparison;

////////////////////////////////
//~ rjf: Language Helper Types

typedef struct MemberOffset MemberOffset;
struct MemberOffset
{
    U64 v;
};

#define MemberOff(S, member) (MemberOffset){OffsetOf(S, member)}
#define MemberFromOff(ptr, type, memoff) (*(type *)((U8 *)ptr + memoff.v))

////////////////////////////////
//~ rjf: Date / Time

typedef struct DateTime DateTime;
struct DateTime
{
    U32 year; // exact year according to the international Gregorian calendar, unadjusted
    U8 mon;   // [0,11]
    U8 day;   // [0,30]
    U8 hour;  // [0,23]
    U8 min;   // [0,59]
    U8 sec;   // [0,60]
    U16 msec; // [0,999]
};

////////////////////////////////
//~ rjf: Random Number Generator

typedef struct Rng Rng;
struct Rng
{
    B8 hold_01;
    B8 hold_normal;
    F32 extra_01;
    F32 extra_normal;
    pcg32_random_t rng;
};

////////////////////////////////
//~ rjf: Assertions

#undef Assert
#define Assert(b) do { if(!(b)) { (*(volatile int *)0 = 0); } } while(0)
#define StaticAssert(c,label) U8 static_assert_##label[(c)?(1):(-1)]
#define NotImplemented Assert(!"Not Implemented")
#define InvalidPath Assert(!"Invalid Path")

////////////////////////////////
//~ rjf: Bit Patterns

engine_function U32 UpToPow2_32(U32 x);
engine_function U64 UpToPow2_64(U64 x);
engine_function U32 SearchFirstOneBit_32_BinarySearch(U32 x);
engine_function U32 SearchFirstOneBit_64_BinarySearch(U64 x);

inline_function F32
F32Inf(void)
{
    union { U32 u; F32 f; } x;
    x.u = 0x7F800000;
    return x.f;
}

inline_function F32
F32NegInf(void)
{
    union { U32 u; F32 f; } x;
    x.u = 0xFF800000;
    return x.f;
}

inline_function B32
F32IsNan(F32 f)
{
    union { U32 u; F32 f; } x;
    x.f = f;
    return ((x.u & ExponentF32) == ExponentF32) && ((x.u & MantissaF32) != 0);
}

inline_function B32
F32IsDenorm(F32 f)
{
    union { U32 u; F32 f; } x;
    x.f = f;
    return ((x.u & ExponentF32) == 0) && ((x.u & MantissaF32) != 0);
}

inline_function F32
AbsoluteValueF32(F32 f)
{
    union { U32 u; F32 f; } x;
    x.f = f;
    x.u = x.u & ~SignF32;
    return x.f;
}

////////////////////////////////
//~ rjf: Comparisons

engine_function B32 Compare_U64(U64 a, U64 b, Comparison comparison);

////////////////////////////////
//~ rjf: Random Number Generation Functions

engine_function Rng MakeRng(U64 seed1, U64 seed2);

#endif // BASE_TYPES_H

