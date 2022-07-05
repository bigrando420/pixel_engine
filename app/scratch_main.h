#ifndef SCRATCH_MAIN_H
#define SCRATCH_MAIN_H

#define WINDOW_X 1280
#define WINDOW_Y 720
#define PIXEL_COUNT WINDOW_X * WINDOW_Y

#define PIXEL_SCALE 10.0f
#define SIM_X (S32)(WINDOW_X / PIXEL_SCALE)
#define SIM_Y (S32)(WINDOW_Y / PIXEL_SCALE)

// TODO(randy): create a pixel clicker debugger


// MetaDesk would make this easier
// Combinatorics pattern taken from https://ryanfleury.substack.com/p/ui-part-3-the-widget-building-language
typedef U32 PixelFlags;
enum
{
    PIXEL_FLAG_gravity =             (1<<0),
    PIXEL_FLAG_move_diagonal =       (1<<1),
    PIXEL_FLAG_transfer_y_vel_to_x_vel_when_splat_lol =   (1<<2),
    PIXEL_FLAG_has_friction =        (1<<3),
    PIXEL_FLAG_immovable =           (1<<4),
    PIXEL_FLAG_fast_disperse  =      (1<<5),
    PIXEL_FLAG_air_lol  =      (1<<6),
    PIXEL_FLAG_inertia  =      (1<<7),
    PIXEL_FLAG_move_sideways_from_x_vel  =      (1<<8),
};
// NOTE(randy): these are pretty poorly defined at the moment, but no matter :)


// TODO(randy): what if two types share the same features?
// I guess we'll just throw an error then, because it's impossible to derive
typedef enum PixelType
{
    PIXEL_TYPE_undefined = 0,
    // 64
    PIXEL_TYPE_air = PIXEL_FLAG_air_lol,
    PIXEL_TYPE_sand = (
                       PIXEL_FLAG_gravity
                       | PIXEL_FLAG_move_diagonal
                       | PIXEL_FLAG_transfer_y_vel_to_x_vel_when_splat_lol
                       | PIXEL_FLAG_has_friction
                       | PIXEL_FLAG_inertia
                       | PIXEL_FLAG_move_sideways_from_x_vel
                       ),
    // 39
    PIXEL_TYPE_water = (PIXEL_FLAG_gravity
                        | PIXEL_FLAG_move_diagonal
                        //| PIXEL_FLAG_transfer_y_vel_to_x_vel_when_splat_lol
                        //| PIXEL_FLAG_fast_disperse
                        | PIXEL_FLAG_move_sideways_from_x_vel
                        ),
    PIXEL_TYPE_platform = (PIXEL_FLAG_immovable),
    PIXEL_TYPE_MAX,
} PixelType;

// NOTE(randy): need a better way of giving the pixel x velocity, especially as water, since it's always got that potential
// TODO(randy): seed it with some kind of initial velocity right away?

typedef struct Pixel
{
    U32 id;
    PixelFlags flags;
    Vec2F32 vel;
    //Vec2F32 sub_pos;
    // NOTE(randy): Not going to bother with sub_pos since it just adds wayyy too much complexity and failure points
    // can get a good enough effect just with >1 velocity-based movement
    
    B8 is_falling;
    
    U8 vertical_move_timer;
} Pixel;

typedef struct S_State S_State;
struct S_State
{
    M_Arena *permanent_arena;
    
    B8 is_simulating;
    
    Pixel pixels[SIM_Y][SIM_X];
    //U8 pixel_render_data[SIM_X * SIM_Y * 4];
    Vec4U8 pixel_render_data[SIM_Y][SIM_X];
    
    U32 selected_pixel;
    Pixel *sel_pixel_this_frame;
    // TODO(randy): can then derive pixel* by looping through each pixel
};
S_State *state;

function void StepPixelSimulation();
function Pixel *PixelAt(S32 x, S32 y);
function void SwapPixels(Pixel *from, Pixel *to, Pixel **from_pointer);
function void FillPixelDataRandomly();
function void UpdatePixelRenderData();
function void DrawLineAtoB(Vec2S32 a, Vec2S32 b, Vec2S32* dest_arr, U32* count, U32 max_count);
function Vec2S32 GetPixelLocation(Pixel *pixel);
function Vec2S32 GetPixelAtMousePos(APP_Window *window);
function void SetDefaultStage();
function void StepPixel(Pixel *pixel, S32 x, S32 y); // TODO(randy): make location implicit (it already is but I'm too lazy to derive it lol)
function void ShuffleArray(S32 *array, size_t n);
function B8 CanPixelMoveTo(Pixel *src, Pixel *dest);
function void ApplyFrictionToPixel(Pixel *pixel);
function Vec4U8 *ColourAt(S32 x, S32 y);

function PixelType GetPixelType(Pixel *pixel);
function void SetPixelType(Pixel *pixel, PixelType type);

//~ NOTE(randy): Prototype controls
#define FRICTION 0.1f
#define BRUSH_SIZE 8
#define DRIP 0
#define DRIP_SPEED 1
#define DISLODGE_CHANCE 2

#define BRUSH_PREVIEW 0

#endif //SCRATCH_MAIN_H
