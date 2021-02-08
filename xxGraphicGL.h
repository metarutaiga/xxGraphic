//==============================================================================
// xxGraphic : OpenGL Function Header
//
// Copyright (c) 2019-2020 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

#define KHRONOS_STATIC
#define GL_GLES_PROTOTYPES 1
#include "gl/gl32.h"

#ifndef GL_APPLE_texture_format_BGRA8888
#define GL_APPLE_texture_format_BGRA8888 1
#define GL_BGRA_EXT                       0x80E1
#define GL_BGRA8_EXT                      0x93A1
#endif /* GL_APPLE_texture_format_BGRA8888 */

#ifndef GL_ARB_texture_rectangle
#define GL_ARB_texture_rectangle 1
#define GL_TEXTURE_RECTANGLE_ARB          0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE_ARB  0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE_ARB    0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB 0x84F8
#endif /* GL_EXT_texture_rectangle */

#ifndef GL_UNSIGNED_INT_8_8_8_8_REV
#define GL_UNSIGNED_INT_8_8_8_8_REV       0x8367
#endif /* GL_UNSIGNED_INT_8_8_8_8_REV */

#ifndef GLEXPORT
#   if defined(_MSC_VER)
#       define GLEXPORT extern
#   else
#       define GLEXPORT extern __attribute__((visibility("hidden")))
#   endif
#endif

//==============================================================================
//  GLSL Shader
//==============================================================================
GLEXPORT const char* const                              glDefaultShaderCode;
//==============================================================================
//  Function
//==============================================================================
GLEXPORT uint64_t                                       (*glCreateContext)(uint64_t device, void* view, void** display);
GLEXPORT void                                           (*glDestroyContext)(uint64_t context, void* view, void* display);
GLEXPORT void*                                          (*glGetProcAddress)(const char* name);
GLEXPORT float                                          (*glGetScaleContext)(uint64_t context, void* view);
GLEXPORT void                                           (*glMakeCurrentContext)(uint64_t context, void* display);
GLEXPORT void                                           (*glPresentContext)(uint64_t context, void* display);
//------------------------------------------------------------------------------
