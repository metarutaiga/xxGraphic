#include "xxGraphicD3D11.h"
#include "xxGraphicD3D11_1.h"
#include "xxGraphicD3DAsm.h"
#include "xxGraphicInternal.h"

#include <d3d11_1.h>

static HMODULE g_d3dLibrary = nullptr;

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D11_1()
{
    if (g_d3dLibrary == nullptr)
        g_d3dLibrary = LoadLibraryW(L"d3d11.dll");
    if (g_d3dLibrary == nullptr)
        return 0;

    xxRegisterFunction(D3D11);

    xxDestroyInstance = xxDestroyInstanceD3D11_1;
    xxCreateDevice = xxCreateDeviceD3D11_1;
    xxGetDeviceString = xxGetDeviceStringD3D11_1;

    return reinterpret_cast<uint64_t>(g_d3dLibrary);
}
//------------------------------------------------------------------------------
void xxDestroyInstanceD3D11_1(uint64_t instance)
{
    if (g_d3dLibrary)
    {
        FreeLibrary(g_d3dLibrary);
        g_d3dLibrary = nullptr;
    }

    xxUnregisterFunction();
}
//==============================================================================
//  Device
//==============================================================================
uint64_t xxCreateDeviceD3D11_1(uint64_t instance)
{
    HMODULE d3d = reinterpret_cast<HMODULE>(instance);
    if (d3d == nullptr)
        return 0;

    PFN_D3D11_CREATE_DEVICE D3D11CreateDevice;
    (void*&)D3D11CreateDevice = GetProcAddress(d3d, "D3D11CreateDevice");
    if (D3D11CreateDevice == nullptr)
        return 0;

    static const D3D_FEATURE_LEVEL d3dFeatureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    ID3D11Device* d3dDevice = nullptr;
    D3D_FEATURE_LEVEL d3dFeatureLevel;
    HRESULT hResult = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, d3dFeatureLevels, xxCountOf(d3dFeatureLevels), D3D11_SDK_VERSION, &d3dDevice, &d3dFeatureLevel, nullptr);
    if (hResult != S_OK)
        return 0;

    return reinterpret_cast<uint64_t>(d3dDevice);
}
//------------------------------------------------------------------------------
const char* xxGetDeviceStringD3D11_1(uint64_t device)
{
    return "Direct3D 11.1";
}
//==============================================================================
