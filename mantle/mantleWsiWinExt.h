//=====================================================================
// Copyright 2015 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
#ifndef _MANTLE_WSI_WIN_EXT_H_
#define _MANTLE_WSI_WIN_EXT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mantle.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// TYPES

typedef GR_HANDLE GR_WSI_WIN_DISPLAY;

// DEFINES

#define GR_MAX_DEVICE_NAME_LEN              32
#define GR_MAX_GAMMA_RAMP_CONTROL_POINTS    1025

// ENUMERATIONS

typedef enum _GR_WSI_WIN_IMAGE_STATE
{
    GR_WSI_WIN_IMAGE_STATE_PRESENT_WINDOWED     = 0x00200000,
    GR_WSI_WIN_IMAGE_STATE_PRESENT_FULLSCREEN   = 0x00200001,
} GR_WSI_WIN_IMAGE_STATE;

typedef enum _GR_WSI_WIN_INFO_TYPE
{
    GR_WSI_WIN_INFO_TYPE_QUEUE_PROPERTIES               = 0x00206800,
    GR_WSI_WIN_INFO_TYPE_DISPLAY_PROPERTIES             = 0x00206801,
    GR_WSI_WIN_INFO_TYPE_GAMMA_RAMP_CAPABILITIES        = 0x00206802,
    GR_WSI_WIN_INFO_TYPE_DISPLAY_FREESYNC_SUPPORT       = 0x00206803,
    GR_WSI_WIN_INFO_TYPE_PRESENTABLE_IMAGE_PROPERTIES   = 0x00206804,
    GR_WSI_WIN_INFO_TYPE_EXTENDED_DISPLAY_PROPERTIES    = 0x00206805,
} GR_WSI_WIN_INFO_TYPE;

typedef enum _GR_WSI_WIN_PRESENT_MODE
{
    GR_WSI_WIN_PRESENT_MODE_WINDOWED    = 0x00200200,
    GR_WSI_WIN_PRESENT_MODE_FULLSCREEN  = 0x00200201,
} GR_WSI_WIN_PRESENT_MODE;

typedef enum _GR_WSI_WIN_ROTATION_ANGLE
{
    GR_WSI_WIN_ROTATION_ANGLE_0     = 0x00200100,
    GR_WSI_WIN_ROTATION_ANGLE_90    = 0x00200101,
    GR_WSI_WIN_ROTATION_ANGLE_180   = 0x00200102,
    GR_WSI_WIN_ROTATION_ANGLE_270   = 0x00200103,
} GR_WSI_WIN_ROTATION_ANGLE;

// DATA STRUCTURES

typedef struct _GR_RGB_FLOAT
{
    GR_FLOAT red;
    GR_FLOAT green;
    GR_FLOAT blue;
} GR_RGB_FLOAT;

typedef struct _GR_WSI_WIN_DISPLAY_MODE
{
    GR_EXTENT2D extent;
    GR_FORMAT format;
    GR_UINT refreshRate;
    GR_BOOL stereo;
    GR_BOOL crossDisplayPresent;
} GR_WSI_WIN_DISPLAY_MODE;

typedef struct _GR_WSI_WIN_DISPLAY_PROPERTIES
{
    HMONITOR hMonitor;
    GR_CHAR displayName[GR_MAX_DEVICE_NAME_LEN];
    GR_RECT desktopCoordinates;
    GR_ENUM rotation;
} GR_WSI_WIN_DISPLAY_PROPERTIES;

typedef struct _GR_WSI_WIN_EXTENDED_DISPLAY_PROPERTIES
{
    GR_FLAGS extendedProperties;
} GR_WSI_WIN_EXTENDED_DISPLAY_PROPERTIES;

typedef struct _GR_WSI_WIN_GAMMA_RAMP
{
    GR_RGB_FLOAT scale;
    GR_RGB_FLOAT offset;
    GR_RGB_FLOAT gammaCurve[GR_MAX_GAMMA_RAMP_CONTROL_POINTS];
} GR_WSI_WIN_GAMMA_RAMP;

typedef struct _GR_WSI_WIN_GAMMA_RAMP_CAPABILITIES
{
    GR_BOOL supportsScaleAndOffset;
    GR_FLOAT minConvertedValue;
    GR_FLOAT maxConvertedValue;
    GR_UINT controlPointCount;
    GR_FLOAT controlPointPositions[GR_MAX_GAMMA_RAMP_CONTROL_POINTS];
} GR_WSI_WIN_GAMMA_RAMP_CAPABILITIES;

