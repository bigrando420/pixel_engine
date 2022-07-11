#include "engine_bundles/engine_full.h"

#include "ts_additions.h"

#include "os/os_entry_point.h"
#include "os/os_entry_point.c"

#include "scratch_main.h"

////////////////////////////////
//~ rjf: Main Calls

function S_State *
S_Open(APP_Window *window)
{
    M_Arena *arena = M_ArenaAllocDefault();
    state = PushStruct(arena, S_State);
    state->permanent_arena = arena;
    
    state->camera_zoom = DEFAULT_CAM_ZOOM;
    state->is_simulating = START_SIM_STRAIGHT_AWAY;
    
    SetDefaultStage();
    
    ChunkInitAtLoc(V2S32(0, -1));
    ChunkInitAtLoc(V2S32(0, 0));
    ChunkInitAtLoc(V2S32(0, -2));
    Chunk *test = ChunkInitAtLoc(V2S32(0, 1));
    //ChunkInitAtLoc(V2S32(1, 0));
    //ChunkInitAtLoc(V2S32(3, 5));
    
    ChunkUnload(test);
    ChunkLoadAt(V2S32(0, 1));
    
    ChunkSortActive();
    
    return state;
}

function void
S_Close(APP_Window *window, S_State *state)
{
    M_ArenaRelease(state->permanent_arena);
}

