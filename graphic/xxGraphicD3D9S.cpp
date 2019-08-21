#include "xxGraphicD3D9S.h"
#include "xxGraphicD3DAsm.h"
#include "xxGraphicInternal.h"

#include "dxsdk/d3d9.h"

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D9S()
{
    uint64_t instance = xxCreateInstanceD3D9();
    if (instance == 0)
        return 0;

    xxCreateInstance = xxCreateInstanceD3D9S;
    xxGetDeviceString = xxGetDeviceStringD3D9S;
    xxCreateVertexAttribute = xxCreateVertexAttributeD3D9S;
    xxDestroyVertexAttribute = xxDestroyVertexAttributeD3D9S;
    xxCreateVertexShader = xxCreateVertexShaderD3D9S;
    xxCreateFragmentShader = xxCreateFragmentShaderD3D9S;
    xxDestroyShader = xxDestroyShaderD3D9S;
    xxSetVertexAttribute = xxSetVertexAttributeD3D9S;
    xxSetVertexShader = xxSetVertexShaderD3D9S;
    xxSetFragmentShader = xxSetFragmentShaderD3D9S;
    xxSetVertexConstantBuffer = xxSetVertexConstantBufferD3D9S;
    xxSetFragmentConstantBuffer = xxSetFragmentConstantBufferD3D9S;
    xxSetTransform = xxSetTransformD3D9S;

    return instance;
}
//==============================================================================
//  Device
//==============================================================================
const char* xxGetDeviceStringD3D9S(uint64_t device)
{
    return "Direct3D 9.0 Shader";
}
//==============================================================================
//  Vertex Attribute
//==============================================================================
struct D3DVERTEXATTRIBUTE9
{
    LPDIRECT3DVERTEXDECLARATION9 vertexDeclaration;
    int stride;
};
//------------------------------------------------------------------------------
uint64_t xxCreateVertexAttributeD3D9S(uint64_t device, int count, ...)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(device);
    if (d3dDevice == nullptr)
        return 0;
    D3DVERTEXATTRIBUTE9* d3dVertexAttribute = new D3DVERTEXATTRIBUTE9;
    if (d3dVertexAttribute == nullptr)
        return 0;
    d3dVertexAttribute->stride = 0;

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

    vertexElements[count] = D3DDECL_END();

    HRESULT hResult = d3dDevice->CreateVertexDeclaration(vertexElements, &d3dVertexAttribute->vertexDeclaration);
    if (hResult != S_OK)
    {
        delete d3dVertexAttribute;
        return 0;
    }
    d3dVertexAttribute->stride = stride;

    return reinterpret_cast<uint64_t>(d3dVertexAttribute);
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeD3D9S(uint64_t vertexAttribute)
{
    D3DVERTEXATTRIBUTE9* d3dVertexAttribute = reinterpret_cast<D3DVERTEXATTRIBUTE9*>(vertexAttribute);
    if (d3dVertexAttribute == nullptr)
        return;

    delete d3dVertexAttribute;
}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderD3D9S(uint64_t device, const char* shader, uint64_t vertexAttribute)
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
uint64_t xxCreateFragmentShaderD3D9S(uint64_t device, const char* shader)
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
void xxDestroyShaderD3D9S(uint64_t device, uint64_t shader)
{
    LPUNKNOWN d3dShader = reinterpret_cast<LPUNKNOWN>(shader);
    if (d3dShader == nullptr)
        return;

    d3dShader->Release();
}
//==============================================================================
//  Command
//==============================================================================
void xxSetVertexAttributeD3D9S(uint64_t commandBuffer, uint64_t vertexAttribute)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;
    D3DVERTEXATTRIBUTE9* d3dVertexAttribtue = reinterpret_cast<D3DVERTEXATTRIBUTE9*>(vertexAttribute);
    if (d3dVertexAttribtue == nullptr)
        return;

    d3dDevice->SetVertexDeclaration(d3dVertexAttribtue->vertexDeclaration);

    for (int i = 0; i < 8; ++i)
    {
        LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer = nullptr;
        UINT offset;
        UINT stride;
        d3dDevice->GetStreamSource(i, &d3dVertexBuffer, &offset, &stride);
        if (d3dVertexBuffer == nullptr)
            break;
        d3dDevice->SetStreamSource(i, d3dVertexBuffer, 0, d3dVertexAttribtue->stride);
        d3dVertexBuffer->Release();
    }
}
//------------------------------------------------------------------------------
void xxSetVertexShaderD3D9S(uint64_t commandBuffer, uint64_t shader)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;
    LPDIRECT3DVERTEXSHADER9 d3dShader = reinterpret_cast<LPDIRECT3DVERTEXSHADER9>(shader);

    d3dDevice->SetVertexShader(d3dShader);
}
//------------------------------------------------------------------------------
void xxSetFragmentShaderD3D9S(uint64_t commandBuffer, uint64_t shader)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;
    LPDIRECT3DPIXELSHADER9 d3dShader = reinterpret_cast<LPDIRECT3DPIXELSHADER9>(shader);

    d3dDevice->SetPixelShader(d3dShader);
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferD3D9S(uint64_t commandBuffer, uint64_t buffer, unsigned int size)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;
    const float* d3dBuffer = reinterpret_cast<float*>(buffer);

    d3dDevice->SetVertexShaderConstantF(0, d3dBuffer, size / sizeof(float) / 4);
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferD3D9S(uint64_t commandBuffer, uint64_t buffer, unsigned int size)
{
    LPDIRECT3DDEVICE9 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE9>(commandBuffer);
    if (d3dDevice == nullptr)
        return;
    const float* d3dBuffer = reinterpret_cast<float*>(buffer);

    d3dDevice->SetPixelShaderConstantF(0, d3dBuffer, size / sizeof(float) / 4);
}
//==============================================================================
//  Fixed-Function
//==============================================================================
void xxSetTransformD3D9S(uint64_t commandBuffer, const float* world, const float* view, const float* projection)
{

}
//==============================================================================