typedef struct _GR_WSI_WIN_PRESENT_INFO
{
    HWND hWndDest;
    GR_IMAGE srcImage;
    GR_ENUM presentMode;
    GR_UINT presentInterval;
    GR_FLAGS flags;
} GR_WSI_WIN_PRESENT_INFO;

typedef struct _GR_WSI_WIN_QUEUE_PROPERTIES
{
    GR_FLAGS presentSupport;
} GR_WSI_WIN_QUEUE_PROPERTIES;

// CREATE INFOS

typedef struct _GR_WSI_WIN_PRESENTABLE_IMAGE_CREATE_INFO
{
    GR_FORMAT format;
    GR_FLAGS usage;
    GR_EXTENT2D extent;
    GR_WSI_WIN_DISPLAY display;
    GR_FLAGS flags;
} GR_WSI_WIN_PRESENTABLE_IMAGE_CREATE_INFO;

typedef struct _GR_WSI_WIN_PRESENTABLE_IMAGE_PROPERTIES
{
    GR_WSI_WIN_PRESENTABLE_IMAGE_CREATE_INFO createInfo;
    GR_GPU_MEMORY mem;
} GR_WSI_WIN_PRESENTABLE_IMAGE_PROPERTIES;

// FUNCTIONS

typedef GR_RESULT (GR_STDCALL *PFN_grWsiWinGetDisplays)(GR_DEVICE device, GR_UINT* pDisplayCount, GR_WSI_WIN_DISPLAY* pDisplayList);
typedef GR_RESULT (GR_STDCALL *PFN_grWsiWinGetDisplayModeList)(GR_WSI_WIN_DISPLAY display, GR_UINT* pDisplayModeCount, GR_WSI_WIN_DISPLAY_MODE* pDisplayModeList);
typedef GR_RESULT (GR_STDCALL *PFN_grWsiWinTakeFullscreenOwnership)(GR_WSI_WIN_DISPLAY display, GR_IMAGE image);
typedef GR_RESULT (GR_STDCALL *PFN_grWsiWinReleaseFullscreenOwnership)(GR_WSI_WIN_DISPLAY display);
typedef GR_RESULT (GR_STDCALL *PFN_grWsiWinSetGammaRamp)(GR_WSI_WIN_DISPLAY display, const GR_WSI_WIN_GAMMA_RAMP* pGammaRamp);
typedef GR_RESULT (GR_STDCALL *PFN_grWsiWinWaitForVerticalBlank)(GR_WSI_WIN_DISPLAY display);
typedef GR_RESULT (GR_STDCALL *PFN_grWsiWinGetScanLine)(GR_WSI_WIN_DISPLAY display, GR_INT* pScanLine);
typedef GR_RESULT (GR_STDCALL *PFN_grWsiWinCreatePresentableImage)(GR_DEVICE device, const GR_WSI_WIN_PRESENTABLE_IMAGE_CREATE_INFO* pCreateInfo, GR_IMAGE* pImage, GR_GPU_MEMORY* pMem);
typedef GR_RESULT (GR_STDCALL *PFN_grWsiWinQueuePresent)(GR_QUEUE queue, const GR_WSI_WIN_PRESENT_INFO* pPresentInfo);
typedef GR_RESULT (GR_STDCALL *PFN_grWsiWinSetMaxQueuedFrames)(GR_DEVICE device, GR_UINT maxFrames);

#if GR_PROTOTYPES
PFN_grWsiWinGetDisplays                 grWsiWinGetDisplays;
PFN_grWsiWinGetDisplayModeList          grWsiWinGetDisplayModeList;
PFN_grWsiWinTakeFullscreenOwnership     grWsiWinTakeFullscreenOwnership;
PFN_grWsiWinReleaseFullscreenOwnership  grWsiWinReleaseFullscreenOwnership;
PFN_grWsiWinSetGammaRamp                grWsiWinSetGammaRamp;
PFN_grWsiWinWaitForVerticalBlank        grWsiWinWaitForVerticalBlank;
PFN_grWsiWinGetScanLine                 grWsiWinGetScanLine;
PFN_grWsiWinCreatePresentableImage      grWsiWinCreatePresentableImage;
PFN_grWsiWinQueuePresent                grWsiWinQueuePresent;
PFN_grWsiWinSetMaxQueuedFrames          grWsiWinSetMaxQueuedFrames;
#endif

#ifdef __cplusplus
}
#endif

#endif  // _MANTLE_WSI_WIN_EXT_H_
