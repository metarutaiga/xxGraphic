//==============================================================================
// xxGraphic : Direct3D Internal Source
//
// Copyright (c) 2019-2025 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphicInternalD3D.h"
#include <dxsdk/d3d9.h>
#include <dxsdk/d3d10.h>

//==============================================================================
struct D3DBlob : public IUnknown
{
    virtual LPCSTR STDMETHODCALLTYPE GetBufferPointer() = 0;
    virtual SIZE_T STDMETHODCALLTYPE GetBufferSize() = 0;
};
//------------------------------------------------------------------------------
static void LogText(char const* label, void const* text, size_t size, bool number)
{
    char* temp = xxAlloc(char, size + 1);
    if (temp)
    {
        memcpy(temp, text, size);
        temp[size] = 0;

        int index = 0;
        char* lasts = temp;
        char* line = strsep(&lasts, "\r\n");
        while (line)
        {
            if (number)
            {
                xxLog(label, "%d : %s", ++index, line);
            }
            else
            {
                xxLog(label, "%s", line);
            }
            line = strsep(&lasts, "\r\n");
        }

        xxFree(temp);
    }
}
//------------------------------------------------------------------------------
struct ID3D10Blob* D3DCompileShader(char const* shader, char const*const* macro, char const* entry, char const* target)
{
    static HRESULT(WINAPI * D3DCompile)(char const*, size_t, char const*, char const*const*, void*, char const*, char const*, int, int, ID3D10Blob**, ID3D10Blob**) = nullptr;
    static HRESULT(WINAPI * D3D10CompileShader)(char const*, size_t, char const*, char const*const*, void*, char const*, char const*, int, ID3D10Blob**, ID3D10Blob**) = nullptr;
    if (D3DCompile == nullptr)
    {
        (void*&)D3DCompile = GetProcAddress(LoadLibraryA("d3dcompiler_47.dll"), "D3DCompile");
    }
    if (D3DCompile == nullptr && D3D10CompileShader == nullptr)
    {
        (void*&)D3D10CompileShader = GetProcAddress(LoadLibraryA("d3d10.dll"), "D3D10CompileShader");
    }
    if (D3DCompile || D3D10CompileShader)
    {
        ID3D10Blob* blob = nullptr;
        ID3D10Blob* error = nullptr;
        if (D3DCompile)
        {
            D3DCompile(shader, strlen(shader), nullptr, macro, nullptr, entry, target, (1 << 12), 0, &blob, &error);
        }
        else if (D3D10CompileShader)
        {
            D3D10CompileShader(shader, strlen(shader), nullptr, macro, nullptr, entry, target, 0, &blob, &error);
        }
        if (error)
        {
            LogText("D3DCompileShader", shader, strlen(shader), true);
            LogText("D3DCompileShader", error->GetBufferPointer(), error->GetBufferSize(), false);
            error->Release();
        }
        return blob;
    }