B8 l_key_flip = 0;
S32 update_count = 0;
function void
S_Update(APP_Window *window, OS_EventList *events, S_State *state)
{
    // Scuffed key_down stuff
    local_persist B8 a_key_down = 0;
    if (OS_KeyPress(events, window->handle, OS_Key_A, 0))
    {
        a_key_down = 1;
    }
    if (OS_KeyRelease(events, window->handle, OS_Key_A, 0))
    {
        a_key_down = 0;
    }
    local_persist B8 d_key_down = 0;
    if (OS_KeyPress(events, window->handle, OS_Key_D, 0))
    {
        d_key_down = 1;
    }
    if (OS_KeyRelease(events, window->handle, OS_Key_D, 0))
    {
        d_key_down = 0;
    }
    local_persist B8 w_key_down = 0;
    if (OS_KeyPress(events, window->handle, OS_Key_W, 0))
    {
        w_key_down = 1;
    }
    if (OS_KeyRelease(events, window->handle, OS_Key_W, 0))
    {
        w_key_down = 0;
    }
    local_persist B8 s_key_down = 0;
    if (OS_KeyPress(events, window->handle, OS_Key_S, 0))
    {
        s_key_down = 1;
    }
    if (OS_KeyRelease(events, window->handle, OS_Key_S, 0))
    {
        s_key_down = 0;
    }
    
    //~
    // NOTE(randy): Camera input & update
    {
        if (OS_KeyPress(events, window->handle, OS_Key_Equal, 0))
        {
            if (state->camera_zoom <= 1.01f)
                state->camera_zoom *= 2.0f;
            else
                state->camera_zoom += 1.0f;
        }
        else if (OS_KeyPress(events, window->handle, OS_Key_Minus, 0))
        {
            if (state->camera_zoom <= 1.01f)
                state->camera_zoom /= 2.0f;
            else
                state->camera_zoom -= 1.0f;
        }
        
        F32 temp_camera_speed = 10.0f;
        
        Vec2F32 axis_input = {0};
        
        if (a_key_down)
            axis_input.x = temp_camera_speed;
        else if (d_key_down)
            axis_input.x = -temp_camera_speed;
        
        if (w_key_down)
            axis_input.y = temp_camera_speed;
        else if (s_key_down)
            axis_input.y = -temp_camera_speed;
        
        CameraUpdate(&state->camera, axis_input);
    }
    
    /* 
// TODO(randy): selection logic
        for (int y = 0; y < SIM_Y; y++)
            for (int x = 0; x < SIM_X; x++)
        {
            Pixel *px = PixelAt(x, y);
            if (px->id == state->selected_pixel)
            {
                state->sel_pixel_this_frame = px;
            }
        }
     */
    
    if (OS_KeyPress(events, window->handle, OS_Key_L, 0))
    {
        l_key_flip = !l_key_flip;
    }
    
    local_persist PixelType selected_type = PIXEL_TYPE_sand;
    if (OS_KeyPress(events, window->handle, OS_Key_1, 0))
    {
        selected_type = PIXEL_TYPE_sand;
    }
    if (OS_KeyPress(events, window->handle, OS_Key_2, 0))
    {
        selected_type = PIXEL_TYPE_water;
    }
    if (OS_KeyPress(events, window->handle, OS_Key_7, 0))
    {
        selected_type = PIXEL_TYPE_air;
    }
    if (OS_KeyPress(events, window->handle, OS_Key_4, 0))
    {
        selected_type = PIXEL_TYPE_platform;
    }
    if (OS_KeyPress(events, window->handle, OS_Key_0, 0))
    {
        selected_type = 0;
    }
    
    if (OS_KeyPress(events, window->handle, OS_Key_MouseLeft, 0))
    {
#if BRUSH_SIZE == 1
        Vec2S32 mouse = GetPixelAtMousePos(window);
        Pixel *pixel = PixelAt(mouse.x, mouse.y);
        SetPixelType(pixel, selected_type);
#else
        S32 size = BRUSH_SIZE;
        for (int y = size / -2; y < size / 2; y++)
            for (int x = size / -2; x < size / 2; x++)
        {
            Vec2S32 mouse = GetPixelAtMousePos(window);
            // TODO(randy): absolute pixel at
            /* 
                        Pixel *pixel = PixelAt(mouse.x + x, mouse.y + y);
                        SetPixelType(pixel, selected_type);
             */
        }
#endif
    }
    
    if (OS_KeyPress(events, window->handle, OS_Key_MouseRight, 0))
    {
        Vec2S32 mouse = GetPixelAtMousePos(window);
        // TODO(randy): 
        /* 
                Pixel *pixel = PixelAt(mouse.x, mouse.y);
                
                state->selected_pixel = pixel->id;
         */
    }
    
    if(OS_KeyPress(events, window->handle, OS_Key_J, 0))
    {
        UnloadWorld();
    }
    if(OS_KeyPress(events, window->handle, OS_Key_K, 0))
    {
        LoadWorld();
    }
    
    if(OS_KeyPress(events, window->handle, OS_Key_Z, 0))
    {
        state->is_simulating = !state->is_simulating;
    }
    
    if (OS_KeyPress(events, window->handle, OS_Key_X, 0))
    {
        StepPixelSimulation();
    }
    
    if (OS_KeyPress(events, window->handle, OS_Key_R, 0))
    {
        // FillPixelDataRandomly();
    }
    
    if (OS_KeyPress(events, window->handle, OS_Key_C, 0))
    {
        SetDefaultStage();
    }
    
    local_persist B8 drip_override = DRIP;
    if (OS_KeyPress(events, window->handle, OS_Key_D, 0))
    {
        drip_override = !drip_override;
    }
#if DRIP
    //@drip
    // yooooo we got the drip drip
    // yuh yuh
    Pixel *top_middle_px = &state->chunks[2].pixels[CHUNK_SIZE-1][CHUNK_SIZE / 2];
    if (drip_override &&
        update_count % DRIP_SPEED == 0)
    {
        top_middle_px->flags = PIXEL_TYPE_sand;
        //drip_override = 0;
    }
#endif
    
    
    if (state->is_simulating)
        StepPixelSimulation();
    
    /* 
        
        {
            Vec2F32 mouse = OS_MouseFromWindow(window->handle);
            
            Vec2S64 size = V2S64(SIM_X,
                                 SIM_Y);
            
    #if BRUSH_PREVIEW
            // TODO(randy): just render the outline instead of full square
            {
                S32 size = BRUSH_SIZE;
                for (int y = size / -2; y < size / 2; y++)
                    for (int x = size / -2; x < size / 2; x++)
                {
                    Vec2S32 mouse = GetPixelAtMousePos(window);
                    Vec4U8 *col = ColourAt(mouse.x + x, SIM_Y-mouse.y + y);
                    if (col)
                    {
                        col->r = 230;
                        col->g = 230;
                        col->b = 230;
                        col->a = 255;
                    }
                }
            }
    #endif
     */
    
    //@render
    DR_Bucket bucket = {0};
    
    Rng2F32 window_rect = OS_ClientRectFromWindow(window->handle);
    
    Rng2F32 render_rect = R2F32(V2F32(100.0f, 100.0f),
                                V2F32(200.0f,
                                      200.0f));
    
    ApplyWorldTransfromOrSomeShit(&render_rect);
    
    DR_Rect(&bucket, render_rect, V4F32(1.0f, 1.0f, 1.0f, 1.0f));
    
    ChunkRenderActive(&bucket);
    
    DR_Submit(window->window_equip, bucket.cmds);
    
    update_count++;
}

