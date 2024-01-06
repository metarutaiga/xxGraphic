//==============================================================================
// xxGraphic : Direct3D 9Ex Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#if defined(_DEBUG)
#define D3D_DEBUG_INFO 1
#endif
#include "xxSystem.h"
#include "dxsdk/d3d9.h"
#include "internal/xxGraphicInternalD3D.h"
#include "xxGraphicD3D9.h"
#include "xxGraphicD3D9PS.h"
#include "xxGraphicD3D9Ex.h"

typedef HRESULT (WINAPI *PFN_DIRECT3D_CREATE9EX)(UINT, LPDIRECT3D9EX*);

static void*                        g_d3dLibrary = nullptr;

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D9Ex()
{
    if (g_d3dLibrary == nullptr)
        g_d3dLibrary = xxLoadLibrary("d3d9.dll");
    if (g_d3dLibrary == nullptr)
        return 0;

    PFN_DIRECT3D_CREATE9EX Direct3DCreate9Ex;
    (void*&)Direct3DCreate9Ex = xxGetProcAddress(g_d3dLibrary, "Direct3DCreate9Ex");
    if (Direct3DCreate9Ex == nullptr)
        return 0;

    LPDIRECT3D9EX d3d = nullptr;
    HRESULT hResult = Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d);
    if (hResult != S_OK)
        return 0;

    xxRegisterFunction(D3D9);

    xxRegisterFunctionSingle(xxCreateInstance, xxCreateInstanceD3D9Ex);
    xxRegisterFunctionSingle(xxDestroyInstance, xxDestroyInstanceD3D9Ex);
    xxRegisterFunctionSingle(xxGetInstanceName, xxGetInstanceNameD3D9Ex);

    xxRegisterFunctionSingle(xxTestDevice, xxTestDeviceD3D9Ex);

    return reinterpret_cast<uint64_t>(d3d);
}
//------------------------------------------------------------------------------
uint64_t xxCreateInstanceD3D9ExPS()
{
    uint64_t instance = xxCreateInstanceD3D9Ex();
    if (instance == 0)
        return 0;

    xxRegisterFunctionSingle(xxCreateInstance, xxCreateInstanceD3D9ExPS);
    xxRegisterFunctionSingle(xxDestroyInstance, xxDestroyInstanceD3D9Ex);
    xxRegisterFunctionSingle(xxGetInstanceName, xxGetInstanceNameD3D9ExPS);

    xxRegisterFunctionSingle(xxTestDevice, xxTestDeviceD3D9Ex);

    xxRegisterFunctionSingle(xxCreateVertexAttribute, xxCreateVertexAttributeD3D9PS);
    xxRegisterFunctionSingle(xxDestroyVertexAttribute, xxDestroyVertexAttributeD3D9PS);

    xxRegisterFunctionSingle(xxCreateVertexShader, xxCreateVertexShaderD3D9PS);
    xxRegisterFunctionSingle(xxCreateFragmentShader, xxCreateFragmentShaderD3D9PS);
    xxRegisterFunctionSingle(xxDestroyShader, xxDestroyShaderD3D9PS);

    xxRegisterFunctionSingle(xxCreatePipeline, xxCreatePipelineD3D9PS);

    xxRegisterFunctionSingle(xxSetVertexBuffers, xxSetVertexBuffersD3D9PS);
    xxRegisterFunctionSingle(xxSetVertexConstantBuffer, xxSetVertexConstantBufferD3D9PS);
    xxRegisterFunctionSingle(xxSetFragmentConstantBuffer, xxSetFragmentConstantBufferD3D9PS);

    return instance;
}
//------------------------------------------------------------------------------
void xxDestroyInstanceD3D9Ex(uint64_t instance)
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
bool xxTestDeviceD3D9Ex(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
