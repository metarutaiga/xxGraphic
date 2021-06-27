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
inline MTLBlendFactor mtlBlendFactor(xxGraphicBlendFactor blend)
{
    switch (blend)
    {
    default:
    case BLEND_FACTOR_ZERO:                     return MTLBlendFactorZero;
    case BLEND_FACTOR_ONE:                      return MTLBlendFactorOne;
    case BLEND_FACTOR_SRC_COLOR:                return MTLBlendFactorSourceColor;
    case BLEND_FACTOR_ONE_MINUS_SRC_COLOR:      return MTLBlendFactorOneMinusSourceColor;
    case BLEND_FACTOR_DST_COLOR:                return MTLBlendFactorDestinationColor;
    case BLEND_FACTOR_ONE_MINUS_DST_COLOR:      return MTLBlendFactorOneMinusDestinationColor;
    case BLEND_FACTOR_SRC_ALPHA:                return MTLBlendFactorSourceAlpha;
    case BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:      return MTLBlendFactorOneMinusSourceAlpha;
    case BLEND_FACTOR_DST_ALPHA:                return MTLBlendFactorDestinationAlpha;
    case BLEND_FACTOR_ONE_MINUS_DST_ALPHA:      return MTLBlendFactorOneMinusDestinationAlpha;
    case BLEND_FACTOR_CONSTANT_COLOR:           return MTLBlendFactorBlendColor;
    case BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR: return MTLBlendFactorOneMinusBlendColor;
    case BLEND_FACTOR_CONSTANT_ALPHA:           return MTLBlendFactorBlendAlpha;
    case BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA: return MTLBlendFactorOneMinusBlendAlpha;
    case BLEND_FACTOR_SRC_ALPHA_SATURATE:       return MTLBlendFactorSourceAlphaSaturated;
    case BLEND_FACTOR_SRC1_COLOR:               return MTLBlendFactorSource1Color;
    case BLEND_FACTOR_ONE_MINUS_SRC1_COLOR:     return MTLBlendFactorOneMinusSource1Color;
    case BLEND_FACTOR_SRC1_ALPHA:               return MTLBlendFactorSource1Alpha;
    case BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA:     return MTLBlendFactorOneMinusSource1Alpha;
    }
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

    // Metal 2
    id <MTLFunction>                vertexShader;
    id <MTLFunction>                fragmentShader;
    id <MTLArgumentEncoder>         vertexArgumentEncoder;
    id <MTLArgumentEncoder>         fragmentArgumentEncoder;
    NSUInteger                      vertexArgumentEncodedLength;
    NSUInteger                      fragmentArgumentEncodedLength;
};
//==============================================================================
