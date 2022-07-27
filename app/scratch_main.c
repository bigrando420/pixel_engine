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
    
    state->window = window;
    
    state->camera_zoom = DEFAULT_CAM_ZOOM;
    state->is_simulating = START_SIM_STRAIGHT_AWAY;
    
    // NOTE(randy): create all textures for all chunks
    for (int i = 0; i < MAX_ACTIVE_CHUNKS; i ++)
    {
        Chunk *chunk = &state->chunks[i];
        chunk->texture = dr_state->backend.ReserveTexture2D(dr_state->os_eqp,
                                                            V2S64(CHUNK_SIZE, CHUNK_SIZE),
                                                            R_Texture2DFormat_RGBA8);
    }
    
    // SetDefaultStage();
    
    ChunkInitAtLoc(V2S32(0, 0));
    ChunkInitAtLoc(V2S32(1, 0));
    ChunkInitAtLoc(V2S32(1, 1));
    ChunkInitAtLoc(V2S32(0, -1));
    ChunkInitAtLoc(V2S32(-1, -2));
    
    return state;
}

function void
S_Close(APP_Window *window, S_State *state)
{
    // NOTE(randy): Do I need to free these?
    for (int i = 0; i < MAX_ACTIVE_CHUNKS; i ++)
    {
        Chunk *chunk = &state->chunks[i];
        dr_state->backend.ReleaseTexture2D(dr_state->os_eqp, chunk->texture);
    }
    
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
    local_persist B8 left_mouse_down = 0;
    if (OS_KeyPress(events, window->handle, OS_Key_MouseLeft, 0))
    {
        left_mouse_down = 1;
    }
    if (OS_KeyRelease(events, window->handle, OS_Key_MouseLeft, 0))
    {
        left_mouse_down = 0;
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
    
    // @brush
    if (left_mouse_down)
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
            Vec2S32 mouse = GetPixelAtMousePos();
            Pixel *pixel = PixelAtAbsolutePos(V2S32(mouse.x + x, mouse.y + y));
            if (pixel)
            {
                SetPixelType(pixel, selected_type);
            }
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
    
    ChunksLoadUnloadInCameraView();
    ChunkSortActive();
    
    if (state->is_simulating)
        StepPixelSimulation();
    
    Render();
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

function Vec2S32 GetPixelAtMousePos()
{
    Vec2F32 v = OS_MouseFromWindow(state->window->handle);
    v = ScreenPositionToWorldPosition(v);
    
    return V2S32(v.x, v.y);
}

function void SetDefaultStage()
{
    for (int i = 0; i < MAX_ACTIVE_CHUNKS; i++)
    {
        Chunk *chunk = &state->chunks[i];
        if (chunk->valid)
        {
            ChunkSetDefaultPixels(chunk);
        }
    }
    
    
    // NOTE(randy): this would be a good solution for deleting the entire world?
    /* 
        UnloadWorld();
        DeleteChunksOnDisk();
     */
    
    
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
    Vec2S32 chunk_loc = ChunkGetPosFromWorldPos(V2F32(pos.x, pos.y));
    
    for (int i = 0; i < MAX_ACTIVE_CHUNKS; i++)
    {
        Chunk *chunk = &state->chunks[i];
        if (chunk->valid &&
            chunk->pos.x == chunk_loc.x &&
            chunk->pos.y == chunk_loc.y)
        {
            S32 x_index = pos.x;
            if (pos.x < 0)
            {
                x_index += 1;
                x_index = abs(x_index);
                x_index = x_index % CHUNK_SIZE;
                x_index = CHUNK_SIZE - x_index - 1;
                // now we are a happy chappy :)
            }
            else
            {
                x_index = x_index % CHUNK_SIZE;
            }
            
            S32 y_index = pos.y;
            if (pos.y < 0)
            {
                y_index += 1;
                y_index = abs(y_index);
                y_index = y_index % CHUNK_SIZE;
                y_index = CHUNK_SIZE - y_index - 1;
            }
            else
            {
                y_index = y_index % CHUNK_SIZE;
            }
            
            Assert(x_index >= 0 && x_index < CHUNK_SIZE &&
                   y_index >= 0 && y_index < CHUNK_SIZE);
            
            return &chunk->pixels[y_index][x_index];
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

// @pixel
function void PixelStep(Chunk *chunk, Pixel *pixel, Vec2S32 local_pos)
{
    if (pixel->has_been_updated)
        return;
    
    pixel->has_been_updated = 1;
    
    B8 has_moved = 0;
    
    //B8 is_falling_check = ((pixel->flags & PIXEL_FLAG_inertia) ? pixel->is_resting : 1);
    
    //~ Gravity - simple 1 step
    if (!pixel->is_resting &&
        pixel->flags & PIXEL_FLAG_gravity_simple)
    {
        Pixel *pxl_below = PixelAtRelativeOffset(chunk, pixel, local_pos, V2S32(0, -1));
        if (!pxl_below)
        {
            PixelClear(pixel);
            return;
        }
        
        if (CanPixelMoveTo(pixel, pxl_below))
        {
            SwapPixels(pixel, pxl_below, &pixel);
            has_moved = 1;
        }
    }
    
    //~ Gravity - velocity based
    if (!pixel->is_resting &&
        pixel->flags & PIXEL_FLAG_gravity_velocity)
    {
        const F32 gravity = 0.4f;
        pixel->vel.y -= gravity;
        
        const F32 max_speed = 10.0f;
        pixel->vel.y = Min(pixel->vel.y, max_speed);
        
        Vec2S32 from_loc = local_pos;
        Vec2S32 to_loc = V2S32(local_pos.x, local_pos.y - 1 + pixel->vel.y);
        
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
    
    //~ Move diagonally
    if (!has_moved &&
        !pixel->is_resting &&
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
        
        Pixel *left = PixelAtRelativeOffset(chunk, pixel, local_pos, V2S32(-1, 0));
        Pixel *right = PixelAtRelativeOffset(chunk, pixel, local_pos, V2S32(1, 0));
        Pixel *down_left = PixelAtRelativeOffset(chunk, pixel, local_pos, V2S32(-1, -1));
        Pixel *down_right = PixelAtRelativeOffset(chunk, pixel, local_pos, V2S32(1, -1));
        if (!left || !right || !down_right || !down_left)
        {
            PixelClear(pixel);
            return;
        }
        
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
    /*
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
*/
    
    //~ Inertia
    //pixel->is_resting = !has_moved;
    // NOTE(randy): why does this collapse the entire structure when it's like this
    
    // TODO(randy): I really need better debug visuals
    // tint the pixel based off of the state (is_resting for example)
    
    if (!pixel->is_resting &&
        (pixel->flags & PIXEL_FLAG_dislodge_neighbours))
    {
        Pixel *left = PixelAtRelativeOffset(chunk, pixel, local_pos, V2S32(-1, 0));
        Pixel *right = PixelAtRelativeOffset(chunk, pixel, local_pos, V2S32(1, 0));
        Pixel *down_left = PixelAtRelativeOffset(chunk, pixel, local_pos, V2S32(-1, -1));
        Pixel *down_right = PixelAtRelativeOffset(chunk, pixel, local_pos, V2S32(1, -1));
        Pixel *top_left = PixelAtRelativeOffset(chunk, pixel, local_pos, V2S32(-1, 1));
        Pixel *top_right = PixelAtRelativeOffset(chunk, pixel, local_pos, V2S32(1, 1));
        Pixel *top = PixelAtRelativeOffset(chunk, pixel, local_pos, V2S32(0, 1));
        Pixel *down = PixelAtRelativeOffset(chunk, pixel, local_pos, V2S32(0, -1));
        
        if (!left || !right || !down_left || !down_right || !top_left || !top_right || !top || !down)
        {
            PixelClear(pixel);
            return;
        }
        
        if (rand() % DISLODGE_CHANCE == 0)
        {
            left->is_resting = 0;
            right->is_resting = 0;
            down_left->is_resting = 0;
            down_right->is_resting = 0;
            top_left->is_resting = 0;
            top_right->is_resting = 0;
            top->is_resting = 0;
            down->is_resting = 0;
        }
    }
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
    return (PixelType)pixel->flags;
}

function void SetPixelType(Pixel *pixel, PixelType type)
{
    pixel->flags = type;
    
    if (!state->is_simulating)
        pixel->is_resting = 1;
    else
        pixel->is_resting = 0;
    
    if (type == PIXEL_TYPE_air)
        pixel->is_resting = 0;
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

function Chunk *ChunkAttemptLoadFromDisk(Vec2S32 pos)
{
    char chunk_fname[256];
    sprintf(chunk_fname, "%i,%i.chunk", pos.x, pos.y);
    char chunk_path[256];
    sprintf(chunk_path, "/balls/%s", chunk_fname);
    
    FILE *f = fopen(chunk_path, "rb");
    if (!f)
        return 0;
    
    Chunk *chunk = ChunkInitAtLoc(pos);
    if (!chunk)
    {
        fclose(f);
        return 0;
    }
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
    
    ChunkZero(chunk);
}

function void ChunkZero(Chunk *chunk)
{
    chunk->valid = 0;
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

function void DeleteChunksOnDisk()
{
    // TODO(randy): idk how to delete an entire folder
}

function void LoadWorld()
{
    Vec2S32 chunks[128];
    U32 count;
    ChunksInRect(CameraGetViewRect(), chunks, 128, &count);
    
    for (int i = 0; i < count; i++)
    {
        ChunkAttemptLoadFromDisk(chunks[i]);
    }
}

function void ChunkRenderDebugAt(Vec2S32 pos)
{
    if (state->render_debug_chunks_count + 1 > DEBUG_CHUNK_COUNT)
        return;
    
    state->render_debug_chunks[state->render_debug_chunks_count++] = pos;
}

function Vec2S32 ChunkGetPosFromWorldPos(Vec2F32 world_pos)
{
    return V2S32(floorf((F32)world_pos.x / (F32)CHUNK_SIZE),
                 floorf((F32)world_pos.y / (F32)CHUNK_SIZE));
}

function void ChunksInRect(Rng2F32 rect, Vec2S32 *chunk_arr, U32 chunk_arr_max, U32 *count)
{
    *count = 0;
    //rect = Pad2F32(rect, -100);
    
    Vec2S32 bottom_left = ChunkGetPosFromWorldPos(rect.min);
    Vec2S32 top_right = ChunkGetPosFromWorldPos(rect.max);
    
    for (int x = bottom_left.x; x <= top_right.x; x++)
    {
        for (int y = bottom_left.y; y <= top_right.y; y++)
        {
            if (*count + 1 > chunk_arr_max)
                return;
            
            Vec2S32 chunk = V2S32(x, y);
            chunk_arr[(*count)++] = chunk;
        }
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
    
    // NOTE(randy): no room for chunk
    return 0;
}

function void ChunkSortActive()
{
    // NOTE(randy): Sort chunks so it's from the bottom up
    /* 
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
     */
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

// @render
function void Render()
{
    DR_Bucket bucket = {0};
    
    /* 
        Rng2F32 cam_rect = CameraGetViewRect();
        DR_Rect(&bucket, cam_rect, V4F32(1.0f, 0.0f, 1.0f, 1.0f));
     */
    // test square
    /* 
        Rng2F32 render_rect = R2F32(V2F32(100.0f, 100.0f),
                                    V2F32(200.0f,
                                          200.0f));
        ApplyWorldTransfromOrSomeShit(&render_rect);
        DR_Rect(&bucket, render_rect, V4F32(1.0f, 1.0f, 1.0f, 1.0f));
     */
    
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
    
    ChunkRenderActive(&bucket);
    
    // NOTE(randy): queue up all chunks in view for render
    // separated out bc I might still want to do other specific debug chunk renders in future
#if CHUNK_RENDER_DEBUG
    Vec2S32 chunks[MAX_ACTIVE_CHUNKS];
    U32 count;
    ChunksInRect(CameraGetViewRect(), chunks, MAX_ACTIVE_CHUNKS, &count);
    for (int i = 0; i < count; i++)
    {
        ChunkRenderDebugAt(chunks[i]);
    }
    
    // NOTE(randy): render debug chunks
    for (int i = 0; i < state->render_debug_chunks_count; i++)
    {
        Vec2S32 pos = state->render_debug_chunks[i];
        
        Rng2F32 render_rect = R2F32(V2F32(0.0f, 0.0f),
                                    V2F32(CHUNK_SIZE,
                                          CHUNK_SIZE));
        render_rect = Shift2F32(render_rect, V2F32(pos.x * CHUNK_SIZE * 1.005f,
                                                   pos.y * CHUNK_SIZE * 1.005f));
        ApplyWorldTransfromOrSomeShit(&render_rect);
        
        DR_Rect_B(&bucket, render_rect, V4F32(1.0f, 0.0f, 0.0f, 1.0f), 5.0f);
    }
    state->render_debug_chunks_count = 0;
#endif
    
    DR_Submit(state->window->window_equip, bucket.cmds);
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
        
        // NOTE(randy): Tinting / debug stuff
        if (px->is_resting)
        {
            col->r = 150;
            col->g = 255;
            col->b = 150;
        }
        
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
    }
    
    // NOTE(randy): Fill texture with render data
    Vec2S64 texture_size = V2S64(CHUNK_SIZE, CHUNK_SIZE);
    
    dr_state->backend.FillTexture2D(dr_state->os_eqp,
                                    chunk->texture,
                                    R2S64(V2S64(0, 0),
                                          texture_size),
                                    Str8((U8*)chunk_texture_data, 
                                         sizeof(chunk_texture_data)));
    
    // TODO(randy): weird bug where the texture being filled into isn't the right size?
    // It's only like 1x64, which is weird af
    // idk lol
    // NOTE(randy): maybe get the textures immediately afterwards and view their data to make sure it's all good?
    
    Rng2F32 render_rect = R2F32(V2F32(0.0f, 0.0f),
                                V2F32(CHUNK_SIZE,
                                      CHUNK_SIZE));
    
    // shift into correct location
    render_rect = Shift2F32(render_rect, V2F32(chunk->pos.x * CHUNK_SIZE * (CHUNK_DEBUG_SPACE ? 1.005f : 1.0f),
                                               chunk->pos.y * CHUNK_SIZE * (CHUNK_DEBUG_SPACE ? 1.005f : 1.0f)));
    ApplyWorldTransfromOrSomeShit(&render_rect);
    
    DR_Sprite(bucket,
              V4F32(1.0f, 1.0f, 1.0f, 1.0f),
              render_rect,
              R2F32(V2F32(0.0f, 0.0f),
                    V2F32(texture_size.x, texture_size.y)),
              chunk->texture);
}

// TODO(randy): rename
// WorldSpaceToCameraSpace or some shit
function void ApplyWorldTransfromOrSomeShit(Rng2F32 *rect)
{
    Rng2F32 window_rect = OS_ClientRectFromWindow(state->window->handle);
    F32 window_height = window_rect.max.y;
    
    // NOTE(randy): scale
    rect->min.x *= state->camera_zoom;
    rect->min.y *= state->camera_zoom;
    rect->max.x *= state->camera_zoom;
    rect->max.y *= state->camera_zoom;
    
    // NOTE(randy): Flip the Y so it's UP instead of the screenspace down
    rect->min.y = (window_height - rect->min.y);
    rect->max.y = (window_height - rect->max.y);
    
    F32 min = rect->min.y;
    rect->min.y = rect->max.y;
    rect->max.y = min;
    
    // NOTE(randy): apply camera translation
    *rect = Shift2F32(*rect, state->camera);
}

function Vec2F32 ScreenPositionToWorldPosition(Vec2F32 v)
{
    Rng2F32 rect = OS_ClientRectFromWindow(state->window->handle);
    
    v.y = rect.max.y - v.y;
    
    v = Add2F32(v, V2F32(-state->camera.x, state->camera.y));
    
    v.x /= state->camera_zoom;
    v.y /= state->camera_zoom;
    
    return v;
}

// do we want the rect in worldspace - so Y positive is up?
// I'd say so, it makes the most intuative sense
// can always revert this?
// it's already in world space?
function Rng2F32 CameraGetViewRect()
{
    Rng2F32 rect = OS_ClientRectFromWindow(state->window->handle);
    
    rect = Shift2F32(rect, V2F32(-state->camera.x, state->camera.y));
    
    rect.min.x /= state->camera_zoom;
    rect.min.y /= state->camera_zoom;
    rect.max.x /= state->camera_zoom;
    rect.max.y /= state->camera_zoom;
    
    return rect;
}

function void ChunksLoadUnloadInCameraView()
{
    Vec2S32 chunks_in_view[128];
    U32 count = 0;
    ChunksInRect(CameraGetViewRect(), chunks_in_view, 128, &count);
    
    // NOTE(randy): Unload inactive chunks
    for (int i = 0; i < MAX_ACTIVE_CHUNKS; i++)
    {
        Chunk *active_chunk = &state->chunks[i];
        if (active_chunk->valid)
        {
            B8 is_chunk_still_in_view = 0;
            
            for (int j = 0; j < count; j++)
            {
                Vec2S32 view_chunk = chunks_in_view[j];
                if (view_chunk.x == active_chunk->pos.x &&
                    view_chunk.y == active_chunk->pos.y)
                {
                    is_chunk_still_in_view = 1;
                }
            }
            
            if (!is_chunk_still_in_view)
            {
                ChunkUnload(active_chunk);
            }
        }
    }
    
    // NOTE(randy): Load in visible chunks
    for (int i = 0; i < count; i++)
    {
        Vec2S32 view_chunk = chunks_in_view[i];
        Chunk *chunk = ChunkAttemptLoadFromDisk(view_chunk);
        if (!chunk)
        {
            // no chunk found on disk (or active buffer is full)
            chunk = ChunkInitAtLoc(view_chunk);
            if (!chunk)
            {
                // is full
            }
        }
    }
}