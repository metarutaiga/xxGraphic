//==============================================================================
// xxGraphic : Metal Internal Header
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxGraphic.h"

#pragma clang diagnostic ignored "-Wnullability-completeness"

#define MTLCreateSystemDefaultDevice MTLCreateSystemDefaultDevice_unused
#define MTLCopyAllDevices MTLCopyAllDevices_unused
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#if defined(xxMACOS)
#import <Cocoa/Cocoa.h>
#elif defined(xxIOS)
#import <UIKit/UIKit.h>
#endif
#undef MTLCreateSystemDefaultDevice
#undef MTLCopyAllDevices

extern Class classMTLCompileOptions;
extern Class classMTLDepthStencilDescriptor;
extern Class classMTLRenderPassDescriptor;
extern Class classMTLRenderPipelineColorAttachmentDescriptor;
extern Class classMTLRenderPipelineDescriptor;
extern Class classMTLSamplerDescriptor;
extern Class classMTLTextureDescriptor;
extern Class classMTLVertexDescriptor;

//==============================================================================
//  MSL Shader
//==============================================================================
extern const char* const mtlDefaultShaderCode;
extern const char* const mtlArgumentShaderCode;

//==============================================================================
//  Blend Factor
//==============================================================================
inline MTLBlendFactor mtlBlendFactor(const char* name)
{
    bool a = false;
    bool c = false;
    bool d = false;
    bool o = false;
    bool s = false;
    for (char x; (x = (*name)); name++)
    {
        switch (x)
        {
        case 'A':
        case 'a':
            a = true;
            c = false;
            break;
        case 'C':
        case 'c':
            a = false;
            c = true;
            break;
        case 'D':
        case 'd':
            if (s) break;
            d = true;
            break;
        case 'O':
        case 'o':
        case '1':
            o = true;
            break;
        case 'S':
        case 's':
            if (s) break;
            s = true;
            break;
        case 'Z':
        case 'z':
        case '0':
            return MTLBlendFactorZero;
        }
    }
    if (d)
    {
        if (c) return o ? MTLBlendFactorOneMinusDestinationColor : MTLBlendFactorDestinationColor;
        if (a) return o ? MTLBlendFactorOneMinusDestinationAlpha : MTLBlendFactorDestinationAlpha;
    }
    if (s)
    {
        if (c) return o ? MTLBlendFactorOneMinusSourceColor : MTLBlendFactorSourceColor;
        if (a) return o ? MTLBlendFactorOneMinusSourceAlpha : MTLBlendFactorSourceAlpha;
    }
    return o ? MTLBlendFactorOne : MTLBlendFactorZero;
}
//==============================================================================
//  Blend Operation
//==============================================================================
inline MTLBlendOperation mtlBlendOp(const char* name)
{
    for (char x; (x = (*name)); name++)
    {
        switch (x)
        {
        case 'A':
        case 'a':
        case '+':
            return MTLBlendOperationAdd;
        case 'S':
        case 's':
        case '-':
            return MTLBlendOperationSubtract;
        case 'I':
        case 'i':
        case 'R':
        case 'r':
            return MTLBlendOperationReverseSubtract;
        case 'M':
        case 'm':
            name++;
            x = (*name);
            return (x == 'I' || x == 'i') ? MTLBlendOperationMin : MTLBlendOperationMax;
        }
    }
    return MTLBlendOperationAdd;
}
//==============================================================================
//  Framebuffer
//==============================================================================
struct MTLFRAMEBUFFER
{
    id <MTLTexture>                 texture;
};
//==============================================================================
//  Swapchain
//==============================================================================
struct MTLSWAPCHAIN : public MTLFRAMEBUFFER
{
    CAMetalLayer*                   metalLayer;
    id <CAMetalDrawable>            drawable;
    id <MTLCommandQueue>            commandQueue;
    id <MTLCommandBuffer>           commandBuffer;
#if defined(xxMACOS)
    NSView*                         view;
#elif defined(xxIOS)
    UIView*                         view;
#endif
    int                             width;
    int                             height;
    float                           scale;

    // Metal 2
    id <MTLRenderCommandEncoder>    commandEncoder;
    int                             argumentBufferIndex;
    int                             argumentBufferStep;
    id <MTLBuffer>                  argumentBuffers[4];
    id <MTLArgumentEncoder>         vertexArgumentEncoder;
    id <MTLArgumentEncoder>         fragmentArgumentEncoder;
};
//==============================================================================
//  Texture
//==============================================================================
struct MTLTEXTURE
{
    id <MTLTexture>                 texture;
    id <MTLBuffer>                  buffer;
    int                             stride;
};
//==============================================================================
//  Pipeline
//==============================================================================
struct MTLPIPELINE
{
    id <MTLRenderPipelineState>     pipeline;
    id <MTLDepthStencilState>       depthStencil;
    MTLCullMode                     cullMode;
    bool                            scissorEnable;

    // Metal 2
    id <MTLFunction>                vertexShader;
    id <MTLFunction>                fragmentShader;
    id <MTLArgumentEncoder>         vertexArgumentEncoder;
    id <MTLArgumentEncoder>         fragmentArgumentEncoder;
    NSUInteger                      vertexArgumentEncodedLength;
    NSUInteger                      fragmentArgumentEncodedLength;
};
//==============================================================================