    return nullptr;
}
//------------------------------------------------------------------------------
void D3DDisassembleShader(struct ID3D10Blob* shader)
{
    static HRESULT(WINAPI * D3DDisassemble)(void const*, SIZE_T, int, char const*, ID3D10Blob**) = nullptr;
    static HRESULT(WINAPI * D3D10DisassembleShader)(void const*, SIZE_T, int, char const*, ID3D10Blob**) = nullptr;
    if (D3DDisassemble == nullptr)
    {
        (void*&)D3DDisassemble = GetProcAddress(LoadLibraryA("d3dcompiler_47.dll"), "D3DDisassemble");
    }
    if (D3DDisassemble == nullptr && D3D10DisassembleShader == nullptr)
    {
        (void*&)D3D10DisassembleShader = GetProcAddress(LoadLibraryA("d3d10.dll"), "D3D10DisassembleShader");
    }
    D3DBlob* blob = (D3DBlob*)shader;
    if (D3DDisassemble || D3D10DisassembleShader)
    {
        ID3D10Blob* text = nullptr;
        if (D3DDisassemble)
        {
            D3DDisassemble(blob->GetBufferPointer(), blob->GetBufferSize(), 0, nullptr, &text);
        }
        else if (D3D10DisassembleShader)
        {
            D3D10DisassembleShader(blob->GetBufferPointer(), blob->GetBufferSize(), 0, nullptr, &text);
        }
        if (text)
        {
            LogText("D3DDisassembleShader", text->GetBufferPointer(), text->GetBufferSize(), false);
            text->Release();
        }
    }
}
//------------------------------------------------------------------------------
#define D3DVSDE_POSITION        0
#define D3DVSDE_BLENDWEIGHT     1
#define D3DVSDE_BLENDINDICES    2
#define D3DVSDE_NORMAL          3
#define D3DVSDE_PSIZE           4
#define D3DVSDE_DIFFUSE         5
#define D3DVSDE_SPECULAR        6
#define D3DVSDE_TEXCOORD0       7
#define D3DVSDE_TEXCOORD1       8
#define D3DVSDE_TEXCOORD2       9
#define D3DVSDE_TEXCOORD3       10
#define D3DVSDE_TEXCOORD4       11
#define D3DVSDE_TEXCOORD5       12
#define D3DVSDE_TEXCOORD6       13
#define D3DVSDE_TEXCOORD7       14
#define D3DVSDE_POSITION2       15
#define D3DVSDE_NORMAL2         16
//------------------------------------------------------------------------------
void D3DDowngradeShader(struct ID3D10Blob* shader, char const* target)
{
    D3DBlob* blob = (D3DBlob*)shader;

    uint8_t mapping[16] = {};
    DWORD* output = (DWORD*)blob->GetBufferPointer();
    DWORD* input = output;
    switch (xxHash(target))
    {
    case xxHash("vs_1_0"):
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
                if ((input[2] & 0xFFFFFFF0) == 0x90E40000)
                {
                    (*output++) = 0x90E40000 | mapping[input[2] & 0xF];
                }
                else
                {
                    (*output++) = input[2];
                }
                input += 3;
                break;
            case D3DSIO_DCL:
                switch (input[1] & 0xF)
                {
                case D3DDECLUSAGE_POSITION:
                    mapping[input[2] & 0xF] = D3DVSDE_POSITION;
                    break;
                case D3DDECLUSAGE_BLENDWEIGHT:
                    mapping[input[2] & 0xF] = D3DVSDE_BLENDWEIGHT;
                    break;
                case D3DDECLUSAGE_BLENDINDICES:
                    mapping[input[2] & 0xF] = D3DVSDE_BLENDINDICES;
                    break;
                case D3DDECLUSAGE_NORMAL:
                    mapping[input[2] & 0xF] = D3DVSDE_NORMAL;
                    break;
                case D3DDECLUSAGE_PSIZE:
                    mapping[input[2] & 0xF] = D3DVSDE_PSIZE;
                    break;
                case D3DDECLUSAGE_TEXCOORD:
                    mapping[input[2] & 0xF] = D3DVSDE_TEXCOORD0;
                    break;
                case D3DDECLUSAGE_COLOR:
                    mapping[input[2] & 0xF] = D3DVSDE_DIFFUSE;
                    break;
                }
                input += 3;
                break;
            case D3DSIO_COMMENT:
//              output += (input[0] >> 16) + 1;
                input += (input[0] >> 16) + 1;
                break;
            case D3DSIO_END:
                (*output++) = D3DSIO_END;
                return;
            }
        }
        break;
    case xxHash("ps_1_0"):
        (*output++) = D3DPS_VERSION(1, 0);
        input++;
        for (;;)
        {
            int count = input[0] >> D3DSI_INSTLENGTH_SHIFT;
            switch (input[0] & D3DSI_OPCODE_MASK)
            {
            default:
                (*output++) = (*input++) & ~D3DSI_INSTLENGTH_MASK;
                for (int i = 0; i < count; ++i)
                    (*output++) = (*input++);
                break;
            case D3DSIO_MOV:
                if (input[1] != 0x800F0800)
                {
                    (*output++) = D3DSIO_MOV;
                    (*output++) = input[1];
                    (*output++) = input[2];
                }
                input += count + 1;
                break;
            case D3DSIO_TEX:
                (*output++) = D3DSIO_TEX;
                (*output++) = (input[2] & ~D3DSP_DSTMOD_MASK) | D3DSP_WRITEMASK_ALL;
                (*output++) = D3DSIO_MOV;
                (*output++) = input[1];
                (*output++) = input[2];
                input += count + 1;
                break;
            case D3DSIO_DCL:
                input += count + 1;
                break;
            case D3DSIO_COMMENT:
//              output += (input[0] >> 16) + 1;
                input += (input[0] >> 16) + 1;
                break;
            case D3DSIO_END:
                (*output++) = D3DSIO_END;
                return;
            }
        }
        break;
    default:
        break;
    }
}
//------------------------------------------------------------------------------
DWORD D3DInputLayout(DWORD dxbc[256], void const* input, int count)
{
    D3D10_INPUT_ELEMENT_DESC const* inputElements = (D3D10_INPUT_ELEMENT_DESC*)input;

    DWORD dxbcChunkCount = 1;
    DWORD dxbcCount = 8 + dxbcChunkCount;
    DWORD inputSignatureCount = 4 + (6 * count) + (4 * count);
    DWORD dxbcSize = (dxbcCount + inputSignatureCount) * sizeof(DWORD);
    DWORD* dxbcChunk = dxbc + 8;
    DWORD* inputSignatureHeader = dxbc + dxbcCount;
    DWORD* inputSignature = inputSignatureHeader + 4;
    char* inputSignatureName = (char*)(inputSignature + (6 * count));

    dxbc[0] = *(DWORD*)"DXBC";
    dxbc[5] = 1;
    dxbc[6] = dxbcSize;
    dxbc[7] = dxbcChunkCount;

    dxbcChunk[0] = (DWORD)(inputSignatureHeader - dxbc) * sizeof(DWORD);

    inputSignatureHeader[0] = *(DWORD*)"ISGN";
    inputSignatureHeader[1] = (inputSignatureCount - 2) * sizeof(DWORD);
    inputSignatureHeader[2] = count;
    inputSignatureHeader[3] = 8;
    for (int i = 0; i < count; ++i)
    {
        const D3D10_INPUT_ELEMENT_DESC& inputElement = inputElements[i];
        inputSignature[0] = (DWORD)(inputSignatureName - (char*)inputSignatureHeader - 8);
        inputSignature[1] = inputElement.SemanticIndex;
        inputSignature[2] = 0;
        inputSignature[3] = 3;
        inputSignature[4] = i;
        switch (inputElement.Format)
        {
        case DXGI_FORMAT_R32_FLOAT:
            inputSignature[5] = 0x0101;
            break;
        case DXGI_FORMAT_R32G32_FLOAT:
            inputSignature[5] = 0x0303;
            break;
        case DXGI_FORMAT_R32G32B32_FLOAT:
            inputSignature[5] = 0x0707;
            break;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        default:
            inputSignature[5] = 0x0F0F;
            break;
        }
        strcpy(inputSignatureName, inputElement.SemanticName);
        inputSignature += 6;
        inputSignatureName += strlen(inputSignatureName) + 1;
    }
    xxDXBCChecksum(dxbc, dxbcSize, (uint8_t*)&dxbc[1]);

    return dxbcSize;
}
//------------------------------------------------------------------------------
void PatchD3DIM(char const* name)
{
    void* d3dim = xxLoadLibrary(name);
    if (d3dim == nullptr)
        return;

    // https://github.com/UCyborg/LegacyD3DResolutionHack
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)d3dim;
    PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((char*)dosHeader + dosHeader->e_lfanew);
    char* codeBase = (char*)d3dim + ntHeader->OptionalHeader.BaseOfCode;
    DWORD codeSize = ntHeader->OptionalHeader.SizeOfCode;

    static BYTE const wantedBytes[] = { 0xB8, 0x00, 0x08, 0x00, 0x00, 0x39 };
    for (DWORD i = 0, size = codeSize - sizeof(wantedBytes); i < size; ++i)
    {
        if (memcmp(codeBase + i, wantedBytes, sizeof(wantedBytes)) == 0)
        {
            DWORD oldProtect;
            if (VirtualProtect(&codeBase[i + 1], 4, PAGE_EXECUTE_READWRITE, &oldProtect))
            {
                codeBase[i + 1] = -1;
                codeBase[i + 2] = -1;
                codeBase[i + 3] = -1;
                codeBase[i + 4] = -1;
                VirtualProtect(&codeBase[i + 1], 4, oldProtect, &oldProtect);
            }
        }
    }

    xxFreeLibrary(d3dim);
}
//------------------------------------------------------------------------------
void ViewportFromScissor(float projectionMatrix[4][4], int fromX, int fromY, int fromWidth, int fromHeight, int toX, int toY, int toWidth, int toHeight)
{
    float invWidth = 1.0f / toWidth;
    float invHeight = 1.0f / toHeight;
    float scaleWidth = fromWidth * invWidth;
    float scaleHeight = fromHeight * invHeight;
    float offsetWidth = scaleWidth + 2.0f * (fromX - (toX + toWidth * 0.5f)) * invWidth;
    float offsetHeight = scaleHeight + 2.0f * (fromY - (toY + toHeight * 0.5f)) * invHeight;
    projectionMatrix[0][0] = projectionMatrix[0][0] * scaleWidth;
    projectionMatrix[1][1] = projectionMatrix[1][1] * scaleHeight;
    projectionMatrix[3][0] = projectionMatrix[3][0] * scaleWidth + offsetWidth;
    projectionMatrix[3][1] = projectionMatrix[3][1] * scaleHeight - offsetHeight;
}
//==============================================================================
