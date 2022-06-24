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
    
    //FillPixelDataRandomly();
    SetDefaultStage();
    
    //state->cpu_data = Str8(state->pixel_data, PIXEL_COUNT * 4);
    
    return state;
}

function void
S_Close(APP_Window *window, S_State *state)
{
    M_ArenaRelease(state->permanent_arena);
}

function void
S_Update(APP_Window *window, OS_EventList *events, S_State *state)
{
    local_persist PixelType selected_type = PIXEL_TYPE_sand;
    if (OS_KeyPress(events, window->handle, OS_Key_1, 0))
    {
        selected_type = PIXEL_TYPE_sand;
    }
    if (OS_KeyPress(events, window->handle, OS_Key_2, 0))
    {
        selected_type = PIXEL_TYPE_water;
    }
    if (OS_KeyPress(events, window->handle, OS_Key_3, 0))
    {
        selected_type = 0;
    }
    if (OS_KeyPress(events, window->handle, OS_Key_0, 0))
    {
        selected_type = -1;
    }
    
    if (OS_KeyPress(events, window->handle, OS_Key_MouseLeft, 0))
    {
#if BRUSH_SIZE == 1
        Vec2S32 mouse = GetPixelAtMousePos(window);
        Pixel *pixel = PixelAt(mouse.x, mouse.y);
        //pixel->type = selected_type;
#else
        S32 size = BRUSH_SIZE;
        for (int y = size / -2; y < size / 2; y++)
            for (int x = size / -2; x < size / 2; x++)
        {
            Vec2S32 mouse = GetPixelAtMousePos(window);
            Pixel *pixel = PixelAt(mouse.x + x, mouse.y + y);
            SetPixelType(pixel, selected_type);
        }
#endif
    }
    
    if(OS_KeyPress(events, window->handle, OS_Key_A, 0))
    {
        state->is_simulating = !state->is_simulating;
    }
    
    if (OS_KeyPress(events, window->handle, OS_Key_S, 0))
    {
        StepPixelSimulation();
    }
    
    if (OS_KeyPress(events, window->handle, OS_Key_R, 0))
    {
        FillPixelDataRandomly();
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
    // yooooo we got the drip drip
    // yuh yuh
    local_persist S32 update_count = 0;
    update_count++;
    Pixel *top_middle_px = PixelAt(SIM_X / 2, SIM_Y-1);
    if (drip_override &&
        update_count % DRIP_SPEED == 0)
    {
        top_middle_px->type = PIXEL_TYPE_sand;
    }
#endif
    
    
    if (!state->is_simulating)
        StepPixelSimulation();
    
    DR_Bucket bucket = {0};
    {
        Vec2F32 mouse = OS_MouseFromWindow(window->handle);
        
        Vec2S64 size = V2S64(SIM_X,
                             SIM_Y);
        
        R_Handle texture = dr_state->backend.ReserveTexture2D(dr_state->os_eqp,
                                                              size,
                                                              R_Texture2DFormat_RGBA8);
        
        UpdatePixelRenderData();
        
        // NOTE(randy): Brush preview
        {
            S32 size = BRUSH_SIZE;
            for (int y = size / -2; y < size / 2; y++)
                for (int x = size / -2; x < size / 2; x++)
            {
                Vec2S32 mouse = GetPixelAtMousePos(window);
                Vec4U8 *col = ColourAt(mouse.x + x, SIM_Y-mouse.y + y);
                if (col)
                {
                    col->r = 255;
                    col->g = 255;
                    col->b = 255;
                    col->a = 255;
                }
            }
        }
        
        dr_state->backend.FillTexture2D(dr_state->os_eqp,
                                        texture,
                                        R2S64(V2S64(0, 0),
                                              size),
                                        Str8((U8*)state->pixel_render_data, sizeof(state->pixel_render_data)));
        
        DR_Sprite(&bucket,
                  V4F32(1.0f, 1.0f, 1.0f, 1.0f),
                  R2F32(V2F32(0.0f, 0.0f),
                        V2F32(WINDOW_X - 20, WINDOW_Y - 55)), // TODO(randy): why isn't this sized properly? Texture overhangs off screen without these subtractions. Window border perhaps?
                  R2F32(V2F32(0.0f, 0.0f),
                        V2F32(SIM_X, SIM_Y)),
                  texture);
    }
    
    
    
    
    DR_Submit(window->window_equip, bucket.cmds);
    
    // NOTE(rjf): Example of explicitly using the low-level "R_" API for
    // just sending data to the rendering backend, instead of using the
    // higher level "DR_" (draw) API @api_notes
    /* 
        M_Temp scratch = GetScratch(0, 0);
        R_CmdList cmds = {0};
        R_Cmd cmd = {0};
        {
            cmd.kind = R_CmdKind_Scratch;
            cmd.cpu_data = state->cpu_data;
        }
        R_CmdListPush(scratch.arena, &cmds, &cmd);
        
        //app_state->r_backend.Submit(app_state->r_os_equip, window->window_equip, cmds);
        DR_Submit(window->window_equip, cmds);
    
    ReleaseScratch(scratch);
     */
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

function Pixel *PixelAt(S32 x, S32 y)
{
    if (x < 0)
        x = SIM_X - x;
    if (y < 0)
        y = SIM_Y - y;
    // TODO(randy): probs bugged
    
    return &state->pixels[y % SIM_Y][x % SIM_X];
}

function Vec4U8 *ColourAt(S32 x, S32 y)
{
    if (x < 0 || x >= SIM_X ||
        y < 0 || y >= SIM_Y)
        return 0;
    
    return &state->pixel_render_data[y][x];
}

function void SwapPixels(Pixel *from, Pixel *to)
{
    Pixel t = *to;
    *to = *from;
    *from = t;
}

function void FillPixelDataRandomly()
{
    for (int y = 0; y < SIM_Y; y++)
        for (int x = 0; x < SIM_X; x++)
    {
        Pixel *pixel = &state->pixels[y][x];
        //pixel->type = (PixelType)(rand() % 3);
    }
}

function void UpdatePixelRenderData()
{
    for (int y = 0; y < SIM_Y; y++)
        for (int x = 0; x < SIM_X; x++)
    {
        Vec4U8 *col = &state->pixel_render_data[y][x];
        Pixel *px = &state->pixels[SIM_Y-y-1][x];
        
        switch (GetPixelType(px))
        {
            case PIXEL_TYPE_boundary:
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
            
            default:
            {
                col->r = 255;
                col->g = 0;
                col->b = 0;
                col->a = 255;
            } break;
        }
    }
}

function void StepPixelSimulation()
{
    S32 x_pixels[SIM_X];
    for (int i = 0; i < SIM_X; i++)
    {
        x_pixels[i] = i;
    }
    ShuffleArray(x_pixels, SIM_X);
    
    for (int y = 0; y < SIM_Y; y++)
    {
        for (int x = 0; x < SIM_X; x++)
        {
            S32 x_pos = x_pixels[x];
            Pixel *pixel = &state->pixels[y][x_pos];
            StepPixel(pixel, x_pos, y);
        }
    }
}


#if 0
else
{
    // move right or left
    U8 dispersion_rate = 5;
    Pixel *move_to = PixelAt(x - dispersion_rate * (rand() % 2 == 0 ? 1 : -1), y);
    
    Vec2S32 move_to_location = GetPixelLocation(move_to);
    
    Vec2S32 pixels_in_line[16];
    U32 count = 0;
    DrawLineAtoB(V2S32(x,y), move_to_location, pixels_in_line, &count, 16);
    
    Pixel *last_good_pixel = 0;
    for (int i = 1; i < count; i++)
    {
        Vec2S32 loc = pixels_in_line[i];
        Pixel *line_pixel = PixelAt(loc.x, loc.y);
        if (line_pixel->type == PIXEL_TYPE_air)
        {
            last_good_pixel = line_pixel;
            continue;
        }
        else
        {
            break;
        }
    }
    
    if (last_good_pixel)
    {
        SwapPixels(pixel, last_good_pixel);
    }
}
#endif



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
    for (int y = 0; y < SIM_Y; y++)
        for (int x = 0; x < SIM_X; x++)
    {
        if (&state->pixels[y][x] == pixel)
            return V2S32(x, y);
    }
    
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
    MemorySet(state->pixels, 0, sizeof(state->pixels));
    
    for (int y = 0; y < SIM_Y; y++)
        for (int x = 0; x < SIM_X; x++)
    {
        if (x >= SIM_X / 10 && x <= SIM_X - SIM_X / 10 &&
            y >= 20 && y <= 30)
        {
            // TODO(randy): set immovable
        }
    }
}

function void StepPixel(Pixel *pixel, S32 x, S32 y)
{
    Pixel *down = PixelAt(x, y-1);
    Pixel *left = PixelAt(x-1, y);
    Pixel *right = PixelAt(x+1, y);
    Pixel *down_left = PixelAt(x-1, y-1);
    Pixel *down_right = PixelAt(x+1, y-1);
    
    if (pixel->flags & PIXEL_FLAG_gravity) // this should work right? it'll be non-zero
    {
        
    }
}

function B8 CanPixelMoveTo(Pixel *src, Pixel *dest)
{
    /* 
        switch (src->type)
        {
            case PIXEL_TYPE_sand:
            return (dest->type == PIXEL_TYPE_air || dest->type == PIXEL_TYPE_water);
            
            case PIXEL_TYPE_water:
            return (dest->type == PIXEL_TYPE_air);
        }
     */
    
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

// TODO(randy):  YEET
function B8 AttemptFallPixel(Pixel *pixel, S32 x, S32 y)
{
    // NOTE(randy): a solution to this max speed banding artifact would be to take the pixel's distance travelled across the frame and blue it in a line, like a fast moving object
    
    // inertial resistance is the % chance a pixel will be dislodged (set to falling) by passing pixels
    
    
    // this whole thing is a hot fuckin mess lmao
    
    // at what point do I want to check the stationary pixel?
    // maybe just run it 
    
    
    // if there's a pixel below it, early out
    if (!CanPixelMoveTo(pixel, PixelAt(x, y-1)))
    {
        if (!F32Compare(pixel->vel.y, 0.0f, 0.01f))
        {
            // NOTE(randy): disperse Y -> X velocity
            {
                S32 sign;
                if (!F32Compare(pixel->vel.x, 0.0f, 0.01f))
                    sign = Sign(pixel->vel.x);
                else
                    sign = rand() % 2 ? -1 : 1;
                
                F32 div_amount = 3.0f;// + (rand() / RAND_MAX * 2);
                
                pixel->vel.x = fabsf(pixel->vel.x) + (fabsf(pixel->vel.y) / div_amount);
                pixel->vel.x *= sign;
                pixel->vel.y = 0.0f;
            }
        }
        
        return 0;
    }
    
    const F32 gravity = 0.4f;
    pixel->vel.y -= gravity;
    
    const max_speed = 10.0f;
    pixel->vel.y = Min(pixel->vel.y, max_speed);
    
    Vec2S32 from_loc = V2S32(x, y);
    Vec2S32 to_loc = V2S32(x, y - 1 + pixel->vel.y);
    
    Vec2S32 inter_pixels[16];
    U32 count = 0;
    DrawLineAtoB(from_loc, to_loc, inter_pixels, &count, 16);
    
    Pixel *last_good_pixel = 0;
    for (int i = 1; i < count; i++)
    {
        Vec2S32 pos = inter_pixels[i];
        Pixel *inter_pixel = PixelAt(pos.x, pos.y);
        
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
        SwapPixels(pixel, last_good_pixel);
        return 1;
    }
    else
    {
        return 0;
    }
}

function B8 AttemptDisperseWater(Pixel *water_pixel, Vec2S32 from_loc, Vec2S32 to_loc)
{
    /* 
        Vec2S32 inter_pixels[16];
        U32 count = 0;
        DrawLineAtoB(from_loc, to_loc, inter_pixels, &count, 16);
        
        Pixel *last_good_pixel = 0;
        for (int i = 1; i < count; i++)
        {
            Vec2S32 pos = inter_pixels[i];
            Pixel *inter_pixel = PixelAt(pos.x, pos.y);
            if (inter_pixel->type != PIXEL_TYPE_water &&
                inter_pixel->type != PIXEL_TYPE_air)
                break;
            else if (inter_pixel->type == PIXEL_TYPE_air)
                last_good_pixel = inter_pixel;
            // NOTE(randy): algo will pass through water
        }
        
        if (last_good_pixel)
        {
            SwapPixels(water_pixel, last_good_pixel);
            
            return 1;
        }
        else
        {
            return 0;
        }
         */
    
    return 0;
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