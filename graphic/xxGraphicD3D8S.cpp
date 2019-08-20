#include "xxGraphicD3D8S.h"
#include "xxGraphicD3DAsm.h"
#include "xxGraphicInternal.h"

#include "dxsdk/d3d8.h"

static DWORD g_defaultShaderVS = 0;
static DWORD g_defaultShaderPS = 0;

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D8S()
{
    uint64_t instance = xxCreateInstanceD3D8();
    if (instance == 0)
        return 0;

    xxCreateInstance = xxCreateInstanceD3D8S;
    xxCreateDevice = xxCreateDeviceD3D8S;
    xxDestroyDevice = xxDestroyDeviceD3D8S;
    xxSetScissor = xxSetScissorD3D8S;
    xxSetVertexAttribute = xxSetVertexAttributeD3D8S;
    xxSetOrthographicTransform = xxSetOrthographicTransformD3D8S;

    return instance;
}
//==============================================================================
//  Device
//==============================================================================
uint64_t xxCreateDeviceD3D8S(uint64_t instance)
{
    uint64_t device = xxCreateDeviceD3D8(instance);
    if (device == 0)
        return 0;

    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(device);
    if (d3dDevice == nullptr)
        return 0;

    if (g_defaultShaderVS == 0)
    {
        HRESULT hResult = d3dDevice->CreateVertexShader(vertexShaderCode10Declaration, vertexShaderCode10, &g_defaultShaderVS, 0);
        if (hResult != S_OK)
        {
            xxDestroyDeviceD3D8S(device);
            return 0;
        }
    }

    if (g_defaultShaderPS == 0)
    {
        HRESULT hResult = d3dDevice->CreatePixelShader(pixelShaderCode11, &g_defaultShaderPS);
        if (hResult != S_OK)
        {
            xxDestroyDeviceD3D8S(device);
            return 0;
        }
    }

    return device;
}
//------------------------------------------------------------------------------
void xxDestroyDeviceD3D8S(uint64_t device)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(device);
    if (d3dDevice == nullptr)
        return;

    if (g_defaultShaderVS)
    {
        d3dDevice->DeleteVertexShader(g_defaultShaderVS);
        g_defaultShaderVS = 0;
    }

    if (g_defaultShaderPS)
    {
        d3dDevice->DeletePixelShader(g_defaultShaderPS);
        g_defaultShaderPS = 0;
    }

    xxDestroyDeviceD3D8(device);
}
//==============================================================================
//  Command
//==============================================================================
void xxSetScissorD3D8S(uint64_t commandBuffer, int x, int y, int width, int height)
{
    xxSetScissorD3D8(commandBuffer, x, y, width, height);

    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    D3DMATRIX projection;
    d3dDevice->GetTransform(D3DTS_PROJECTION, &projection);

    d3dDevice->SetVertexShaderConstant(0, projection.m[0], 16);
}
//------------------------------------------------------------------------------
void xxSetVertexAttributeD3D8S(uint64_t commandBuffer, uint64_t vertexAttribute)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    D3DVERTEXATTRIBUTE8 d3dVertexAttribtue = { vertexAttribute };

    LPDIRECT3DVERTEXBUFFER8 d3dVertexBuffer = nullptr;
    for (int i = 0; i < 8; ++i)
    {
        UINT stride;
        d3dDevice->GetStreamSource(i, &d3dVertexBuffer, &stride);
        if (d3dVertexBuffer == nullptr)
            break;
        d3dDevice->SetStreamSource(i, d3dVertexBuffer, d3dVertexAttribtue.stride);
        d3dVertexBuffer->Release();
    }
}
//==============================================================================
//  Fixed-Function
//==============================================================================
void xxSetOrthographicTransformD3D8S(uint64_t commandBuffer, float left, float right, float top, float bottom)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    xxSetOrthographicTransformD3D8(commandBuffer, left, right, top, bottom);

    D3DMATRIX projection;
    d3dDevice->GetTransform(D3DTS_PROJECTION, &projection);

    d3dDevice->SetVertexShaderConstant(0, projection.m[0], 16);
    d3dDevice->SetVertexShader(g_defaultShaderVS);
    d3dDevice->SetPixelShader(g_defaultShaderPS);
}
//==============================================================================
