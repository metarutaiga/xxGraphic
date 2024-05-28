//==============================================================================
// xxGraphic : Direct3D 8.0 Programmable Shader Source
//
// Copyright (c) 2019-2024 TAiGA
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

        if (element == 'NOR3' && size == sizeof(float) * 3)
        {
            declaration = D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3);
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
        DWORD d3dShader = 0;
        HRESULT hResult = d3dDevice->CreateVertexShader(d3dVertexAttribute->declaration, vertexShaderCode10, &d3dShader, 0);
        if (hResult != S_OK)
            return 0;

        return d3dShader | 0x100000000ull;
    }
    else
    {
        static char const* const macro[] = { "SHADER_HLSL", "8",
                                             "SHADER_VERTEX", "1",
                                             nullptr, nullptr };
        ID3D10Blob* blob = D3DCompileShader(shader, macro, "Main", "vs_1_1");
        if (blob == nullptr)
            return 0;

        struct Shader
        {
            static void Downgrade(DWORD* shader)
            {
                uint8_t mapping[16];
                DWORD* output = shader;
                DWORD* input = shader;
                memset(mapping, 0xFF, sizeof(mapping));
                (*output++) = D3DVS_VERSION(1, 0);
                input++;
                for (;;)
                {
                    switch (input[0] & D3DSI_OPCODE_MASK)
                    {
                    default:
                        (*output++) = (*input++);
                        while (input[0] & 0x80000000)
                            (*output++) = (*input++);
                        break;
                    case D3DSIO_MOV:
                        (*output++) = D3DSIO_MOV;
                        (*output++) = input[1];
                        if ((input[2] & 0xFFFFFF00) == 0x90E40000)
                        {
                            (*output++) = 0x90E40000 | mapping[input[2] & 0xF];
                        }
                        else
                        {
                            (*output++) = input[2];
                        }
                        input += 3;
                        break;
                    case 31/*D3DSIO_DCL*/:
                        switch (input[1] & 0xF)
                        {
                        case 0:// D3DDECLUSAGE_POSITION:
                            mapping[input[2] & 0xF] = D3DVSDE_POSITION;
                            break;
                        case 1:// D3DDECLUSAGE_BLENDWEIGHT:
                            mapping[input[2] & 0xF] = D3DVSDE_BLENDWEIGHT;
                            break;
                        case 2:// D3DDECLUSAGE_BLENDINDICES:
                            mapping[input[2] & 0xF] = D3DVSDE_BLENDINDICES;
                            break;
                        case 3:// D3DDECLUSAGE_NORMAL:
                            mapping[input[2] & 0xF] = D3DVSDE_NORMAL;
                            break;
                        case 4:// D3DDECLUSAGE_PSIZE:
                            mapping[input[2] & 0xF] = D3DVSDE_PSIZE;
                            break;
                        case 5:// D3DDECLUSAGE_TEXCOORD:
                            mapping[input[2] & 0xF] = D3DVSDE_TEXCOORD0;
                            break;
                        case 10:// D3DDECLUSAGE_COLOR:
                            mapping[input[2] & 0xF] = D3DVSDE_DIFFUSE;
                            break;
                        }
                        input += 3;
                        break;
                    case D3DSIO_COMMENT:
                        output += (input[0] >> 16) + 1;
                        input += (input[0] >> 16) + 1;
                        break;
                    case D3DSIO_END:
                        (*output++) = D3DSIO_END;
                        return;
                    }
                }
            }
        };
        Shader::Downgrade((DWORD*)blob->GetBufferPointer());
#if defined(_DEBUG)
        D3DDisassembleShader(blob);
#endif

        DWORD d3dShader = 0;
        HRESULT hResult = d3dDevice->CreateVertexShader(d3dVertexAttribute->declaration, (DWORD*)blob->GetBufferPointer(), &d3dShader, 0);
        blob->Release();
        if (hResult != S_OK)
            return 0;

        return d3dShader | 0x100000000ull;
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
        DWORD d3dShader = 0;
        HRESULT hResult = d3dDevice->CreatePixelShader(pixelShaderCode10, &d3dShader);
        if (hResult != S_OK)
            return 0;

        return d3dShader | 0x200000000ull;
    }
    else
    {
        static char const* const macro[] = { "SHADER_HLSL", "8",
                                             "SHADER_FRAGMENT", "1",
                                             nullptr, nullptr };
        ID3D10Blob* blob = D3DCompileShader(shader, macro, "Main", "ps_2_0");
        if (blob == nullptr)
            return 0;

        struct Shader
        {
            static void Downgrade(DWORD* shader)
            {
                int D3DSI_INSTLENGTH_MASK = 0x0F000000;
                int D3DSI_INSTLENGTH_SHIFT = 24;
                DWORD* output = shader;
                DWORD* input = shader;
                (*output++) = D3DPS_VERSION(1, 0);
                input++;
                for (;;)
                {
                    int count = input[0] >> D3DSI_INSTLENGTH_SHIFT;
                    switch (input[0] & D3DSI_OPCODE_MASK)
                    {
                    case D3DSIO_MOV:
                        if (input[1] != 0x800F0800)
                        {
                            (*output++) = D3DSIO_MOV;
                            (*output++) = input[1];
                            (*output++) = input[2];
                        }
                        input += count + 1;
                        break;
                    case D3DSIO_ADD:
                    case D3DSIO_MUL:
                        (*output++) = (*input++) & ~D3DSI_INSTLENGTH_MASK;
                        for (int i = 0; i < count; ++i)
                            (*output++) = (*input++);
                        break;
                    case D3DSIO_TEX:
                        (*output++) = D3DSIO_TEX;
                        (*output++) = input[2] & ~D3DSP_DSTMOD_MASK | D3DSP_WRITEMASK_ALL;
                        (*output++) = D3DSIO_MOV;
                        (*output++) = input[1];
                        (*output++) = input[2];
                        input += count + 1;
                        break;
                    case 31/*D3DSIO_DCL*/:
                        input += count + 1;
                        break;
                    case D3DSIO_COMMENT:
                        output += (input[0] >> 16) + 1;
                        input += (input[0] >> 16) + 1;
                        break;
                    case D3DSIO_END:
                        (*output++) = D3DSIO_END;
                        return;
                    default:
                        xxLog("xxGraphic", "Unknown Opcode (%08X)", (*input++));
                        break;
                    }
                }
            }
        };
        Shader::Downgrade((DWORD*)blob->GetBufferPointer());
#if defined(_DEBUG)
        D3DDisassembleShader(blob);
#endif

        DWORD d3dShader = 0;
        HRESULT hResult = d3dDevice->CreatePixelShader((DWORD*)blob->GetBufferPointer(), &d3dShader);
        blob->Release();
        if (hResult != S_OK)
            return 0;

        return d3dShader | 0x200000000ull;
    }

    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderD3D8PS(uint64_t device, uint64_t shader)
{
    LPDIRECT3DDEVICE8 d3dDevice = reinterpret_cast<LPDIRECT3DDEVICE8>(device);
    if (d3dDevice == nullptr)
        return;

    switch (shader & 0x300000000ull)
    {
    case 0x100000000ull:
    {
        DWORD d3dShader = static_cast<DWORD>(shader);

        d3dDevice->DeleteVertexShader(d3dShader);
        break;
    }
    case 0x200000000ull:
    {
        DWORD d3dShader = static_cast<DWORD>(shader);

        d3dDevice->DeletePixelShader(d3dShader);
        break;
    }
    default:
        break;
    }
}
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t xxCreatePipelineD3D8PS(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    return xxCreatePipelineD3D8(device, renderPass, blendState, depthStencilState, rasterizerState, 0, vertexShader, fragmentShader);
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
