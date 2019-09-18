#include "xxGraphicInternal.h"
#include "xxGraphicD3D.h"
#include "xxGraphicD3D10.h"
#include "xxGraphicD3D10_1.h"

#include <d3d10_1.h>

static HMODULE g_d3dLibrary = nullptr;

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceD3D10_1()
{
    if (g_d3dLibrary == nullptr)
        g_d3dLibrary = LoadLibraryW(L"d3d10_1.dll");
    if (g_d3dLibrary == nullptr)
        return 0;

    xxRegisterFunction(D3D10);

    xxDestroyInstance = xxDestroyInstanceD3D10_1;
    xxCreateDevice = xxCreateDeviceD3D10_1;
    xxGetDeviceString = xxGetDeviceStringD3D10_1;

    return reinterpret_cast<uint64_t>(g_d3dLibrary);
}
//------------------------------------------------------------------------------
void xxDestroyInstanceD3D10_1(uint64_t instance)
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
uint64_t xxCreateDeviceD3D10_1(uint64_t instance)
{
    HMODULE d3d = reinterpret_cast<HMODULE>(instance);
    if (d3d == nullptr)
        return 0;

    PFN_D3D10_CREATE_DEVICE1 D3D10CreateDevice1;
    (void*&)D3D10CreateDevice1 = GetProcAddress(d3d, "D3D10CreateDevice1");
    if (D3D10CreateDevice1 == nullptr)
        return 0;

    UINT flags = D3D10_CREATE_DEVICE_ALLOW_NULL_FROM_MAP | D3D10_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
    flags |= D3D10_CREATE_DEVICE_DEBUG;
#endif
    ID3D10Device1* d3dDevice = nullptr;
    HRESULT hResult = E_NOTIMPL;
    if (hResult != S_OK)
        hResult = D3D10CreateDevice1(nullptr, D3D10_DRIVER_TYPE_HARDWARE, nullptr, flags, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &d3dDevice);
    if (hResult != S_OK)
        hResult = D3D10CreateDevice1(nullptr, D3D10_DRIVER_TYPE_HARDWARE, nullptr, flags, D3D10_FEATURE_LEVEL_10_0, D3D10_1_SDK_VERSION, &d3dDevice);
    if (hResult != S_OK)
        hResult = D3D10CreateDevice1(nullptr, D3D10_DRIVER_TYPE_HARDWARE, nullptr, flags, D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &d3dDevice);
    if (hResult != S_OK)
        hResult = D3D10CreateDevice1(nullptr, D3D10_DRIVER_TYPE_HARDWARE, nullptr, flags, D3D10_FEATURE_LEVEL_9_2, D3D10_1_SDK_VERSION, &d3dDevice);
    if (hResult != S_OK)
        hResult = D3D10CreateDevice1(nullptr, D3D10_DRIVER_TYPE_HARDWARE, nullptr, flags, D3D10_FEATURE_LEVEL_9_1, D3D10_1_SDK_VERSION, &d3dDevice);
    if (hResult != S_OK)
        return 0;

    IUnknown* unknown = nullptr;
    xxLocalBreak()
    {
        if (d3dDevice->QueryInterface(__uuidof(ID3D10Device1*), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "10.1", xxGetDeviceString(reinterpret_cast<uint64_t>(d3dDevice)));
            break;
        }
        if (d3dDevice->QueryInterface(__uuidof(ID3D10Device*), (void**)&unknown) == S_OK)
        {
            xxLog("xxGraphic", "%s %s (%s)", "Direct3D", "10.0", xxGetDeviceString(reinterpret_cast<uint64_t>(d3dDevice)));
            break;
        }
    }
    SafeRelease(unknown);

    return reinterpret_cast<uint64_t>(d3dDevice);
}
//------------------------------------------------------------------------------
const char* xxGetDeviceStringD3D10_1(uint64_t device)
{
    return "Direct3D 10.1";
}
//==============================================================================
