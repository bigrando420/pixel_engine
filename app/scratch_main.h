#ifndef SCRATCH_MAIN_H
#define SCRATCH_MAIN_H

#define WINDOW_X 1280
#define WINDOW_Y 720
#define PIXEL_COUNT WINDOW_X * WINDOW_Y

#define PIXEL_SCALE 10.0f
#define SIM_X (S32)(WINDOW_X / PIXEL_SCALE)
#define SIM_Y (S32)(WINDOW_Y / PIXEL_SCALE)

typedef enum PixelType
{
    PIXEL_TYPE_boundary = -1,
    PIXEL_TYPE_air = 0,
    PIXEL_TYPE_sand,
    PIXEL_TYPE_water,
    PIXEL_TYPE_MAX,
} PixelType;

typedef struct Pixel
{
    PixelType type;
    Vec2F32 vel;
    
    B8 is_stationary;
} Pixel;

typedef struct S_State S_State;
struct S_State
{
    M_Arena *permanent_arena;
    
    B8 is_simulating;
    
    Pixel pixels[SIM_Y][SIM_X];
    U8 pixel_render_data[SIM_X * SIM_Y * 4];
    
    Vec2S32 test[64];
};
S_State *state;

function void StepPixelSimulation();
function Pixel *PixelAt(S32 x, S32 y);
function void SwapPixels(Pixel *from, Pixel *to);
function void FillPixelDataRandomly();
function void UpdatePixelRenderData();
function void DrawLineAtoB(Vec2S32 a, Vec2S32 b, Vec2S32* dest_arr, U32* count, U32 max_count);
function Vec2S32 GetPixelLocation(Pixel *pixel);
function Vec2S32 GetPixelAtMousePos(APP_Window *window);
function void SetDefaultStage();
function void StepPixel(Pixel *pixel, S32 x, S32 y); // TODO(randy): make location implicit (it already is but I'm too lazy to derive it lol)
function void ShuffleArray(S32 *array, size_t n);
function B8 AttemptDisperseWater(Pixel *water_pixel, Vec2S32 from_loc, Vec2S32 to_loc);
function B8 AttemptFallPixel(Pixel *pixel, S32 x, S32 y);
function B8 CanPixelMoveTo(Pixel *src, Pixel *dest);

// NOTE(randy): is there an existing TS function for this?
// TODO(randy): if not, pull out into base_math
function B8 F32Compare(F32 a, F32 b, F32 epsilon)
{
    return (fabsf(a-b) < epsilon);
}
// TODO(randy): same goes for this
#define Sign(_x__) ((_x__ > 0) - (_x__ < 0))

#endif //SCRATCH_MAIN_H
