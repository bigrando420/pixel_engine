#ifndef TS_ADDITIONS_H
#define TS_ADDITIONS_H

// NOTE(randy): is there an existing TS function for this?
// TODO(randy): if not, pull out into base_math
function B8 F32Compare(F32 a, F32 b, F32 epsilon)
{
    return (fabsf(a-b) < epsilon);
}
// TODO(randy): same goes for this
#define Sign(_x__) ((_x__ > 0) - (_x__ < 0))

typedef union Vec4U8 Vec4U8;
union Vec4U8
{
    struct
    {
        U8 x;
        U8 y;
        U8 z;
        U8 w;
    };
    
    struct
    {
        U8 r;
        U8 g;
        U8 b;
        U8 a;
    };
    
    struct
    {
        U8 red;
        U8 green;
        U8 blue;
        U8 alpha;
    };
    
    struct
    {
        U8 left;
        U8 up;
        U8 right;
        U8 down;
    };
    
    struct
    {
        U8 padding_[2];
        U8 dim[2];
    };
    
    U8 elements[4];
    U8 v[4];
};

#endif //TS_ADDITIONS_H
