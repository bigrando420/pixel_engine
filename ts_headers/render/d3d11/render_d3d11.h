#ifndef RENDER_D3D11_H
#define RENDER_D3D11_H

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

struct R_D3D11_Globals_PerRect
{
    Vec2F32 viewport_size;
    F32 sdf_mask_boldness;
    F32 sdf_mask_softness;
    Matrix4x4F32 albedo_sample_channel_map;
    Vec2F32 albedo_t2d_size;
};

struct R_D3D11_Texture2D
{
    ID3D11Texture2D *texture;
    ID3D11ShaderResourceView *view;
    Vec2S32 size;
    R_Texture2DFormat format;
};

struct R_D3D11_Buffer
{
    ID3D11Buffer *obj;
    U64 size;
};

struct R_D3D11_OSEquip
{
    ID3D11Device *base_device;
    ID3D11DeviceContext *base_device_ctx;
    ID3D11Device1 *device;
    ID3D11DeviceContext1 *device_ctx;
    IDXGIDevice1 *dxgi_device;
    IDXGIAdapter *dxgi_adapter;
    IDXGIFactory2 *dxgi_factory;
    ID3D11RasterizerState1 *rasterizer;
    ID3D11BlendState *blend_state;
    ID3D11SamplerState *default_sampler;
    ID3D11DepthStencilState *depth_stencil_state;
    ID3D11VertexShader *rect_vshad;
    ID3D11PixelShader *rect_pshad;
    ID3D11InputLayout *rect_ilay;
    ID3D11Buffer *constants__per_rect_buffer;
    ID3D11Buffer *scratch_buffer_16mb;
    R_Handle white_texture;
    R_Handle backup_texture;
};

struct R_D3D11_WindowEquip
{
    IDXGISwapChain1 *swapchain;
    ID3D11Texture2D *framebuffer;
    ID3D11RenderTargetView *framebuffer_view;
    Vec2S64 last_resolution;
};

////////////////////////////////
//~ rjf: Helpers

function DXGI_FORMAT R_D3D11_DXGIFormatFromTexture2DFormat(R_Texture2DFormat fmt);

function R_D3D11_OSEquip *R_D3D11_OSEquipFromHandle(R_Handle handle);
function R_Handle R_D3D11_HandleFromOSEquip(R_D3D11_OSEquip *equip);

function R_D3D11_WindowEquip *R_D3D11_WindowEquipFromHandle(R_Handle handle);
function R_Handle R_D3D11_HandleFromWindowEquip(R_D3D11_WindowEquip *equip);

function R_D3D11_Texture2D R_D3D11_Texture2DFromHandle(R_Handle handle);
function R_Handle R_D3D11_HandleFromTexture2D(R_D3D11_Texture2D texture);

function R_D3D11_Buffer R_D3D11_BufferFromHandle(R_Handle handle);
function R_Handle R_D3D11_HandleFromBuffer(R_D3D11_Buffer buffer);

function void R_D3D11_WriteGPUBuffer(ID3D11DeviceContext1 *device_ctx, ID3D11Buffer *buffer, U64 offset, String8 data);
function ID3D11Buffer *R_D3D11_ResolveCPUAndGPUDataToBuffer(R_D3D11_OSEquip *os, String8 cpu_data, R_Handle gpu_data);

////////////////////////////////
//~ rjf: Backend Hooks

exported R_Handle EquipOS(void);
exported R_Handle EquipWindow(R_Handle os_eqp, OS_Handle window);
exported void UnequipWindow(R_Handle os_eqp, R_Handle window_eqp);
exported R_Handle ReserveTexture2D(R_Handle os_eqp, Vec2S64 size, R_Texture2DFormat fmt);
exported void ReleaseTexture2D(R_Handle os_eqp, R_Handle texture);
exported void FillTexture2D(R_Handle os_eqp, R_Handle texture, Rng2S64 subrect, String8 data);
exported Vec2F32 SizeFromTexture2D(R_Handle texture);
exported void Start(R_Handle os_eqp, R_Handle window_eqp, Vec2S64 resolution);
exported void Submit(R_Handle os_eqp, R_Handle window_eqp, R_CmdList commands);
exported void Finish(R_Handle os_eqp, R_Handle window_eqp);

#endif // RENDER_D3D11_H
