//==============================================================================
// xxGraphic : Direct3D 8.0 Programmable Shader Source
//
// Copyright (c) 2019-2026 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxSystem.h"
#include "dxsdk/d3d8.h"
#include "dxsdk/d3dcommon.h"
#include "internal/xxGraphicInternalD3D.h"
#include "xxGraphicD3DAsm.h"
#include "xxGraphicD3D8PS.h"

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D8PS()
{
    uint64_t instance = xxCreateInstanceD3D8();
    if (instance == 0)
        return 0;

    xxRegisterFunctionSingle(xxCreateInstance, xxCreateInstanceD3D8PS);
    xxRegisterFunctionSingle(xxGetInstanceName, xxGetInstanceNameD3D8PS);

    xxRegisterFunctionSingle(xxCreateVertexAttribute, xxCreateVertexAttributeD3D8PS);
    xxRegisterFunctionSingle(xxDestroyVertexAttribute, xxDestroyVertexAttributeD3D8PS);

    xxRegisterFunctionSingle(xxCreateVertexShader, xxCreateVertexShaderD3D8PS);
    xxRegisterFunctionSingle(xxCreateFragmentShader, xxCreateFragmentShaderD3D8PS);
    xxRegisterFunctionSingle(xxDestroyShader, xxDestroyShaderD3D8PS);

    xxRegisterFunctionSingle(xxCreatePipeline, xxCreatePipelineD3D8PS);
    xxRegisterFunctionSingle(xxDestroyPipeline, xxDestroyPipelineD3D8PS);

    xxRegisterFunctionSingle(xxSetScissor, xxSetScissorD3D8PS);
    xxRegisterFunctionSingle(xxSetVertexBuffers, xxSetVertexBuffersD3D8PS);
    xxRegisterFunctionSingle(xxSetVertexConstantBuffer, xxSetVertexConstantBufferD3D8PS);
    xxRegisterFunctionSingle(xxSetFragmentConstantBuffer, xxSetFragmentConstantBufferD3D8PS);

    return instance;
}
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t xxCreateVertexAttributeD3D8PS(uint64_t device, int count, int* attribute)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3DVERTEXATTRIBUTE8PS* d3dVertexAttribute = xxAlloc(D3DVERTEXATTRIBUTE8PS);
    if (d3dVertexAttribute == nullptr)
        return 0;

    d3dVertexAttribute->declaration[0] = D3DVSD_STREAM(0);
    BYTE textureIndex = 0;
    int stride = 0;

    for (int i = 0; i < count; ++i)
    {
        int stream = (*attribute++);
        int offset = (*attribute++);
        int element = (*attribute++);
        int size = (*attribute++);

        (void)stream;
        (void)offset;
        stride += size;

        DWORD& declaration = d3dVertexAttribute->declaration[i + 1];

        if (element == 'POS3' && size == sizeof(float) * 3)
        {
            declaration = D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3);
            continue;
        }

        if (element == 'BON3' && size == sizeof(float) * 3)
        {
            declaration = D3DVSD_REG(D3DVSDE_BLENDWEIGHT, D3DVSDT_FLOAT3);
            continue;
        }

        if (element == 'BON4' && size == sizeof(char) * 4)
        {
            declaration = D3DVSD_REG(D3DVSDE_BLENDINDICES, D3DVSDT_UBYTE4);
            continue;
        }

        if (element == 'NOR3')
        {
            switch (size)
            {
            case sizeof(char) * 4:
                declaration = D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_UBYTE4);
                break;
            case sizeof(float) * 3:
                declaration = D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3);
                break;
            }
            continue;
        }

        if (element == 'COL4' && size == sizeof(char) * 4)
        {
            declaration = D3DVSD_REG(D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR);
            continue;
        }

        if (element == 'TEX2' && size == sizeof(float) * 2)
        {
            declaration = D3DVSD_REG(D3DVSDE_TEXCOORD0 + textureIndex, D3DVSDT_FLOAT2);
            textureIndex++;
            continue;
        }
    }

    d3dVertexAttribute->declaration[count + 1] = D3DVSD_END();
    d3dVertexAttribute->stride = stride;

    return reinterpret_cast<uint64_t>(d3dVertexAttribute);
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeD3D8PS(uint64_t vertexAttribute)
{
    D3DVERTEXATTRIBUTE8PS* d3dVertexAttribute = reinterpret_cast<D3DVERTEXATTRIBUTE8PS*>(vertexAttribute);

    xxFree(d3dVertexAttribute);
}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderD3D8PS(uint64_t device, char const* shader, uint64_t vertexAttribute)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3DVERTEXATTRIBUTE8PS* d3dVertexAttribute = reinterpret_cast<D3DVERTEXATTRIBUTE8PS*>(vertexAttribute);
    if (d3dVertexAttribute == nullptr)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        char* d3dShader = xxAlloc(char, 16 + vertexShaderCode10Size);
        if (d3dShader == nullptr)
            return 0;
        memcpy(d3dShader + 16, vertexShaderCode10, vertexShaderCode10Size);

        DWORD handle = 0;
        d3dDevice->CreateVertexShader(d3dVertexAttribute->declaration, vertexShaderCode10, &handle, 0);
        handle = handle ? handle + 1 : 0;
        memcpy(d3dShader, &handle, sizeof(handle));

        return reinterpret_cast<uint64_t>(d3dShader);
    }
    else
    {
        static char const* const macro[] = { "SHADER_HLSL", "8",
                                             "SHADER_VERTEX", "1",
                                             nullptr, nullptr };
        ID3D10Blob* blob = D3DCompileShader(shader, macro, "Main", "vs_1_1");
        if (blob == nullptr)
            return 0;

        D3DDowngradeShader(blob, "vs_1_0");
#if defined(_DEBUG)
        D3DDisassembleShader(blob);
#endif

        char* d3dShader = xxAlloc(char, 16 + blob->GetBufferSize());
        if (d3dShader == nullptr)
            return 0;
        memcpy(d3dShader + 16, blob->GetBufferPointer(), blob->GetBufferSize());

        DWORD handle = 0;
        d3dDevice->CreateVertexShader(d3dVertexAttribute->declaration, (DWORD*)blob->GetBufferPointer(), &handle, 0);
        handle = handle ? handle + 1 : 0;
        memcpy(d3dShader, &handle, sizeof(handle));

        return reinterpret_cast<uint64_t>(d3dShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderD3D8PS(uint64_t device, char const* shader)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(device);
    if (d3dDevice == nullptr)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        char* d3dShader = xxAlloc(char, 16 + pixelShaderCode10Size);
        if (d3dShader == nullptr)
            return 0;
        memcpy(d3dShader + 16, pixelShaderCode10, pixelShaderCode10Size);

        DWORD handle = 0;
        d3dDevice->CreatePixelShader(pixelShaderCode10, &handle);
        handle = handle ? handle + 2 : 0;
        memcpy(d3dShader, &handle, sizeof(handle));

        return reinterpret_cast<uint64_t>(d3dShader);
    }
    else
    {
        static char const* const macro[] = { "SHADER_HLSL", "8",
                                             "SHADER_FRAGMENT", "1",
                                             nullptr, nullptr };
        ID3D10Blob* blob = D3DCompileShader(shader, macro, "Main", "ps_2_0");
        if (blob == nullptr)
            return 0;

        D3DDowngradeShader(blob, "ps_1_0");
#if defined(_DEBUG)
        D3DDisassembleShader(blob);
#endif

        char* d3dShader = xxAlloc(char, 16 + blob->GetBufferSize());
        if (d3dShader == nullptr)
            return 0;
        memcpy(d3dShader + 16, blob->GetBufferPointer(), blob->GetBufferSize());

        DWORD handle = 0;
        d3dDevice->CreatePixelShader((DWORD*)blob->GetBufferPointer(), &handle);
        handle = handle ? handle + 2 : 0;
        memcpy(d3dShader, &handle, sizeof(handle));

        return reinterpret_cast<uint64_t>(d3dShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderD3D8PS(uint64_t device, uint64_t shader)
{
    DWORD* d3dShader = reinterpret_cast<DWORD*>(shader);

    if (d3dShader && d3dShader[0])
    {
        LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(device);
        if (d3dDevice)
        {
            switch (d3dShader[0] & 3)
            {
            case 1:
                d3dDevice->DeleteVertexShader(d3dShader[0] & ~3);
                break;
            case 2:
                d3dDevice->DeletePixelShader(d3dShader[0] & ~3);
                break;
            }
        }
    }

    xxFree(d3dShader);
}
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t xxCreatePipelineD3D8PS(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t meshShader, uint64_t vertexShader, uint64_t fragmentShader)
{
    DWORD* d3dVertexShader = reinterpret_cast<DWORD*>(vertexShader);
    if (d3dVertexShader == nullptr)
        return 0;
    DWORD* d3dPixelShader = reinterpret_cast<DWORD*>(fragmentShader);
    if (d3dPixelShader == nullptr)
        return 0;

    uint64_t pipeline = xxCreatePipelineD3D8(device, renderPass, blendState, depthStencilState, rasterizerState, 0, 0, 0, 0);
    D3DPIPELINE8* d3dPipeline = reinterpret_cast<D3DPIPELINE8*>(pipeline);
    if (d3dPipeline == nullptr)
        return 0;

    d3dPipeline->vertexShader = d3dVertexShader[0] & ~3;
    d3dPipeline->pixelShader = d3dPixelShader[0] & ~3;

    return pipeline;
}
//------------------------------------------------------------------------------
void xxDestroyPipelineD3D8PS(uint64_t pipeline)
{
    D3DPIPELINE8* d3dPipeline = reinterpret_cast<D3DPIPELINE8*>(pipeline);

    xxFree(d3dPipeline);
}
//==============================================================================
//  Command
//==============================================================================
void xxSetScissorD3D8PS(uint64_t commandEncoder, int x, int y, int width, int height)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandEncoder);

    D3DVIEWPORT8 vp;
    d3dDevice->GetViewport(&vp);

    D3DMATRIX projection;
    d3dDevice->GetVertexShaderConstant(8, &projection, 4);
    ViewportFromScissor(projection.m, vp.X, vp.Y, vp.Width, vp.Height, x, y, width, height);
    d3dDevice->SetVertexShaderConstant(8, &projection, 4);

    vp.X = x;
    vp.Y = y;
    vp.Width = width;
    vp.Height = height;
    d3dDevice->SetViewport(&vp);
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersD3D8PS(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandEncoder);
    D3DVERTEXATTRIBUTE8PS* d3dVertexAttribute = reinterpret_cast<D3DVERTEXATTRIBUTE8PS*>(vertexAttribute);

    for (int i = 0; i < count; ++i)
    {
        LPDIRECT3DVERTEXBUFFER8 d3dVertexBuffer = reinterpret_cast<LPDIRECT3DVERTEXBUFFER8>(getResourceData(buffers[i]));
        d3dDevice->SetStreamSource(i, d3dVertexBuffer, d3dVertexAttribute->stride);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferD3D8PS(uint64_t commandEncoder, uint64_t buffer, int size)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandEncoder);
    const float* d3dBuffer = reinterpret_cast<float*>(buffer);

    d3dDevice->SetVertexShaderConstant(0, d3dBuffer, size / sizeof(float) / 4);
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferD3D8PS(uint64_t commandEncoder, uint64_t buffer, int size)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(commandEncoder);
    const float* d3dBuffer = reinterpret_cast<float*>(buffer);

    d3dDevice->SetPixelShaderConstant(0, d3dBuffer, size / sizeof(float) / 4);
}
//==============================================================================
