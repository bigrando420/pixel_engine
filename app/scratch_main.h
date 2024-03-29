#ifndef SCRATCH_MAIN_H
#define SCRATCH_MAIN_H

#define WINDOW_X 1280
#define WINDOW_Y 720
#define PIXEL_COUNT WINDOW_X * WINDOW_Y

#define PIXEL_SCALE 10.0f
#define SIM_X (S32)(WINDOW_X / PIXEL_SCALE)
#define SIM_Y (S32)(WINDOW_Y / PIXEL_SCALE)

// Combinatorics pattern taken from https://ryanfleury.substack.com/p/ui-part-3-the-widget-building-language
typedef U32 PixelFlags;
enum
{
    PIXEL_FLAG_gravity_simple =      (1<<0),
    PIXEL_FLAG_gravity_velocity =    (1<<5),
    PIXEL_FLAG_move_diagonal =       (1<<1),
    PIXEL_FLAG_transfer_y_vel_to_x_vel_when_splat_lol =   (1<<2),
    PIXEL_FLAG_has_friction =        (1<<3),
    PIXEL_FLAG_immovable =           (1<<4),
    //   =      (1<<5),
    PIXEL_FLAG_air_lol  =      (1<<6),
    PIXEL_FLAG_dislodge_neighbours  =      (1<<7),
    PIXEL_FLAG_move_sideways_from_x_vel  =      (1<<8),
    PIXEL_FLAG_seed_random_x_velocity =      (1<<9),
};
// NOTE(randy): these are pretty poorly defined at the moment, but no matter :)


// TODO(randy): what if two types share the same features?
// I guess we'll just throw an error then, because it's impossible to derive
typedef enum PixelType
{
    PIXEL_TYPE_undefined = 0,
    // 64
    PIXEL_TYPE_air = PIXEL_FLAG_air_lol,
    // 399
    PIXEL_TYPE_sand = (
                       PIXEL_FLAG_gravity_velocity
                       | PIXEL_FLAG_move_diagonal
                       | PIXEL_FLAG_transfer_y_vel_to_x_vel_when_splat_lol
                       | PIXEL_FLAG_has_friction
                       | PIXEL_FLAG_dislodge_neighbours
                       | PIXEL_FLAG_move_sideways_from_x_vel
                       ),
    // 39
    PIXEL_TYPE_water = (PIXEL_FLAG_gravity_simple
                        | PIXEL_FLAG_move_diagonal
                        | PIXEL_FLAG_transfer_y_vel_to_x_vel_when_splat_lol
                        | PIXEL_FLAG_seed_random_x_velocity
                        | PIXEL_FLAG_move_sideways_from_x_vel
                        ),
    PIXEL_TYPE_platform = (PIXEL_FLAG_immovable),
    PIXEL_TYPE_MAX,
} PixelType;

typedef struct Pixel
{
    U32 id;
    B8 has_been_updated;
    PixelFlags flags;
    Vec2F32 vel;
    
    B8 is_resting; // just inverse of is_falling
    
    U8 vertical_move_timer;
} Pixel;

#define MAX_ACTIVE_CHUNKS 128
#define CHUNK_SIZE 64
typedef struct Chunk
{
    B8 valid; // scuffed, this'll become implicit when/if I move to a hash map
    Vec2S32 pos;
    Pixel pixels[CHUNK_SIZE][CHUNK_SIZE];
    //Vec4U8 temp_pixel_texture[CHUNK_SIZE][CHUNK_SIZE]; // It'd be cheaper to recalc every frame
    
    R_Handle texture;
} Chunk;

#define DEBUG_CHUNK_COUNT 256

typedef struct S_State S_State;
struct S_State
{
    M_Arena *permanent_arena;
    APP_Window *window;
    
    B8 is_simulating;
    
    Chunk chunks[MAX_ACTIVE_CHUNKS];
    Vec2S32 render_debug_chunks[DEBUG_CHUNK_COUNT];
    S32 render_debug_chunks_count;
    
    //Pixel pixels[SIM_Y][SIM_X];
    //Vec4U8 pixel_render_data[SIM_Y][SIM_X];
    
    U32 selected_pixel;
    Pixel *sel_pixel_this_frame;
    
    Vec2F32 camera;
    F32 camera_zoom;
};
S_State *state;

function void StepPixelSimulation();

function void SwapPixels(Pixel *from, Pixel *to, Pixel **from_pointer);
function void UpdatePixelRenderData();
function void DrawLineAtoB(Vec2S32 a, Vec2S32 b, Vec2S32* dest_arr, U32* count, U32 max_count);
function Vec2S32 GetPixelLocation(Pixel *pixel);
function Vec2S32 GetPixelAtMousePos();
function void ChunksClearActive();
function void SetTestStage();

function void ShuffleArray(S32 *array, size_t n);
function B8 CanPixelMoveTo(Pixel *src, Pixel *dest);
function void ApplyFrictionToPixel(Pixel *pixel);
function Vec4U8 *ColourAt(S32 x, S32 y);

function PixelType GetPixelType(Pixel *pixel);
function void SetPixelType(Pixel *pixel, PixelType type);

function Vec2F32 ScreenPositionToWorldPosition(Vec2F32 v);
function Rng2F32 CameraGetViewRect();
function void CameraUpdate(Vec2F32 *cam, Vec2F32 axis_input);
function void ApplyWorldTransfromOrSomeShit(Rng2F32 *rect);

// NOTE(randy): should I add chunk in front of this so it's clear that it's a sub-set?
// I think so.
function void PixelStep(Chunk *chunk, Pixel *pixel, Vec2S32 local_pos);
function Pixel *PixelAtRelativeOffset(Chunk *chunk, Pixel *relative_pixel, Vec2S32 rel_pixel_pos, Vec2S32 offset);
function Pixel *PixelAtAbsolutePos(Vec2S32 pos);

function void ChunksLoadUnloadInCameraView();
function void ChunksInRect(Rng2F32 rect, Vec2S32 *chunk_arr, U32 chunk_arr_max, U32 *count);
function Vec2S32 ChunkGetPosFromWorldPos(Vec2F32 world_pos);
function void ChunkUnload(Chunk *chunk); // rename ChunkUnloadToDisk
// ^ or maybe it should be ChunkUnloadToFile - and I just pass in the file as well
// to keep this function more atomic
function void ChunkZero(Chunk *chunk);
function Chunk *ChunkAttemptLoadFromDisk(Vec2S32 pos);
function void ChunkUpdateActive();
function void ChunkUpdate(Chunk *chunk);
function void ChunkRenderActive(DR_Bucket *bucket);
function void ChunkRender(Chunk *chunk, DR_Bucket *bucket);
// returns 0 if already exists
function Chunk *ChunkInitAtLoc(Vec2S32 pos);
function void ChunkSortActive();
function void ChunkSetDefaultPixels(Chunk *chunk);

function void UnloadWorld();
function void LoadWorld();
function void DeleteChunksOnDisk();

function void Render();

function Vec3F32 RGB8ToRGBF32(Vec3U8 rgb);
function Vec3U8 RGBF32ToRGB8(Vec3F32 rgb);


//~ NOTE(randy): Prototype controls
#define FRICTION 0.1f
#define BRUSH_SIZE 8
#define START_SIM_STRAIGHT_AWAY 0
#define DRIP 0
#define DRIP_SPEED 1
#define DISLODGE_CHANCE 1

#define BRUSH_PREVIEW 0

#define DEFAULT_CAM_ZOOM 10

#define CHUNK_RENDER_DEBUG 0
#define CHUNK_DEBUG_SPACE 1

#endif //SCRATCH_MAIN_H