////////////////////////////////
//~ rjf: Entry Points

function void
APP_Repaint(void)
{
    //- rjf: begin frame
    APP_BeginFrame();
    CFG_BeginFrame();
    OS_EventList events = OS_GetEvents(APP_FrameArena());
    
    //- rjf: update all windows
    for(APP_Window *window = app_state->first_window; window != 0; window = window->next)
    {
        if(window->initialized == 0)
        {
            window->initialized = 1;
            window->user.data = window->user.hooks.Open(window);
            OS_WindowFirstPaint(window->handle);
        }
        app_state->r_backend.Start(app_state->r_os_equip, window->window_equip, Vec2S64FromVec(Dim2F32(OS_ClientRectFromWindow(window->handle))));
        window->user.hooks.Update(window, &events, window->user.data);
        app_state->r_backend.Finish(app_state->r_os_equip, window->window_equip);
    }
    
    //- rjf: handle windows closing
    for(OS_Event *e = events.first; e != 0; e = e->next)
    {
        if(e->kind == OS_EventKind_WindowClose)
        {
            APP_Window *window = APP_WindowFromHandle(e->window);
            window->user.hooks.Close(window, window->user.data);
            APP_WindowClose(window);
        }
    }
    
    //- rjf: end frame
    CFG_EndFrame();
    APP_EndFrame();
}

function void
APP_EntryPoint(void)
{
    //- rjf: init
    TCTX tctx = MakeTCTX();
    SetTCTX(&tctx);
    OS_Init();
    OS_InitGfx(APP_Repaint);
    APP_Init();
    DR_Init(app_state->r_backend, app_state->r_os_equip);
    C_Init();
    VIN_Init();
    CFG_Init();
    
    //- rjf: open window
    APP_WindowUserHooks hooks =
    {
        (APP_WindowUserOpenFunction *)S_Open,
        (APP_WindowUserCloseFunction *)S_Close,
        (APP_WindowUserUpdateFunction *)S_Update,
    };
    APP_WindowOpen(Str8Lit("[Telescope] Randall's Scratch"), V2S64(WINDOW_X, WINDOW_Y), hooks);
    
    //- rjf: loop
    for(;!APP_Quit();)
    {
        APP_Repaint();
    }
    
    //- rjf: quit
    C_Quit();
}

// TODO(randy): maybe make the pixel getting based off of a local pixel?
Pixel boundary_pixel = {PIXEL_TYPE_platform}; // lol
/* 
function Pixel *PixelAt(S32 x, S32 y)
{
    if (x < 0 || x >= SIM_X ||
        y < 0 || y >= SIM_Y)
        return &boundary_pixel;
    
    return &boundary_pixel;
    //return &state->pixels[y % SIM_Y][x % SIM_X];
}
 */

function Vec4U8 *ColourAt(S32 x, S32 y)
{
    if (x < 0 || x >= SIM_X ||
        y < 0 || y >= SIM_Y)
        return 0;
    
    return 0;
    //return &state->pixel_render_data[y][x];
}

function void SwapPixels(Pixel *from, Pixel *to, Pixel **from_pointer)
{
    Pixel t = *to;
    *to = *from;
    *from = t;
    
    *from_pointer = to;
}

/* 
function void FillPixelDataRandomly()
{
    for (int y = 0; y < SIM_Y; y++)
        for (int x = 0; x < SIM_X; x++)
    {
        Pixel *pixel = &state->pixels[y][x];
        //pixel->type = (PixelType)(rand() % 3);
    }
}
 */


function void StepPixelSimulation()
{
    ChunkUpdateActive();
}

