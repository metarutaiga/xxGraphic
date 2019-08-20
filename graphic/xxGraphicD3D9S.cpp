#include "xxGraphicD3D9S.h"
#include "xxGraphicD3DAsm.h"
#include "xxGraphicInternal.h"

#include "dxsdk/d3d9.h"

static LPDIRECT3DVERTEXDECLARATION9 g_defaultVertexDelaration = nullptr;
static LPDIRECT3DVERTEXSHADER9      g_defaultShaderVS = nullptr;
static LPDIRECT3DPIXELSHADER9       g_defaultShaderPS = nullptr;

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D9S()
{
    uint64_t instance = xxCreateInstanceD3D9();
    if (instance == 0)
        return 0;

    xxCreateInstance = xxCreateInstanceD3D9S;
    xxCreateDevice = xxCreateDeviceD3D9S;
    xxDestroyDevice = xxDestroyDeviceD3D9S;
    xxSetVertexAttribute = xxSetVertexAttributeD3D9S;
    xxSetOrthographicTransform = xxSetOrthographicTransformD3D9S;

    return instance;
}
//==============================================================================
//  Device
//==============================================================================
uint64_t xxCreateDeviceD3D9S(uint64_t instance)
{
    uint64_t device = xxCreateDeviceD3D9(instance);
    if (device == 0)
        return 0;

    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;

    if (g_defaultVertexDelaration == nullptr)
    {
        D3DVERTEXELEMENT9 vertexElement[] =
        {
            { 0, 0,  D3DDECLTYPE_FLOAT3,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,   0 },
            { 0, 12, D3DDECLTYPE_D3DCOLOR,  D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,      0 },
            { 0, 16, D3DDECLTYPE_FLOAT2,    D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,   0 },
            D3DDECL_END()
        };
        HRESULT hResult = d3dDevice->CreateVertexDeclaration(vertexElement, &g_defaultVertexDelaration);
        if (hResult != S_OK)
        {
            xxDestroyDeviceD3D9S(device);
            return 0;
        }
    }

    if (g_defaultShaderVS == nullptr)
    {
        HRESULT hResult = d3dDevice->CreateVertexShader(vertexShaderCode11, &g_defaultShaderVS);
        if (hResult != S_OK)
        {
            xxDestroyDeviceD3D9S(device);
            return 0;
        }
    }

    if (g_defaultShaderPS == nullptr)
    {
        HRESULT hResult = d3dDevice->CreatePixelShader(pixelShaderCode11, &g_defaultShaderPS);
        if (hResult != S_OK)
        {
            xxDestroyDeviceD3D9S(device);
            return 0;
        }
    }

    return device;
}
//------------------------------------------------------------------------------
void xxDestroyDeviceD3D9S(uint64_t device)
{
    if (g_defaultVertexDelaration)
    {
        g_defaultVertexDelaration->Release();
        g_defaultVertexDelaration = nullptr;
    }

    if (g_defaultShaderVS)
    {
        g_defaultShaderVS->Release();
        g_defaultShaderVS = nullptr;
    }

    if (g_defaultShaderPS)
    {
        g_defaultShaderPS->Release();
        g_defaultShaderPS = nullptr;
    }

    xxDestroyDeviceD3D9(device);
}
//==============================================================================
//  Command
//==============================================================================
void xxSetVertexAttributeD3D9S(uint64_t commandBuffer, uint64_t vertexAttribute)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    D3DVERTEXATTRIBUTE9 d3dVertexAttribtue = { vertexAttribute };
    d3dDevice->SetVertexDeclaration(g_defaultVertexDelaration);

    LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer = nullptr;
    for (int i = 0; i < 8; ++i)
    {
        UINT offset;
        UINT stride;
        d3dDevice->GetStreamSource(i, &d3dVertexBuffer, &offset, &stride);
        if (d3dVertexBuffer == nullptr)
            break;
        d3dDevice->SetStreamSource(i, d3dVertexBuffer, 0, d3dVertexAttribtue.stride);
        d3dVertexBuffer->Release();
    }
}
//==============================================================================
//  Fixed-Function
//==============================================================================
void xxSetOrthographicTransformD3D9S(uint64_t commandBuffer, float left, float right, float top, float bottom)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;

    xxSetOrthographicTransformD3D9(commandBuffer, left, right, top, bottom);

    D3DMATRIX projection;
    d3dDevice->GetTransform(D3DTS_PROJECTION, &projection);

    d3dDevice->SetVertexShaderConstantF(0, projection.m[0], 16);
    d3dDevice->SetVertexShader(g_defaultShaderVS);
    d3dDevice->SetPixelShader(g_defaultShaderPS);
}
//==============================================================================
