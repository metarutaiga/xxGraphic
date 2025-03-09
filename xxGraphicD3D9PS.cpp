//==============================================================================
// xxGraphic : Direct3D 9.0 Programmable Shader Source
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxSystem.h"
#include "dxsdk/d3d9.h"
#include "dxsdk/d3dcommon.h"
#include "internal/xxGraphicInternalD3D.h"
#include "xxGraphicD3DAsm.h"
#include "xxGraphicD3D9PS.h"

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D9PS()
{
    uint64_t instance = xxCreateInstanceD3D9();
    if (instance == 0)
        return 0;

    xxRegisterFunctionSingle(xxCreateInstance, xxCreateInstanceD3D9PS);
    xxRegisterFunctionSingle(xxGetInstanceName, xxGetInstanceNameD3D9PS);

    xxRegisterFunctionSingle(xxCreateVertexAttribute, xxCreateVertexAttributeD3D9PS);
    xxRegisterFunctionSingle(xxDestroyVertexAttribute, xxDestroyVertexAttributeD3D9PS);

    xxRegisterFunctionSingle(xxCreateVertexShader, xxCreateVertexShaderD3D9PS);
    xxRegisterFunctionSingle(xxCreateFragmentShader, xxCreateFragmentShaderD3D9PS);
    xxRegisterFunctionSingle(xxDestroyShader, xxDestroyShaderD3D9PS);

    xxRegisterFunctionSingle(xxCreatePipeline, xxCreatePipelineD3D9PS);
    xxRegisterFunctionSingle(xxDestroyPipeline, xxDestroyPipelineD3D9PS);

    xxRegisterFunctionSingle(xxSetVertexBuffers, xxSetVertexBuffersD3D9PS);
    xxRegisterFunctionSingle(xxSetVertexConstantBuffer, xxSetVertexConstantBufferD3D9PS);
    xxRegisterFunctionSingle(xxSetFragmentConstantBuffer, xxSetFragmentConstantBufferD3D9PS);

    return instance;
}
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t xxCreateVertexAttributeD3D9PS(uint64_t device, int count, int* attribute)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3DVERTEXATTRIBUTE9PS* d3dVertexAttribute = xxAlloc(D3DVERTEXATTRIBUTE9PS);
    if (d3dVertexAttribute == nullptr)
        return 0;

    D3DVERTEXELEMENT9 vertexElements[32];
    int normalIndex = 0;
    int textureIndex = 0;
    int stride = 0;

    for (int i = 0; i < count; ++i)
    {
        int stream = (*attribute++);
        int offset = (*attribute++);
        int element = (*attribute++);
        int size = (*attribute++);

        stride += size;

        D3DVERTEXELEMENT9& vertexElement = vertexElements[i];
        vertexElement.Stream = stream;
        vertexElement.Offset = offset;
        vertexElement.Type = D3DDECLTYPE_FLOAT1;
        vertexElement.Method = D3DDECLMETHOD_DEFAULT;
        vertexElement.Usage;
        vertexElement.UsageIndex = 0;

        if (element == 'POS3' && size == sizeof(float) * 3)
        {
            vertexElement.Type = D3DDECLTYPE_FLOAT3;
            vertexElement.Usage = D3DDECLUSAGE_POSITION;
            continue;
        }

        if (element == 'BON3' && size == sizeof(float) * 3)
        {
            vertexElement.Type = D3DDECLTYPE_FLOAT3;
            vertexElement.Usage = D3DDECLUSAGE_BLENDWEIGHT;
            continue;
        }

        if (element == 'BON4' && size == sizeof(char) * 4)
        {
            vertexElement.Type = D3DDECLTYPE_UBYTE4;
            vertexElement.Usage = D3DDECLUSAGE_BLENDINDICES;
            continue;
        }

        if (element == 'NOR3')
        {
            switch (size)
            {
            case sizeof(char) * 4:
                vertexElement.Type = D3DDECLTYPE_UBYTE4;
                break;
            case sizeof(float) * 3:
                vertexElement.Type = D3DDECLTYPE_FLOAT3;
                break;
            }
            switch (normalIndex)
            {
            case 0: vertexElement.Usage = D3DDECLUSAGE_NORMAL; break;
            case 1: vertexElement.Usage = D3DDECLUSAGE_TANGENT; break;
            case 2: vertexElement.Usage = D3DDECLUSAGE_BINORMAL; break;
            }
            normalIndex++;
            continue;
        }

        if (element == 'COL4' && size == sizeof(char) * 4)
        {
            vertexElement.Type = D3DDECLTYPE_D3DCOLOR;
            vertexElement.Usage = D3DDECLUSAGE_COLOR;
            continue;
        }

        if (element == 'TEX2' && size == sizeof(float) * 2)
        {
            vertexElement.Type = D3DDECLTYPE_FLOAT2;
            vertexElement.Usage = D3DDECLUSAGE_TEXCOORD;
            vertexElement.UsageIndex = textureIndex;
            textureIndex++;
            continue;
        }
    }

    D3DVERTEXELEMENT9 end = D3DDECL_END();
    vertexElements[count] = end;

    LPDIRECT3DVERTEXDECLARATION9 vertexDeclaration = nullptr;
    HRESULT hResult = d3dDevice->CreateVertexDeclaration(vertexElements, &vertexDeclaration);
    if (hResult != S_OK)
    {
        xxFree(d3dVertexAttribute);
        return 0;
    }
    d3dVertexAttribute->vertexDeclaration = vertexDeclaration;
    d3dVertexAttribute->stride = stride;

    return reinterpret_cast<uint64_t>(d3dVertexAttribute);
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeD3D9PS(uint64_t vertexAttribute)
{
    D3DVERTEXATTRIBUTE9PS* d3dVertexAttribute = reinterpret_cast<D3DVERTEXATTRIBUTE9PS*>(vertexAttribute);
    if (d3dVertexAttribute == nullptr)
        return;

    d3dVertexAttribute->vertexDeclaration->Release();
    xxFree(d3dVertexAttribute);
}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderD3D9PS(uint64_t device, char const* shader, uint64_t vertexAttribute)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3DVERTEXATTRIBUTE9PS* d3dVertexAttribute = reinterpret_cast<D3DVERTEXATTRIBUTE9PS*>(vertexAttribute);
    if (d3dVertexAttribute == nullptr)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        char* d3dShader = xxAlloc(char, 16 + vertexShaderCode11Size);
        if (d3dShader == nullptr)
            return 0;
        memcpy(d3dShader + 16, vertexShaderCode11, vertexShaderCode11Size);

        IDirect3DVertexShader9* vertexShader = nullptr;
        d3dDevice->CreateVertexShader(vertexShaderCode11, &vertexShader);
        memcpy(d3dShader, &vertexShader, sizeof(vertexShader));

        return reinterpret_cast<uint64_t>(d3dShader);
    }
    else
    {
        static char const* const macro[] = { "SHADER_HLSL", "9", 
                                             "SHADER_VERTEX", "1",
                                             nullptr, nullptr };
        ID3D10Blob* blob = D3DCompileShader(shader, macro, "Main", "vs_3_0");
        if (blob == nullptr)
            return 0;

        char* d3dShader = xxAlloc(char, 16 + blob->GetBufferSize());
        if (d3dShader == nullptr)
            return 0;
        memcpy(d3dShader + 16, blob->GetBufferPointer(), blob->GetBufferSize());

        IDirect3DVertexShader9* vertexShader = nullptr;
        d3dDevice->CreateVertexShader((DWORD*)blob->GetBufferPointer(), &vertexShader);
        memcpy(d3dShader, &vertexShader, sizeof(vertexShader));

        return reinterpret_cast<uint64_t>(d3dShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderD3D9PS(uint64_t device, char const* shader)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        char* d3dShader = xxAlloc(char, 16 + pixelShaderCode10Size);
        if (d3dShader == nullptr)
            return 0;
        memcpy(d3dShader + 16, pixelShaderCode10, pixelShaderCode10Size);

        IDirect3DPixelShader9* pixelShader = nullptr;
        d3dDevice->CreatePixelShader(pixelShaderCode10, &pixelShader);
        memcpy(d3dShader, &pixelShader, sizeof(pixelShader));

        return reinterpret_cast<uint64_t>(d3dShader);
    }
    else
    {
        static char const* const macro[] = { "SHADER_HLSL", "9",
                                             "SHADER_FRAGMENT", "1",
                                             nullptr, nullptr };
        ID3D10Blob* blob = D3DCompileShader(shader, macro, "Main", "ps_3_0");
        if (blob == nullptr)
            return 0;

        char* d3dShader = xxAlloc(char, 16 + blob->GetBufferSize());
        if (d3dShader == nullptr)
            return 0;
        memcpy(d3dShader + 16, blob->GetBufferPointer(), blob->GetBufferSize());

        IDirect3DPixelShader9* pixelShader = nullptr;
        d3dDevice->CreatePixelShader((DWORD*)blob->GetBufferPointer(), &pixelShader);
        memcpy(d3dShader, &pixelShader, sizeof(pixelShader));

        return reinterpret_cast<uint64_t>(d3dShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderD3D9PS(uint64_t device, uint64_t shader)
{
    void** d3dShader = reinterpret_cast<void**>(shader);

    if (d3dShader && d3dShader[0])
    {
        IUnknown* unknown = reinterpret_cast<IUnknown*>(d3dShader[0]);
        SafeRelease(unknown);
    }

    xxFree(d3dShader);
}
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t xxCreatePipelineD3D9PS(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t meshShader, uint64_t vertexShader, uint64_t fragmentShader)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3DVERTEXATTRIBUTE9PS* d3dVertexAttribute = reinterpret_cast<D3DVERTEXATTRIBUTE9PS*>(vertexAttribute);
    if (d3dVertexAttribute == nullptr)
        return 0;
    IDirect3DVertexShader9* d3dVertexShader = reinterpret_cast<IDirect3DVertexShader9*>(vertexShader);
    if (d3dVertexShader == nullptr)
        return 0;
    IDirect3DPixelShader9* d3dPixelShader = reinterpret_cast<IDirect3DPixelShader9*>(fragmentShader);
    if (d3dPixelShader == nullptr)
        return 0;

    uint64_t pipeline = xxCreatePipelineD3D9(device, renderPass, blendState, depthStencilState, rasterizerState, 0, 0, 0, 0);
    D3DPIPELINE9* d3dPipeline = reinterpret_cast<D3DPIPELINE9*>(pipeline);
    if (d3dPipeline == nullptr)
        return 0;

    d3dPipeline->vertexDeclaration = d3dVertexAttribute->vertexDeclaration;
    d3dPipeline->vertexShader = d3dVertexShader;
    d3dPipeline->pixelShader = d3dPixelShader;
    SafeAddRef(d3dPipeline->vertexDeclaration);
    SafeAddRef(d3dPipeline->vertexShader);
    SafeAddRef(d3dPipeline->pixelShader);

    return pipeline;
}
//------------------------------------------------------------------------------
void xxDestroyPipelineD3D9PS(uint64_t pipeline)
{
    D3DPIPELINE9* d3dPipeline = reinterpret_cast<D3DPIPELINE9*>(pipeline);
    if (d3dPipeline == nullptr)
        return;

    SafeRelease(d3dPipeline->vertexDeclaration);
    SafeRelease(d3dPipeline->vertexShader);
    SafeRelease(d3dPipeline->pixelShader);
    xxFree(d3dPipeline);
}
//==============================================================================
//  Command
//==============================================================================
void xxSetVertexBuffersD3D9PS(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandEncoder);
    D3DVERTEXATTRIBUTE9PS* d3dVertexAttribute = reinterpret_cast<D3DVERTEXATTRIBUTE9PS*>(vertexAttribute);

    for (int i = 0; i < count; ++i)
    {
        LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer = reinterpret_cast<LPDIRECT3DVERTEXBUFFER9>(getResourceData(buffers[i]));
        d3dDevice->SetStreamSource(i, d3dVertexBuffer, 0, d3dVertexAttribute->stride);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferD3D9PS(uint64_t commandEncoder, uint64_t buffer, int size)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandEncoder);
    const float* d3dBuffer = reinterpret_cast<float*>(buffer);

    d3dDevice->SetVertexShaderConstantF(0, d3dBuffer, size / sizeof(float) / 4);
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferD3D9PS(uint64_t commandEncoder, uint64_t buffer, int size)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandEncoder);
    const float* d3dBuffer = reinterpret_cast<float*>(buffer);

    d3dDevice->SetPixelShaderConstantF(0, d3dBuffer, size / sizeof(float) / 4);
}
//==============================================================================