// NOTE(randy): "adapted" from https://gist.github.com/bert/1085538
function void DrawLineAtoB(Vec2S32 a, Vec2S32 b, Vec2S32* dest_arr, U32* dest_count, U32 max_count)
{
    int x0 = a.x;
    int y0 = a.y;
    int x1 = b.x;
    int y1 = b.y;
    
    int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1; 
    int err = dx + dy, e2;
    
    S32 count = 0;
    
    while (count + 1 < max_count)
    {
        dest_arr[(count)++] = V2S32(x0, y0);
        
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
    
    *dest_count = count;
}

function Vec2S32 GetPixelLocation(Pixel *pixel)
{
    /* 
        for (int y = 0; y < SIM_Y; y++)
            for (int x = 0; x < SIM_X; x++)
        {
            if (&state->pixels[y][x] == pixel)
                return V2S32(x, y);
        }
     */
    
    return V2S32(-1, -1);
}

function Vec2S32 GetPixelAtMousePos(APP_Window *window)
{
    Vec2S32 loc = V2S32(OS_MouseFromWindow(window->handle).x,
                        WINDOW_Y - OS_MouseFromWindow(window->handle).y);
    return V2S32(loc.x / PIXEL_SCALE, loc.y / PIXEL_SCALE);
}

function void SetDefaultStage()
{
    /* 
        MemorySet(state->pixels, 0, sizeof(state->pixels));
        
        for (int y = 0; y < SIM_Y; y++)
            for (int x = 0; x < SIM_X; x++)
        {
            Pixel* px = PixelAt(x, y);
            px->id = x + y * SIM_X + 1;
            
            if (x >= SIM_X / 10 && x <= SIM_X - SIM_X / 10 &&
                y >= 20 && y <= 30)
            {
                SetPixelType(px, PIXEL_TYPE_platform);
            }
            else
            {
                SetPixelType(px, PIXEL_TYPE_air);
            }
        }
     */
}

/* 
function Vec2S32 PixelDeriveLocalLocation(Chunk *chunk, Pixel *pixel)
{
    // TODO(randy): this logic ain't right, fix it
    S32 index = (pixel - (Pixel*)chunk->pixels) / sizeof(Pixel);
    S32 x = index % CHUNK_SIZE;
    S32 y = floorf((F32)index / (F32)CHUNK_SIZE);
    
    return V2S32(x, y);
}
 */

function Pixel *PixelAtAbsolutePos(Vec2S32 pos)
{
    Vec2S32 chunk_loc = V2S32(floorf((F32)pos.x / (F32)CHUNK_SIZE),
                              floorf((F32)pos.y / (F32)CHUNK_SIZE));
    
    for (int i = 0; i < MAX_ACTIVE_CHUNKS; i++)
    {
        Chunk *chunk = &state->chunks[i];
        if (chunk->valid &&
            chunk->pos.x == chunk_loc.x &&
            chunk->pos.y == chunk_loc.y)
        {
            Vec2S32 a = V2S32(abs(pos.x) % CHUNK_SIZE,
                              abs(pos.y) % CHUNK_SIZE);
            
            // NOTE(randy): invert if it's negative, to get the correct index
            if (pos.x < 0)
            {
                a.x = CHUNK_SIZE - a.x;
            }
            if (pos.y < 0)
            {
                a.y = CHUNK_SIZE - a.y;
            }
            
            return &chunk->pixels[a.y][a.x];
        }
    }
    
    // NotImplemented;
    // NOTE(randy): chunk doesn't exist
    // Do I just make it standard that the return pixel is null?
    return 0;
}

function Pixel *PixelAtRelativeOffset(Chunk *chunk, Pixel *relative_pixel, Vec2S32 rel_pixel_pos, Vec2S32 offset)
{
    if (rel_pixel_pos.x + offset.x >= 0 &&
        rel_pixel_pos.x + offset.x < CHUNK_SIZE &&
        rel_pixel_pos.y + offset.y >= 0 &&
        rel_pixel_pos.y + offset.y < CHUNK_SIZE)
    {
        return &chunk->pixels[rel_pixel_pos.y + offset.y][rel_pixel_pos.x + offset.x];
    }
    else
    {
        // it's in another chunk
        
        // get the absolute position
        Vec2S32 pixel_at_abs_pos = V2S32(chunk->pos.x * CHUNK_SIZE + rel_pixel_pos.x + offset.x,
                                         chunk->pos.y * CHUNK_SIZE + rel_pixel_pos.y + offset.y);
        
        Pixel *pxl = PixelAtAbsolutePos(pixel_at_abs_pos);
        
        return pxl;
    }
}

function void PixelClear(Pixel *pxl)
{
    U32 id = pxl->id;
    
    Pixel pxl_clear = {0};
    pxl_clear.id = id;
    SetPixelType(&pxl_clear, PIXEL_TYPE_air);
    
    *pxl = pxl_clear;
}

function void PixelStep(Chunk *chunk, Pixel *pixel, Vec2S32 local_pos)
{
    if (pixel->has_been_updated)
        return;
    
    pixel->has_been_updated = 1;
    
    B8 has_moved = 0;
    
    B8 is_falling_check = ((pixel->flags & PIXEL_FLAG_inertia) ? pixel->is_falling : 1);
    
    //~ Gravity
    if (pixel->flags & PIXEL_FLAG_gravity)
    {
        const F32 gravity = 0.4f;
        pixel->vel.y -= gravity;
        
        const max_speed = 10.0f;
        pixel->vel.y = Min(pixel->vel.y, max_speed);
        
        Vec2S32 from_loc = local_pos;
        Vec2S32 to_loc = V2S32(local_pos.x, local_pos.y - 1 + pixel->vel.y);
        //Vec2S32 to_loc = V2S32(local_pos.x, local_pos.y - 5);
        
        Vec2S32 inter_pixels[16];
        U32 count = 0;
        DrawLineAtoB(from_loc, to_loc, inter_pixels, &count, 16);
        
        Pixel *last_good_pixel = 0;
        for (int i = 1; i < count; i++)
        {
            Vec2S32 pos = inter_pixels[i];
            Pixel *inter_pixel = PixelAtRelativeOffset(chunk, pixel, local_pos, V2S32(pos.x - local_pos.x,
                                                                                      pos.y - local_pos.y));
            
            if (!inter_pixel)
            {
                // NOTE(randy): moved into invalid chunk, yeet it.
                PixelClear(pixel);
                return;
            }
            
            if (CanPixelMoveTo(pixel, inter_pixel))
            {
                last_good_pixel = inter_pixel;
            }
            else
            {
                break;
            }
        }
        
        if (last_good_pixel)
        {
            SwapPixels(pixel, last_good_pixel, &pixel);
            has_moved = 1;
        }
        else
        {
            if ((pixel->flags & PIXEL_FLAG_transfer_y_vel_to_x_vel_when_splat_lol) &&
                !F32Compare(pixel->vel.y, 0.0f, 0.01f) &&
                F32Compare(pixel->vel.x, 0.0f, 0.01f))
            {
                S32 sign;
                if (!F32Compare(pixel->vel.x, 0.0f, 0.01f))
                    sign = Sign(pixel->vel.x);
                else
                    sign = rand() % 2 ? -1 : 1;
                
                F32 div_amount = 3.0f;// + ((F32)rand() / (F32)RAND_MAX * 2);
                
                pixel->vel.x = fabsf(pixel->vel.x) + (fabsf(pixel->vel.y) / div_amount);
                pixel->vel.x *= sign;
                pixel->vel.y = 0.0f;
            }
        }
    }
    
    /*
            //~ Move diagonally
            if (!has_moved &&
                is_falling_check &&
                (pixel->flags & PIXEL_FLAG_move_diagonal))
            {
                B8 has_x_vel = !F32Compare(pixel->vel.x, 0.0f, 0.01f);
                B8 check_left_first;
                if (has_x_vel)
                {
                    check_left_first = Sign(pixel->vel.x) == -1;
                }
                else
                {
                    check_left_first = rand() % 2 == 0;
                }
                
                Pixel *left = PixelAt(x-1, y);
                Pixel *right = PixelAt(x+1, y);
                Pixel *down_left = PixelAt(x-1, y-1);
                Pixel *down_right = PixelAt(x+1, y-1);
                
                if (check_left_first &&
                    CanPixelMoveTo(pixel, left) &&
                    CanPixelMoveTo(pixel, down_left))
                {
                    SwapPixels(pixel, down_left, &pixel);
                    has_moved = 1;
                }
                else if (CanPixelMoveTo(pixel, right) &&
                         CanPixelMoveTo(pixel, down_right))
                {
                    SwapPixels(pixel, down_right, &pixel);
                    has_moved = 1;
                }
                else if (!check_left_first &&
                         CanPixelMoveTo(pixel, left) &&
                         CanPixelMoveTo(pixel, down_left))
                {
                    SwapPixels(pixel, down_left, &pixel);
                    has_moved = 1;
                }
            }
            
            if ((pixel->flags & PIXEL_FLAG_seed_random_x_velocity) &&
                F32Compare(pixel->vel.x, 0.0f, 0.01f) && GetPixelType(pixel) == PIXEL_TYPE_water)
            {
                pixel->vel.x = 5.0f + (rand() % 2 == 0 ? -1 : 1) * (rand() % 2);
                pixel->vel.x *= (rand() % 2 == 0 ? 1 : -1);
            }
            
            
            
            //~ Move sideways
            // TODO(randy): Need a better way of figuring out if the water pixel is level
            // Right now it's a bit icky, but good enough
            if (has_moved)
                pixel->vertical_move_timer = 20;
            else if (pixel->vertical_move_timer != 0)
                pixel->vertical_move_timer--;
            
            B8 has_pixel_reached_level = pixel->vertical_move_timer == 0;
            B8 skip_sideways_update = (has_pixel_reached_level ? !(update_count % 10 == 0) : 0);
            
            if (!skip_sideways_update &&
                !has_moved &&
                is_falling_check &&
                !F32Compare(pixel->vel.x, 0.0f, 0.01f) &&
                (pixel->flags & PIXEL_FLAG_move_sideways_from_x_vel))
            {
                Vec2S32 from_loc = V2S32(x, y);
                Vec2S32 to_loc = V2S32(x + roundf(pixel->vel.x), y);
                
                Vec2S32 pixel_path[16];
                U32 count = 0;
                DrawLineAtoB(from_loc, to_loc, pixel_path, &count, 16);
                
                Pixel *last_good_pixel = 0;
                Pixel *next_pixel = 0;
                for (int i = 1; i < count; i++)
                {
                    Vec2S32 pos = pixel_path[i];
                    next_pixel = PixelAt(pos.x, pos.y);
                    
                    if (CanPixelMoveTo(pixel, next_pixel) ||
                        (has_pixel_reached_level ? 0 : pixel->flags == next_pixel->flags))
                    {
                        last_good_pixel = next_pixel;
                    }
                    else
                    {
                        break;
                    }
                }
                
                if (last_good_pixel)
                {
                    SwapPixels(pixel, last_good_pixel, &pixel);
                    has_moved = 1;
                }
                else
                {
                    pixel->vel.x *= -1;
                }
                
                if (pixel->flags & PIXEL_FLAG_has_friction)
                    ApplyFrictionToPixel(pixel);
            }
            
            //~ Inertia
            pixel->is_falling = has_moved;//!F32Compare(pixel->vel.x, 0.0f, 0.1f);
            
            if (pixel->is_falling &&
                (pixel->flags & PIXEL_FLAG_inertia))
            {
                Pixel *left = PixelAt(x-1, y);
                Pixel *right = PixelAt(x+1, y);
                Pixel *down_left = PixelAt(x-1, y-1);
                Pixel *down_right = PixelAt(x+1, y-1);
                Pixel *top_left = PixelAt(x-1, y+1);
                Pixel *top_right = PixelAt(x+1, y+1);
                
                if (rand() % DISLODGE_CHANCE == 0)
                {
                    left->is_falling = 1;
                    right->is_falling = 1;
                    down_left->is_falling = 1;
                    down_right->is_falling = 1;
                    top_left->is_falling = 1;
                    top_right->is_falling = 1;
                }
            }
         */
    
    
}

function B8 CanPixelMoveTo(Pixel *src, Pixel *dest)
{
    switch (GetPixelType(src))
    {
        case PIXEL_TYPE_sand:
        return (GetPixelType(dest) == PIXEL_TYPE_air ||
                GetPixelType(dest) == PIXEL_TYPE_water);
        
        case PIXEL_TYPE_water:
        return (GetPixelType(dest) == PIXEL_TYPE_air);
    }
    
    return 0;
}

function void ApplyFrictionToPixel(Pixel *pixel)
{
    const F32 friction = FRICTION + ((F32)rand() / (F32)RAND_MAX / 5.0f);
    
    S32 sign = Sign(pixel->vel.x);
    F32 sub = fabsf(pixel->vel.x) - friction;
    
    if ((S32)Sign(sub * sign) != sign)
    {
        pixel->vel.x = 0.0f;
    }
    else
    {
        pixel->vel.x = sub * sign;
    }
}

// NOTE(randy): taken from https://stackoverflow.com/questions/6127503/shuffle-array-in-c
function void ShuffleArray(int *array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

function PixelType GetPixelType(Pixel *pixel)
{
    return pixel->flags;
}

function void SetPixelType(Pixel *pixel, PixelType type)
{
    pixel->flags = type;
}

function void CameraUpdate(Vec2F32 *cam, Vec2F32 axis_input)
{
    *cam = Add2F32(*cam, axis_input);
}

function void ChunkSetDefaultPixels(Chunk *chunk)
{
    for (int y = 0; y < CHUNK_SIZE; y++)
        for (int x = 0; x < CHUNK_SIZE; x++)
    {
        Pixel *pxl = &chunk->pixels[y][x];
        SetPixelType(pxl, PIXEL_TYPE_air);
        pxl->id = x + y * CHUNK_SIZE;
    }
}

function Chunk *ChunkLoadAt(Vec2S32 pos)
{
    char chunk_fname[256];
    sprintf(chunk_fname, "%i,%i.chunk", pos.x, pos.y);
    char chunk_path[256];
    sprintf(chunk_path, "/balls/%s", chunk_fname);
    
    FILE *f = fopen(chunk_path, "rb");
    if (!f)
        return 0;
    
    Chunk *chunk = ChunkInitAtLoc(pos);
    fread(&chunk->pos, sizeof(chunk->pos), 1, f);
    fread(chunk->pixels, sizeof(chunk->pixels), 1, f);
    
    fclose(f);
    return chunk;
}

function void ChunkUnload(Chunk *chunk)
{
    Assert(chunk && chunk->valid);
    
    OS_MakeDirectory(Str8C("/balls"));
    
    // NOTE(randy): chunk path at POSX,POSY.chunk
    char chunk_fname[256];
    sprintf(chunk_fname, "%i,%i.chunk", chunk->pos.x, chunk->pos.y);
    char chunk_path[256];
    sprintf(chunk_path, "/balls/%s", chunk_fname);
    
    // NOTE(randy): CHUNK DATA SPEC
    FILE *f = fopen(chunk_path, "wb");
    fwrite(&chunk->pos, sizeof(chunk->pos), 1, f); // implicit in name but eh, who cares
    fwrite(chunk->pixels, sizeof(chunk->pixels), 1, f);
    fclose(f);
    
    MemoryZeroStruct(chunk);
}

function void UnloadWorld()
{
    for (int i = 0; i < MAX_ACTIVE_CHUNKS; i++)
    {
        Chunk *chunk = &state->chunks[i];
        if (chunk->valid)
            ChunkUnload(chunk);
    }
}

function void LoadWorld()
{
    // TODO(randy): get chunks in viewport or some shit
    
    Vec2S32 list_of_chunks[] =
    {
        V2S32(0, -1),
        V2S32(0, 0),
        V2S32(0, -2),
        V2S32(0, 1),
        V2S32(1, 0),
        V2S32(3, 5),
    };
    S32 count = sizeof(list_of_chunks) / sizeof(Vec2S32);
    for (int i = 0; i < count; i++)
    {
        ChunkLoadAt(list_of_chunks[i]);
    }
}

function Chunk *ChunkInitAtLoc(Vec2S32 pos)
{
    for (int i = 0; i < MAX_ACTIVE_CHUNKS; i++)
    {
        Chunk *next_chunk = &state->chunks[i];
        if (next_chunk->valid &&
            next_chunk->pos.x == pos.x &&
            next_chunk->pos.y == pos.y)
        {
            // already exists
            Assert(0);
            return 0;
        }
    }
    
    // NOTE(randy): find next valid chunk, init it with this location
    for (int i = 0; i < MAX_ACTIVE_CHUNKS; i++)
    {
        Chunk *chunk = &state->chunks[i];
        if (!chunk->valid)
        {
            chunk->valid = 1;
            chunk->pos = pos;
            ChunkSetDefaultPixels(chunk);
            
            return chunk;
        }
    }
    
    return 0;
}

function void ChunkSortActive()
{
    // NOTE(randy): Sort chunks so it's from the bottom up
    for (int i = 0; i < MAX_ACTIVE_CHUNKS - 1; i++)
    {
        for (int j = 0; j < MAX_ACTIVE_CHUNKS - 1 - i; j++)
        {
            Chunk *a = &state->chunks[j];
            Chunk *b = &state->chunks[j + 1];
            
            if (a->pos.y > b->pos.y)
            {
                Chunk temp = *a;
                *a = *b;
                *b = temp;
            }
        }
    }
}

function void ChunkUpdateActive()
{
    // clear updates from last frame
    for (int i = 0; i < MAX_ACTIVE_CHUNKS; i++)
    {
        Chunk *chunk = &state->chunks[i];
        if (chunk->valid)
        {
            for (int y = 0; y < CHUNK_SIZE; y++)
                for (int x = 0; x < CHUNK_SIZE; x++)
            {
                Pixel *pixel = &chunk->pixels[y][x];
                pixel->has_been_updated = 0;
            }
        }
    }
    
    for (int i = 0; i < MAX_ACTIVE_CHUNKS; i++)
    {
        Chunk *next_chunk = &state->chunks[i];
        if (next_chunk->valid)
        {
            ChunkUpdate(next_chunk);
        }
    }
}

function void ChunkUpdate(Chunk *chunk)
{
    // randomise x
    S32 x_pixels[CHUNK_SIZE];
    for (int i = 0; i < CHUNK_SIZE; i++)
    {
        x_pixels[i] = i;
    }
    ShuffleArray(x_pixels, CHUNK_SIZE);
    
    // step all pixels
    for (int y = 0; y < CHUNK_SIZE; y++)
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            S32 x_pos = x_pixels[x];
            Pixel *pixel = &chunk->pixels[y][x_pos];
            PixelStep(chunk, pixel, V2S32(x_pos, y));
        }
    }
}


