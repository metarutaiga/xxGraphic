//==============================================================================
// xxGraphic : Direct3D 9.0 Programmable Shader Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphicInternal.h"
#include "xxGraphicD3D.h"
#include "xxGraphicD3DAsm.h"
#include "xxGraphicD3D9PS.h"

#include "dxsdk/d3d9.h"

//==============================================================================
//  Resource Type
//==============================================================================
static uint64_t getResourceType(uint64_t resource)
{
    return resource & 7ull;
}
//------------------------------------------------------------------------------
static uint64_t getResourceData(uint64_t resource)
{
    return resource & ~7ull;
}
//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D9PS()
{
    uint64_t instance = xxCreateInstanceD3D9();
    if (instance == 0)
        return 0;

    xxCreateInstance = xxCreateInstanceD3D9PS;
    xxGetDeviceName = xxGetDeviceNameD3D9PS;
    xxCreateVertexAttribute = xxCreateVertexAttributeD3D9PS;
    xxDestroyVertexAttribute = xxDestroyVertexAttributeD3D9PS;
    xxCreateVertexShader = xxCreateVertexShaderD3D9PS;
    xxCreateFragmentShader = xxCreateFragmentShaderD3D9PS;
    xxDestroyShader = xxDestroyShaderD3D9PS;
    xxCreatePipeline = xxCreatePipelineD3D9PS;
    xxSetVertexBuffers = xxSetVertexBuffersD3D9PS;
    xxSetVertexConstantBuffer = xxSetVertexConstantBufferD3D9PS;
    xxSetFragmentConstantBuffer = xxSetFragmentConstantBufferD3D9PS;
    xxSetTransform = xxSetTransformD3D9PS;

    return instance;
}
//==============================================================================
//  Device
//==============================================================================
const char* xxGetDeviceNameD3D9PS()
{
    return "Direct3D 9.0 Programmable Shader";
}
//==============================================================================
//  Vertex Attribute
//==============================================================================
struct D3DVERTEXATTRIBUTE9PS
{
    LPDIRECT3DVERTEXDECLARATION9    vertexDeclaration;
    int                             stride;
};
//------------------------------------------------------------------------------
uint64_t xxCreateVertexAttributeD3D9PS(uint64_t device, int count, ...)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3DVERTEXATTRIBUTE9PS* d3dVertexAttribute = new D3DVERTEXATTRIBUTE9PS;
    if (d3dVertexAttribute == nullptr)
        return 0;

    D3DVERTEXELEMENT9 vertexElements[32];
    BYTE xyzIndex = 0;
    BYTE textureIndex = 0;
    int stride = 0;

    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; ++i)
    {
        int stream = va_arg(args, int);
        int offset = va_arg(args, int);
        int element = va_arg(args, int);
        int size = va_arg(args, int);

        stride += size;

        D3DVERTEXELEMENT9& vertexElement = vertexElements[i];
        vertexElement.Stream = stream;
        vertexElement.Offset = offset;
        vertexElement.Type = D3DDECLTYPE_FLOAT1;
        vertexElement.Method = D3DDECLMETHOD_DEFAULT;
        vertexElement.Usage = D3DDECLUSAGE_POSITION;
        vertexElement.UsageIndex = 0;

        if (element == 3 && size == sizeof(float) * 3)
        {
            vertexElement.Type = D3DDECLTYPE_FLOAT3;
            switch (xyzIndex)
            {
            case 0:
                vertexElement.Usage = D3DDECLUSAGE_POSITION;
                vertexElement.UsageIndex = 0;
                break;
            case 1:
                vertexElement.Usage = D3DDECLUSAGE_NORMAL;
                vertexElement.UsageIndex = 0;
                break;
            case 2:
                vertexElement.Usage = D3DDECLUSAGE_BINORMAL;
                vertexElement.UsageIndex = 0;
                break;
            case 3:
                vertexElement.Usage = D3DDECLUSAGE_TANGENT;
                vertexElement.UsageIndex = 0;
                break;
            default:
                break;
            }
            xyzIndex++;
            continue;
        }

        if (element == 4 && size == sizeof(char) * 4)
        {
            vertexElement.Type = D3DDECLTYPE_D3DCOLOR;
            vertexElement.Usage = D3DDECLUSAGE_COLOR;
            vertexElement.UsageIndex = 0;
            continue;
        }

        if (element == 2 && size == sizeof(float) * 2)
        {
            vertexElement.Type = D3DDECLTYPE_FLOAT2;
            vertexElement.Usage = D3DDECLUSAGE_TEXCOORD;
            vertexElement.UsageIndex = textureIndex;
            textureIndex++;
            continue;
        }
    }
    va_end(args);

    D3DVERTEXELEMENT9 end = D3DDECL_END();
    vertexElements[count] = end;

    LPDIRECT3DVERTEXDECLARATION9 vertexDeclaration = nullptr;
    HRESULT hResult = d3dDevice->CreateVertexDeclaration(vertexElements, &vertexDeclaration);
    if (hResult != S_OK)
    {
        delete d3dVertexAttribute;
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
    delete d3dVertexAttribute;
}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderD3D9PS(uint64_t device, const char* shader, uint64_t vertexAttribute)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        LPDIRECT3DVERTEXSHADER9 d3dShader = nullptr;
        HRESULT hResult = d3dDevice->CreateVertexShader(vertexShaderCode11, &d3dShader);
        if (hResult != S_OK)
            return 0;

        return reinterpret_cast<uint64_t>(d3dShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderD3D9PS(uint64_t device, const char* shader)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        LPDIRECT3DPIXELSHADER9 d3dShader = nullptr;
        HRESULT hResult = d3dDevice->CreatePixelShader(pixelShaderCode11, &d3dShader);
        if (hResult != S_OK)
            return 0;

        return reinterpret_cast<uint64_t>(d3dShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderD3D9PS(uint64_t device, uint64_t shader)
{
    LPUNKNOWN d3dShader = reinterpret_cast<LPUNKNOWN>(shader);

    SafeRelease(d3dShader);
}
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t xxCreatePipelineD3D9PS(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    uint64_t pipeline = xxCreatePipelineD3D9(device, renderPass, blendState, depthStencilState, rasterizerState, 0, vertexShader, fragmentShader);
    if (pipeline == 0)
        return pipeline;
    D3DVERTEXATTRIBUTE9PS* d3dVertexAttribute = reinterpret_cast<D3DVERTEXATTRIBUTE9PS*>(vertexAttribute);
    if (d3dVertexAttribute == nullptr)
        return pipeline;
    LPDIRECT3DVERTEXDECLARATION9* d3dVertexDeclaration = reinterpret_cast<LPDIRECT3DVERTEXDECLARATION9*>(pipeline);

    (*d3dVertexDeclaration) = d3dVertexAttribute->vertexDeclaration;

    return pipeline;
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
void xxSetVertexConstantBufferD3D9PS(uint64_t commandEncoder, uint64_t buffer, unsigned int size)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandEncoder);
    const float* d3dBuffer = reinterpret_cast<float*>(buffer);

    d3dDevice->SetVertexShaderConstantF(0, d3dBuffer, size / sizeof(float) / 4);
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferD3D9PS(uint64_t commandEncoder, uint64_t buffer, unsigned int size)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandEncoder);
    const float* d3dBuffer = reinterpret_cast<float*>(buffer);

    d3dDevice->SetPixelShaderConstantF(0, d3dBuffer, size / sizeof(float) / 4);
}
//==============================================================================
//  Fixed-Function
//==============================================================================
void xxSetTransformD3D9PS(uint64_t commandEncoder, const float* world, const float* view, const float* projection)
{

}
//==============================================================================
