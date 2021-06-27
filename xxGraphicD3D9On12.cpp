//==============================================================================
// xxGraphic : Direct3D 9On12 Source
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#if defined(_DEBUG)
#define D3D_DEBUG_INFO 1
#endif
#include "xxSystem.h"
#include "dxsdk/d3d9on12.h"
#include "internal/xxGraphicInternal.h"
#include "internal/xxGraphicInternalD3D.h"
#include "xxGraphicD3D9.h"
#include "xxGraphicD3D9PS.h"
#include "xxGraphicD3D9On12.h"


static void*                        g_d3dLibrary = nullptr;

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D9On12()
{
    if (g_d3dLibrary == nullptr)
        g_d3dLibrary = xxLoadLibrary("d3d9.dll");
    if (g_d3dLibrary == nullptr)
        return 0;

    PFN_Direct3DCreate9On12 Direct3DCreate9On12;
    (void*&)Direct3DCreate9On12 = xxGetProcAddress(g_d3dLibrary, MAKEINTRESOURCEA(20)/*"Direct3DCreate9On12"*/);
    if (Direct3DCreate9On12 == nullptr)
        return 0;

    D3D9ON12_ARGS args = {};
    args.Enable9On12 = TRUE;

    LPDIRECT3D9 d3d = Direct3DCreate9On12(D3D_SDK_VERSION, &args, 1);
    if (d3d == nullptr)
        return 0;

    xxRegisterFunction(D3D9);

    xxDestroyInstance = xxDestroyInstanceD3D9On12;
    xxGetDeviceName = xxGetDeviceNameD3D9On12;

    return reinterpret_cast<uint64_t>(d3d);
}
//------------------------------------------------------------------------------
uint64_t xxCreateInstanceD3D9On12PS()
{
    uint64_t instance = xxCreateInstanceD3D9On12();
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

    xxDestroyInstance = xxDestroyInstanceD3D9On12;
    xxGetDeviceName = xxGetDeviceNameD3D9On12PS;

    return instance;
}
//------------------------------------------------------------------------------
uint64_t xxCreateInstanceD3D9On12Ex()
{
    if (g_d3dLibrary == nullptr)
        g_d3dLibrary = xxLoadLibrary("d3d9.dll");
    if (g_d3dLibrary == nullptr)
        return 0;

    PFN_Direct3DCreate9On12Ex Direct3DCreate9On12Ex;
    (void*&)Direct3DCreate9On12Ex = xxGetProcAddress(g_d3dLibrary, MAKEINTRESOURCEA(21)/*"Direct3DCreate9On12Ex"*/);
    if (Direct3DCreate9On12Ex == nullptr)
        return 0;

    D3D9ON12_ARGS args = {};
    args.Enable9On12 = TRUE;

    LPDIRECT3D9EX d3d = nullptr;
    HRESULT hResult = Direct3DCreate9On12Ex(D3D_SDK_VERSION, &args, 1, &d3d);
    if (hResult != S_OK)
        return 0;

    xxRegisterFunction(D3D9);

    xxTestDevice = xxTestDeviceD3D9On12Ex;
    xxDestroyInstance = xxDestroyInstanceD3D9On12;
    xxGetDeviceName = xxGetDeviceNameD3D9On12Ex;

    return reinterpret_cast<uint64_t>(d3d);
}
//------------------------------------------------------------------------------
uint64_t xxCreateInstanceD3D9On12ExPS()
{
    uint64_t instance = xxCreateInstanceD3D9On12Ex();
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

    xxTestDevice = xxTestDeviceD3D9On12Ex;
    xxDestroyInstance = xxDestroyInstanceD3D9On12;
    xxGetDeviceName = xxGetDeviceNameD3D9On12ExPS;

    return instance;
}
//------------------------------------------------------------------------------
void xxDestroyInstanceD3D9On12(uint64_t instance)
{
    LPDIRECT3D9 d3d = reinterpret_cast<LPDIRECT3D9>(instance);

    SafeRelease(d3d);

    if (g_d3dLibrary)
    {
        xxFreeLibrary(g_d3dLibrary);
        g_d3dLibrary = nullptr;
    }

    xxUnregisterFunction();
}
//==============================================================================
//  Device
//==============================================================================
bool xxTestDeviceD3D9On12Ex(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
const char* xxGetDeviceNameD3D9On12()
{
    return "Direct3D 9On12 Fixed Function";
}
//------------------------------------------------------------------------------
const char* xxGetDeviceNameD3D9On12PS()
{
    return "Direct3D 9On12 Programmable Shader";
}
//------------------------------------------------------------------------------
const char* xxGetDeviceNameD3D9On12Ex()
{
    return "Direct3D 9On12Ex Fixed Function";
}
//------------------------------------------------------------------------------
const char* xxGetDeviceNameD3D9On12ExPS()
{
    return "Direct3D 9On12Ex Programmable Shader";
}
//------------------------------------------------------------------------------