function void ChunkRenderActive(DR_Bucket *bucket)
{
    for (int i = 0; i < MAX_ACTIVE_CHUNKS; i++)
    {
        Chunk *next_chunk = &state->chunks[i];
        if (next_chunk->valid)
        {
            ChunkRender(next_chunk, bucket);
        }
    }
}

function void ChunkRender(Chunk *chunk, DR_Bucket *bucket)
{
    Vec4U8 chunk_texture_data[CHUNK_SIZE][CHUNK_SIZE];
    
    for (int y = 0; y < CHUNK_SIZE; y++)
        for (int x = 0; x < CHUNK_SIZE; x++)
    {
        Vec4U8 *col = &chunk_texture_data[y][x];
        Pixel *px = &chunk->pixels[CHUNK_SIZE-y-1][x];
        
        /* 
                col->r = (F32)x / (F32)CHUNK_SIZE * 255;
                col->g = (F32)y / (F32)CHUNK_SIZE * 255;
                col->b = 0;
                col->a = 255;
         */
        
        /* 
                if (px->id == state->selected_pixel)
                {
                    col->r = 255;
                    col->g = 100;
                    col->b = 100;
                    col->a = 255;
                    continue;
                }
         */
        
        switch (GetPixelType(px))
        {
            case PIXEL_TYPE_platform:
            {
                col->r = 145;
                col->g = 139;
                col->b = 134;
                col->a = 255;
            } break;
            
            case PIXEL_TYPE_air:
            {
                col->r = 180;
                col->g = 203;
                col->b = 240;
                col->a = 255;
            } break;
            
            case PIXEL_TYPE_sand:
            {
                col->r = 242;
                col->g = 216;
                col->b = 145;
                col->a = 255;
            } break;
            
            case PIXEL_TYPE_water:
            {
                col->r = 74;
                col->g = 147;
                col->b = 244;
                col->a = 255;
            } break;
            
            case PIXEL_TYPE_undefined:
            default:
            {
                col->r = 255;
                col->g = 0;
                col->b = 0;
                col->a = 255;
            } break;
        }
    }
    
    // NOTE(randy): Create a texture and fill it with the pixel data
    Vec2S64 texture_size = V2S64(CHUNK_SIZE, CHUNK_SIZE);
    R_Handle texture = dr_state->backend.ReserveTexture2D(dr_state->os_eqp,
                                                          texture_size,
                                                          R_Texture2DFormat_RGBA8);
    dr_state->backend.FillTexture2D(dr_state->os_eqp,
                                    texture,
                                    R2S64(V2S64(0, 0),
                                          texture_size),
                                    Str8((U8*)chunk_texture_data, 
                                         sizeof(chunk_texture_data)));
    
    F32 chunk_render_size = texture_size.x * state->camera_zoom;
    
    Rng2F32 render_rect = R2F32(V2F32(0.0f, 0.0f),
                                V2F32(chunk_render_size,
                                      chunk_render_size));
    
    render_rect = Shift2F32(render_rect, V2F32(chunk->pos.x * chunk_render_size * 1.005f,
                                               chunk->pos.y * chunk_render_size * 1.005f));
    
    ApplyWorldTransfromOrSomeShit(&render_rect);
    
    // TODO(randy): it's being updated twice?
    
    DR_Sprite(bucket,
              V4F32(1.0f, 1.0f, 1.0f, 1.0f),
              render_rect,
              R2F32(V2F32(0.0f, 0.0f),
                    V2F32(texture_size.x, texture_size.y)),
              texture);
}

function void ApplyWorldTransfromOrSomeShit(Rng2F32 *rect)
{
    // derived from ClientRectFromWindow
    const F32 window_height = 664.0f;
    
    // NOTE(randy): Flip the Y so it's UP instead of the screenspace down
    rect->min.y = (window_height - rect->min.y);
    rect->max.y = (window_height - rect->max.y);
    
    F32 min = rect->min.y;
    rect->min.y = rect->max.y;
    rect->max.y = min;
    
    // NOTE(randy): apply camera
    *rect = Shift2F32(*rect, state->camera);
}