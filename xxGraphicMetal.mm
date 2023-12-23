//==============================================================================
// xxGraphic : Metal Source
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#define xxCreateInstanceMetal xxConcate(xxCreateInstanceMetal, __LINE__)
#include "internal/xxGraphicInternalMetal.h"
#include "xxGraphicMetal.h"

Class                       classMTLCompileOptions = nil;
Class                       classMTLDepthStencilDescriptor = nil;
Class                       classMTLRenderPassDescriptor = nil;
Class                       classMTLRenderPipelineColorAttachmentDescriptor = nil;
Class                       classMTLRenderPipelineDescriptor = nil;
Class                       classMTLSamplerDescriptor = nil;
Class                       classMTLTextureDescriptor = nil;
Class                       classMTLVertexDescriptor = nil;

static void*                metalLibrary = nullptr;
static MTLViewport          metalViewport;
static MTLScissorRect       metalScissor;
static bool                 metalScissorEnable = false;
static MTLVertexDescriptor* metalVertexAttribute __unused = nil;
static id <MTLDevice>       (*MTLCreateSystemDefaultDevice)() NS_RETURNS_RETAINED = nullptr;
static NSArray*             (*MTLCopyAllDevices)() NS_RETURNS_RETAINED = nullptr;

//==============================================================================
//  Instance
//==============================================================================
static bool MTLSymbolFailed = false;
static void* MTLSymbol(char const* name)
{
    void* ptr = nullptr;

    if (ptr == nullptr && metalLibrary)
        ptr = xxGetProcAddress(metalLibrary, name);

    if (ptr == nullptr)
        xxLog("Metal", "%s is not found", name);

    MTLSymbolFailed |= (ptr == nullptr);

    return ptr;
}
#define MTLSymbol(var) (void*&)var = MTLSymbol(#var);
//------------------------------------------------------------------------------
id xxCreateInstanceMetal()
{
    if (metalLibrary == nullptr)
        metalLibrary = xxLoadLibrary("/System/Library/Frameworks/Metal.framework/Metal");
    if (metalLibrary == nullptr)
        return 0;

    MTLSymbolFailed = false;
    MTLSymbol(MTLCreateSystemDefaultDevice);
#if defined(xxMACOS)
    MTLSymbol(MTLCopyAllDevices);
#elif defined(xxIOS)
    MTLCopyAllDevices = nullptr;
#endif
    if (MTLSymbolFailed)
        return 0;

    classMTLCompileOptions = (__bridge Class)xxGetProcAddress(metalLibrary, "OBJC_CLASS_$_MTLCompileOptions");
    classMTLDepthStencilDescriptor = (__bridge Class)xxGetProcAddress(metalLibrary, "OBJC_CLASS_$_MTLDepthStencilDescriptor");
    classMTLRenderPassDescriptor = (__bridge Class)xxGetProcAddress(metalLibrary, "OBJC_CLASS_$_MTLRenderPassDescriptor");
    classMTLRenderPipelineColorAttachmentDescriptor = (__bridge Class)xxGetProcAddress(metalLibrary, "OBJC_CLASS_$_MTLRenderPipelineColorAttachmentDescriptor");
    classMTLRenderPipelineDescriptor = (__bridge Class)xxGetProcAddress(metalLibrary, "OBJC_CLASS_$_MTLRenderPipelineDescriptor");
    classMTLSamplerDescriptor = (__bridge Class)xxGetProcAddress(metalLibrary, "OBJC_CLASS_$_MTLSamplerDescriptor");
    classMTLTextureDescriptor = (__bridge Class)xxGetProcAddress(metalLibrary, "OBJC_CLASS_$_MTLTextureDescriptor");
    classMTLVertexDescriptor = (__bridge Class)xxGetProcAddress(metalLibrary, "OBJC_CLASS_$_MTLVertexDescriptor");

#if defined(xxMACOS)
    NSArray* allDevices = MTLCopyAllDevices();
    if (allDevices == nil)
        return 0;
#elif defined(xxIOS)
    id <MTLDevice> device = MTLCreateSystemDefaultDevice();
    if (device == nil)
        return 0;
    NSArray* allDevices = [[NSArray alloc] initWithObjects:&device
                                                     count:1];
#endif

    xxRegisterFunction(Metal);

    objc_retain(allDevices);
    return allDevices;
}
//------------------------------------------------------------------------------
void xxDestroyInstanceMetal(id __unsafe_unretained instance)
{
    objc_release(instance);
    instance = nil;

    if (metalLibrary)
    {
        xxFreeLibrary(metalLibrary);
        metalLibrary = nullptr;
    }

    xxUnregisterFunction();
}
//==============================================================================
//  Device
//==============================================================================
id xxCreateDeviceMetal(id __unsafe_unretained instance)
{
    NSArray* __unsafe_unretained allDevices = instance;
    if (allDevices.count == 0)
        return 0;

    id <MTLDevice> __unsafe_unretained device = allDevices.firstObject;
    for (NSUInteger i = 0; i < allDevices.count; ++i)
    {
        id <MTLDevice> __unsafe_unretained device = allDevices[i];
        xxLog("xxGraphic", "%s", device.name.UTF8String);
    }

    objc_retain(device);
    return device;
}
//------------------------------------------------------------------------------
void xxDestroyDeviceMetal(id <MTLDevice> __unsafe_unretained device)
{
    objc_release(device);
    device = nil;
}
//------------------------------------------------------------------------------
bool xxResetDeviceMetal(id <MTLDevice> __unsafe_unretained device)
{
    return true;
}
//------------------------------------------------------------------------------
bool xxTestDeviceMetal(id <MTLDevice> __unsafe_unretained device)
{
    return true;
}
//==============================================================================
//  Swapchain
//==============================================================================
MTLSWAPCHAIN* xxCreateSwapchainMetal(id <MTLDevice> __unsafe_unretained device, MTLRenderPassDescriptor* __unsafe_unretained renderPass, void* view, int width, int height, MTLSWAPCHAIN* oldSwapchain)
{
    if (device == nil)
        return 0;
    if (oldSwapchain && oldSwapchain->view == view && oldSwapchain->width == width && oldSwapchain->height == height)
        return oldSwapchain;
#if defined(xxMACOS)
    NSWindow* __unsafe_unretained nsWindow = (__bridge NSWindow*)view;
    if (nsWindow == nil)
        return 0;
    NSView* __unsafe_unretained nsView = nsWindow.contentViewController.view;
    if (nsView == nil)
        nsView = nsWindow.contentView;
    if (nsView == nil)
        return 0;
    float contentsScale = nsWindow.backingScaleFactor;
#elif defined(xxIOS)
    UIWindow* __unsafe_unretained nsWindow = (__bridge UIWindow*)view;
    if (nsWindow == nil)
        return 0;
    UIView* __unsafe_unretained nsView = nsWindow.rootViewController.view;
    if (nsView == nil)
        nsView = nsWindow;
    if (nsView == nil)
        return 0;
    float contentsScale = nsWindow.screen.nativeScale;
#endif

    MTLSWAPCHAIN* swapchain = new MTLSWAPCHAIN{};
    if (swapchain == nullptr)
        return 0;

    xxDestroySwapchainMetal(oldSwapchain);

#if defined(xxMACOS)
    CAMetalLayer* layer = [CAMetalLayer layer];
    layer.displaySyncEnabled = NO;
    nsView.layer = layer;
    nsView.wantsLayer = YES;
#elif defined(xxIOS)
    CAMetalLayer* layer = (CAMetalLayer*)nsView.layer;
    layer.drawableSize = CGSizeMake(width * contentsScale, height * contentsScale);
#endif
    layer.contentsScale = contentsScale;
    layer.device = device;
    layer.framebufferOnly = NO;
    layer.pixelFormat = MTLPixelFormatBGRA8Unorm;

    swapchain->metalLayer = layer;
    swapchain->drawable = [layer nextDrawable];
    swapchain->commandQueue = [device newCommandQueue];
    swapchain->view = nsView;
    swapchain->width = width;
    swapchain->height = height;
    swapchain->scale = contentsScale;

    MTLTextureDescriptor* desc = [classMTLTextureDescriptor new];
    desc.textureType = MTLTextureType2D;
    desc.pixelFormat = MTLPixelFormatDepth32Float;
    desc.width = width * contentsScale;
    desc.height = height * contentsScale;
    desc.usage = MTLTextureUsageRenderTarget;
#if defined(xxMACOS_LEGACY) || defined(xxIOS_LEGACY)
    desc.resourceOptions = MTLResourceStorageModePrivate;
#endif
    swapchain->depthstencil = [device newTextureWithDescriptor:desc];

    return swapchain;
}
//------------------------------------------------------------------------------
void xxDestroySwapchainMetal(MTLSWAPCHAIN* swapchain)
{
    delete swapchain;
}
//------------------------------------------------------------------------------
void xxPresentSwapchainMetal(MTLSWAPCHAIN* swapchain)
{
    id <MTLCommandBuffer> __unsafe_unretained commandBuffer = swapchain->commandBuffer;
    [commandBuffer presentDrawable:swapchain->drawable];
    [commandBuffer commit];

    swapchain->texture = nil;
    swapchain->drawable = nil;
    swapchain->commandBuffer = nil;
}
//------------------------------------------------------------------------------
id xxGetCommandBufferMetal(id <MTLDevice> __unsafe_unretained device, MTLSWAPCHAIN* swapchain)
{
    swapchain->commandBuffer = [swapchain->commandQueue commandBuffer];

    return swapchain->commandBuffer;
}
//------------------------------------------------------------------------------
MTLFRAMEBUFFER* xxGetFramebufferMetal(id <MTLDevice> __unsafe_unretained device, MTLSWAPCHAIN* swapchain, float* scale)
{
    if (scale)
    {
#if defined(xxMACOS)
        float scaleFactor = swapchain->view.window.backingScaleFactor;
#else
        float scaleFactor = swapchain->view.contentScaleFactor;
#endif
        if (swapchain->scale != scaleFactor)
        {
            swapchain->scale = scaleFactor;
            swapchain->metalLayer.contentsScale = scaleFactor;
            swapchain->metalLayer.drawableSize = CGSizeMake(swapchain->width * scaleFactor, swapchain->height * scaleFactor);
        }

        (*scale) = scaleFactor;
    }

    swapchain->drawable = [swapchain->metalLayer nextDrawable];
    swapchain->texture = swapchain->drawable.texture;

    return swapchain;
}
//==============================================================================
//  Command Buffer
//==============================================================================
bool xxBeginCommandBufferMetal(uint64_t commandBuffer)
{
    return true;
}
//------------------------------------------------------------------------------
void xxEndCommandBufferMetal(uint64_t commandBuffer)
{

}
//------------------------------------------------------------------------------
void xxSubmitCommandBufferMetal(uint64_t commandBuffer, uint64_t swapchain)
{
}
//==============================================================================
//  Render Pass
//==============================================================================
id xxCreateRenderPassMetal(id <MTLDevice> __unsafe_unretained device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil)
{
    MTLRenderPassDescriptor* renderPass = [classMTLRenderPassDescriptor new];

    renderPass.colorAttachments[0].loadAction = clearColor ? MTLLoadActionClear : MTLLoadActionDontCare;
    renderPass.depthAttachment.loadAction = clearDepth ? MTLLoadActionClear : MTLLoadActionDontCare;
    renderPass.stencilAttachment.loadAction = clearStencil ? MTLLoadActionClear : MTLLoadActionDontCare;

    renderPass.colorAttachments[0].storeAction = storeColor ? MTLStoreActionStore : MTLStoreActionDontCare;
    renderPass.depthAttachment.storeAction = storeDepth ? MTLStoreActionStore : MTLStoreActionDontCare;
    renderPass.stencilAttachment.storeAction = storeStencil ? MTLStoreActionStore : MTLStoreActionDontCare;

    objc_retain(renderPass);
    return renderPass;
}
//------------------------------------------------------------------------------
void xxDestroyRenderPassMetal(MTLRenderPassDescriptor* __unsafe_unretained renderPass)
{
    objc_release(renderPass);
    renderPass = nil;
}
//------------------------------------------------------------------------------
id xxBeginRenderPassMetal(id <MTLCommandBuffer> __unsafe_unretained commandBuffer, MTLFRAMEBUFFER* framebuffer, MTLRenderPassDescriptor* __unsafe_unretained renderPass, int width, int height, float color[4], float depth, unsigned char stencil)
{
    renderPass.colorAttachments[0].texture = framebuffer->texture;
    renderPass.depthAttachment.texture = framebuffer->depthstencil;

    renderPass.colorAttachments[0].clearColor = MTLClearColorMake(color[0], color[1], color[2], color[3]);
    renderPass.depthAttachment.clearDepth = depth;
    renderPass.stencilAttachment.clearStencil = stencil;

    id <MTLRenderCommandEncoder> commandEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPass];
    if (commandEncoder == nil)
        return nil;

    MTLViewport vp;
    vp.originX = 0;
    vp.originY = 0;
    vp.width = width;
    vp.height = height;
    vp.znear = 0.0f;
    vp.zfar = 1.0f;
    [commandEncoder setViewport:vp];
    metalViewport = vp;

    MTLScissorRect rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = width;
    rect.height = height;
    [commandEncoder setScissorRect:rect];
    metalScissor = rect;

    return commandEncoder;
}
//------------------------------------------------------------------------------
void xxEndRenderPassMetal(id <MTLRenderCommandEncoder> __unsafe_unretained commandEncoder, MTLFRAMEBUFFER* framebuffer, MTLRenderPassDescriptor* __unsafe_unretained renderPass)
{
    [commandEncoder endEncoding];
}
//==============================================================================
//  Vertex Attribute
//==============================================================================
id xxCreateVertexAttributeMetal(id <MTLDevice> __unsafe_unretained device, int count, int* attribute)
{
    MTLVertexDescriptor* vertexDescriptor = [classMTLVertexDescriptor new];
    int stride = 0;

    for (int i = 0; i < count; ++i)
    {
        int stream = (*attribute++);
        int offset = (*attribute++);
        int element = (*attribute++);
        int size = (*attribute++);

        stride += size;

        MTLVertexAttributeDescriptor* __unsafe_unretained attribute = vertexDescriptor.attributes[i];
        attribute.offset = offset;
        attribute.bufferIndex = xxGraphicDescriptor::VERTEX_BUFFER + stream;

        switch (size / element)
        {
        case sizeof(char):
            switch (element)
            {
            case 1:
                attribute.format = MTLVertexFormat(47)/*MTLVertexFormatUCharNormalized*/;
                break;
            case 2:
                attribute.format = MTLVertexFormatUChar2Normalized;
                break;
            case 3:
                attribute.format = MTLVertexFormatUChar3Normalized;
                break;
            case 4:
                attribute.format = MTLVertexFormatUChar4Normalized;
                break;
            }
            break;

        case sizeof(float):
            switch (element)
            {
            case 1:
                attribute.format = MTLVertexFormatFloat;
                break;
            case 2:
                attribute.format = MTLVertexFormatFloat2;
                break;
            case 3:
                attribute.format = MTLVertexFormatFloat3;
                break;
            case 4:
                attribute.format = MTLVertexFormatFloat4;
                break;
            }
            break;
        }
    }

    MTLVertexBufferLayoutDescriptor* __unsafe_unretained layout = vertexDescriptor.layouts[xxGraphicDescriptor::VERTEX_BUFFER];
    layout.stride = stride;
    layout.stepRate = MTLVertexStepFunctionPerVertex;

    objc_retain(vertexDescriptor);
    return vertexDescriptor;
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeMetal(MTLVertexDescriptor* __unsafe_unretained vertexAttribute)
{
    objc_release(vertexAttribute);
    vertexAttribute = nil;
}
//==============================================================================
//  Buffer
//==============================================================================
id xxCreateConstantBufferMetal(id <MTLDevice> __unsafe_unretained device, int size)
{
    if (device == nil)
        return 0;

    id <MTLBuffer> buffer = [device newBufferWithLength:size
                                                options:MTLResourceStorageModeShared];

    objc_retain(buffer);
    return buffer;
}
//------------------------------------------------------------------------------
id xxCreateIndexBufferMetal(id <MTLDevice> __unsafe_unretained device, int size)
{
    if (device == nil)
        return 0;

    id <MTLBuffer> buffer = [device newBufferWithLength:size
                                                options:MTLResourceStorageModeShared];

    objc_retain(buffer);
    return buffer;
}
//------------------------------------------------------------------------------
id xxCreateVertexBufferMetal(id <MTLDevice> __unsafe_unretained device, int size, MTLVertexDescriptor* __unsafe_unretained vertexAttribute)
{
    if (device == nil)
        return 0;

    id <MTLBuffer> buffer = [device newBufferWithLength:size
                                                options:MTLResourceStorageModeShared];

    objc_retain(buffer);
    return buffer;
}
//------------------------------------------------------------------------------
void xxDestroyBufferMetal(id <MTLDevice> __unsafe_unretained device, id <MTLBuffer> __unsafe_unretained buffer)
{
    objc_release(buffer);
    buffer = nil;
}
//------------------------------------------------------------------------------
void* xxMapBufferMetal(id <MTLDevice> __unsafe_unretained device, id <MTLBuffer> __unsafe_unretained buffer)
{
    return [buffer contents];
}
//------------------------------------------------------------------------------
void xxUnmapBufferMetal(id <MTLDevice> __unsafe_unretained device, id <MTLBuffer> __unsafe_unretained buffer)
{

}
//==============================================================================
//  Texture
//==============================================================================
MTLTEXTURE* xxCreateTextureMetal(id <MTLDevice> __unsafe_unretained device, int format, int width, int height, int depth, int mipmap, int array, void const* external)
{
    if (device == nil)
        return 0;

    MTLTEXTURE* output = new MTLTEXTURE{};
    if (output == nullptr)
        return 0;

    MTLPixelFormat pixelFormat = MTLPixelFormatRGBA8Unorm;
#if defined(xxMACOS_LEGACY)
    MTLResourceOptions options = MTLResourceStorageModeManaged;
#else
    MTLResourceOptions options = MTLResourceStorageModeShared;
#endif

    int stride = 0;
    IOSurfaceRef ioSurface = nullptr;
    id <MTLTexture> texture = nil;
    id <MTLBuffer> buffer = nil;
    if (external)
    {
        pixelFormat = MTLPixelFormatBGRA8Unorm;
        if ([NSStringFromClass([(__bridge id)external class]) containsString:@"IOSurface"])
        {
            ioSurface = (IOSurfaceRef)external;
            stride = 0;
#if defined(xxMACOS) || defined(xxMACCATALYST)
            options = MTLResourceStorageModeManaged;
#endif
        }
        else if ([NSStringFromClass([(__bridge id)external class]) containsString:@"Texture"])
        {
            texture = (__bridge id)external;
            stride = 0;
        }
        else
        {
            buffer = (__bridge id)external;
            stride = (int)buffer.length / height;
            options = buffer.storageMode << MTLResourceStorageModeShift;
        }
    }
    else if (mipmap == 1)
    {
        int alignment = 256;
        if (@available(macOS 10.13, iOS 11.0, *))
        {
            alignment = (int)[device minimumLinearTextureAlignmentForPixelFormat:pixelFormat];
        }
        stride = width * sizeof(int);
        stride = (stride + (alignment - 1)) & ~(alignment - 1);
        buffer = [device newBufferWithLength:stride * height
                                     options:options];
    }
    else
    {
        stride = width * sizeof(int);
    }

    MTLTextureDescriptor* desc = [classMTLTextureDescriptor texture2DDescriptorWithPixelFormat:pixelFormat
                                                                                         width:width
                                                                                        height:height
                                                                                     mipmapped:mipmap > 1];
    desc.resourceOptions = options;

    if (ioSurface)
    {
        texture = [device newTextureWithDescriptor:desc
                                         iosurface:ioSurface
                                             plane:0];
    }
    else if (texture)
    {
        
    }
    else if (buffer)
    {
        texture = [buffer newTextureWithDescriptor:desc
                                            offset:0
                                       bytesPerRow:stride];
    }
    else
    {
        texture = [device newTextureWithDescriptor:desc];
    }

    output->texture = texture;
    output->buffer = buffer;
    for (int i = 0; i < mipmap; ++i)
    {
        output->strides[i] = (stride >> i);
        if (output->strides[i] < sizeof(int))
            output->strides[i] = sizeof(int);
    }

    return output;
}
//------------------------------------------------------------------------------
void xxDestroyTextureMetal(MTLTEXTURE* texture)
{
    delete texture;
}
//------------------------------------------------------------------------------
void* xxMapTextureMetal(id <MTLDevice> __unsafe_unretained device, MTLTEXTURE* texture, int* stride, int level, int array)
{
    (*stride) = texture->strides[level];
    if (texture->buffer)
    {
        return texture->buffer.contents;
    }
    NSUInteger count = texture->texture.width * texture->texture.height * sizeof(int);
    texture->temporary = xxRealloc(texture->temporary, char, count);
    return texture->temporary;
}
//------------------------------------------------------------------------------
void xxUnmapTextureMetal(id <MTLDevice> __unsafe_unretained device, MTLTEXTURE* texture, int level, int array)
{
    if (texture->temporary)
    {
        NSUInteger width = texture->texture.width >> level;
        NSUInteger height = texture->texture.height >> level;
        [texture->texture replaceRegion:MTLRegionMake2D(0, 0, width, height)
                            mipmapLevel:level
                              withBytes:texture->temporary
                            bytesPerRow:texture->strides[level]];
        xxFree(texture->temporary);
        texture->temporary = nullptr;
        return;
    }
#if TARGET_OS_SIMULATOR
    [texture->texture replaceRegion:MTLRegionMake2D(0, 0, texture->texture.width, texture->texture.height)
                        mipmapLevel:0
                          withBytes:texture->buffer.contents
                        bytesPerRow:texture->buffer.length / texture->texture.height];
#elif defined(xxMACOS_LEGACY)
    [texture->buffer didModifyRange:NSMakeRange(0, texture->buffer.length)];
#endif
}
//==============================================================================
//  Sampler
//==============================================================================
id xxCreateSamplerMetal(id <MTLDevice> __unsafe_unretained device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    if (device == nil)
        return 0;

    MTLSamplerDescriptor* desc = [classMTLSamplerDescriptor new];
    desc.sAddressMode = clampU ? MTLSamplerAddressModeClampToEdge : MTLSamplerAddressModeRepeat;
    desc.tAddressMode = clampV ? MTLSamplerAddressModeClampToEdge : MTLSamplerAddressModeRepeat;
    desc.rAddressMode = clampW ? MTLSamplerAddressModeClampToEdge : MTLSamplerAddressModeRepeat;
    desc.magFilter = linearMag ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
    desc.minFilter = linearMin ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
    desc.mipFilter = linearMip ? MTLSamplerMipFilterLinear : MTLSamplerMipFilterNearest;
    desc.maxAnisotropy = anisotropy;

    id <MTLSamplerState> sampler = [device newSamplerStateWithDescriptor:desc];

    objc_retain(sampler);
    return sampler;
}
//------------------------------------------------------------------------------
void xxDestroySamplerMetal(id <MTLSamplerState> __unsafe_unretained sampler)
{
    objc_release(sampler);
    sampler = nil;
}
//==============================================================================
//  Shader
//==============================================================================
id xxCreateVertexShaderMetal(id <MTLDevice> __unsafe_unretained device, char const* shader, MTLVertexDescriptor* __unsafe_unretained vertexAttribute)
{
    if (device == nil)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        shader = mtlDefaultShaderCode;
    }

    NSError* error;
    MTLCompileOptions* options = [classMTLCompileOptions new];
    options.fastMathEnabled = YES;

    id <MTLLibrary> library = [device newLibraryWithSource:@(shader)
                                                   options:options
                                                     error:&error];
    if (library == nil)
    {
        xxLog("xxGraphic", "%s", error.localizedDescription.UTF8String);
        return 0;
    }

    id <MTLFunction> function = [library newFunctionWithName:@"VSMain"];

    objc_retain(function);
    return function;
}
//------------------------------------------------------------------------------
id xxCreateFragmentShaderMetal(id <MTLDevice> __unsafe_unretained device, char const* shader)
{
    if (device == nil)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        shader = mtlDefaultShaderCode;
    }

    NSError* error;
    MTLCompileOptions* options = [classMTLCompileOptions new];
    options.fastMathEnabled = YES;

    id <MTLLibrary> library = [device newLibraryWithSource:@(shader)
                                                   options:options
                                                     error:&error];
    if (library == nil)
    {
        xxLog("xxGraphic", "%s", error.localizedDescription.UTF8String);
        return 0;
    }

    id <MTLFunction> function = [library newFunctionWithName:@"FSMain"];

    objc_retain(function);
    return function;
}
//------------------------------------------------------------------------------
void xxDestroyShaderMetal(id <MTLDevice> __unsafe_unretained device, id <MTLFunction> __unsafe_unretained shader)
{
    objc_release(shader);
    shader = nil;
}
//==============================================================================
//  Pipeline
//==============================================================================
id xxCreateBlendStateMetal(id <MTLDevice> __unsafe_unretained device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha)
{
    MTLRenderPipelineColorAttachmentDescriptor* desc = [classMTLRenderPipelineColorAttachmentDescriptor new];
    desc.pixelFormat = MTLPixelFormatBGRA8Unorm;
    desc.sourceRGBBlendFactor = mtlBlendFactor(sourceColor);
    desc.destinationRGBBlendFactor = mtlBlendFactor(destinationColor);
    desc.rgbBlendOperation = mtlBlendOp(operationColor);
    desc.sourceAlphaBlendFactor = mtlBlendFactor(sourceAlpha);
    desc.destinationAlphaBlendFactor = mtlBlendFactor(destinationAlpha);
    desc.alphaBlendOperation = mtlBlendOp(operationAlpha);
    desc.blendingEnabled = (desc.sourceRGBBlendFactor != MTLBlendFactorOne ||
                            desc.destinationRGBBlendFactor != MTLBlendFactorZero ||
                            desc.rgbBlendOperation != MTLBlendOperationAdd ||
                            desc.sourceAlphaBlendFactor != MTLBlendFactorOne ||
                            desc.destinationAlphaBlendFactor != MTLBlendFactorZero ||
                            desc.alphaBlendOperation != MTLBlendOperationAdd) ? YES : NO;

    objc_retain(desc);
    return desc;
}
//------------------------------------------------------------------------------
id xxCreateDepthStencilStateMetal(id <MTLDevice> __unsafe_unretained device, char const* depthTest, bool depthWrite)
{
    MTLDepthStencilDescriptor* desc = [classMTLDepthStencilDescriptor new];
    desc.depthCompareFunction = mtlCompareOp(depthTest);
    desc.depthWriteEnabled = depthWrite;

    id <MTLDepthStencilState> depthStencilState = [device newDepthStencilStateWithDescriptor:desc];

    objc_retain(depthStencilState);
    return depthStencilState;
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateMetal(id <MTLDevice> __unsafe_unretained device, bool cull, bool scissor)
{
    uint64_t state = 0;
    state |= (cull << 0);
    state |= (scissor << 1);
    return state;
}
//------------------------------------------------------------------------------
MTLPIPELINE* xxCreatePipelineMetal(id <MTLDevice> __unsafe_unretained device, MTLRenderPassDescriptor* __unsafe_unretained renderPass, MTLRenderPipelineColorAttachmentDescriptor* __unsafe_unretained blendState, id <MTLDepthStencilState> __unsafe_unretained depthStencilState, uint64_t rasterizerState, MTLVertexDescriptor* __unsafe_unretained vertexAttribute, id <MTLFunction> __unsafe_unretained vertexShader, id <MTLFunction> __unsafe_unretained fragmentShader)
{
    if (device == nil)
        return 0;

    MTLPIPELINE* pipeline = new MTLPIPELINE{};
    if (pipeline == nullptr)
        return 0;

    MTLRenderPipelineDescriptor* desc = [classMTLRenderPipelineDescriptor new];
    desc.vertexFunction = vertexShader;
    desc.fragmentFunction = fragmentShader;
    desc.vertexDescriptor = vertexAttribute;
    desc.colorAttachments[0] = blendState;
    desc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;

    NSError* error;
    pipeline->pipeline = [device newRenderPipelineStateWithDescriptor:desc
                                                                error:&error];
    if (pipeline->pipeline == nil)
    {
        xxLog("xxGraphic", "%s", error.localizedDescription.UTF8String);
        return 0;
    }

    pipeline->depthStencil = depthStencilState;
    pipeline->cullMode = (rasterizerState & 1) ? MTLCullModeBack : MTLCullModeNone;
    pipeline->scissorEnable = (rasterizerState & 2) != 0;

    return pipeline;
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateMetal(MTLRenderPipelineColorAttachmentDescriptor* __unsafe_unretained blendState)
{
    objc_release(blendState);
    blendState = nil;
}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateMetal(id <MTLDepthStencilState> __unsafe_unretained depthStencilState)
{
    objc_release(depthStencilState);
    depthStencilState = nil;
}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateMetal(uint64_t rasterizerState)
{

}
//------------------------------------------------------------------------------
void xxDestroyPipelineMetal(MTLPIPELINE* pipeline)
{
    delete pipeline;
}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportMetal(id <MTLRenderCommandEncoder> __unsafe_unretained commandEncoder, int x, int y, int width, int height, float minZ, float maxZ)
{
    MTLViewport vp;
    vp.originX = x;
    vp.originY = y;
    vp.width = width;
    vp.height = height;
    vp.znear = minZ;
    vp.zfar = maxZ;
    [commandEncoder setViewport:vp];

    metalViewport = vp;
}
//------------------------------------------------------------------------------
void xxSetScissorMetal(id <MTLRenderCommandEncoder> __unsafe_unretained commandEncoder, int x, int y, int width, int height)
{
    MTLScissorRect rect;
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    [commandEncoder setScissorRect:rect];

    metalScissor = rect;
    metalScissorEnable = (width != metalViewport.width || height != metalViewport.height);
}
//------------------------------------------------------------------------------
void xxSetPipelineMetal(id <MTLRenderCommandEncoder> __unsafe_unretained commandEncoder, MTLPIPELINE* pipeline)
{
    [commandEncoder setRenderPipelineState:pipeline->pipeline];
    [commandEncoder setDepthStencilState:pipeline->depthStencil];
    [commandEncoder setCullMode:pipeline->cullMode];

    if (metalScissorEnable != pipeline->scissorEnable)
    {
        if (pipeline->scissorEnable)
        {
            [commandEncoder setScissorRect:metalScissor];
            metalScissorEnable = true;
        }
        else
        {
            MTLScissorRect rect;
            rect.x = 0;
            rect.y = 0;
            rect.width = metalViewport.width;
            rect.height = metalViewport.height;
            [commandEncoder setScissorRect:rect];
            metalScissorEnable = false;
        }
    }
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersMetal(id <MTLRenderCommandEncoder> __unsafe_unretained commandEncoder, int count, id <MTLBuffer> __unsafe_unretained* buffers, MTLVertexDescriptor* __unsafe_unretained vertexAttribute)
{
    static const NSUInteger offsets[8] = {};

    [commandEncoder setVertexBuffers:buffers
                             offsets:offsets
                           withRange:NSMakeRange(xxGraphicDescriptor::VERTEX_BUFFER, count)];
#if TARGET_OS_SIMULATOR
    metalVertexAttribute = vertexAttribute;
#endif
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesMetal(id <MTLRenderCommandEncoder> __unsafe_unretained commandEncoder, int count, MTLTEXTURE** textures)
{
    id <MTLTexture> __unsafe_unretained vertexTextures[8];

    for (int i = 0; i < count; ++i)
    {
        vertexTextures[i] = textures[i]->texture;
    }

    [commandEncoder setVertexTextures:vertexTextures
                            withRange:NSMakeRange(0, count)];
}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesMetal(id <MTLRenderCommandEncoder> __unsafe_unretained commandEncoder, int count, MTLTEXTURE** textures)
{
    id <MTLTexture> __unsafe_unretained fragmentTextures[8];

    for (int i = 0; i < count; ++i)
    {
        fragmentTextures[i] = textures[i]->texture;
    }

    [commandEncoder setFragmentTextures:fragmentTextures
                              withRange:NSMakeRange(0, count)];
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersMetal(id <MTLRenderCommandEncoder> __unsafe_unretained commandEncoder, int count, id <MTLSamplerState> __unsafe_unretained* samplers)
{
    [commandEncoder setVertexSamplerStates:samplers
                                 withRange:NSMakeRange(0, count)];
}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersMetal(id <MTLRenderCommandEncoder> __unsafe_unretained commandEncoder, int count, id <MTLSamplerState> __unsafe_unretained* samplers)
{
    [commandEncoder setFragmentSamplerStates:samplers
                                   withRange:NSMakeRange(0, count)];
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferMetal(id <MTLRenderCommandEncoder> __unsafe_unretained commandEncoder, id <MTLBuffer> __unsafe_unretained buffer, int size)
{
    [commandEncoder setVertexBuffer:buffer
                             offset:0
                            atIndex:0];
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferMetal(id <MTLRenderCommandEncoder> __unsafe_unretained commandEncoder, id <MTLBuffer> __unsafe_unretained buffer, int size)
{
    [commandEncoder setFragmentBuffer:buffer
                               offset:0
                              atIndex:0];
}
//------------------------------------------------------------------------------
void xxDrawMetal(id <MTLRenderCommandEncoder> __unsafe_unretained commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance)
{
    [commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                       vertexStart:firstVertex
                       vertexCount:vertexCount
                     instanceCount:instanceCount
                      baseInstance:firstInstance];
}
//------------------------------------------------------------------------------
void xxDrawIndexedMetal(id <MTLRenderCommandEncoder> __unsafe_unretained commandEncoder, id <MTLBuffer> __unsafe_unretained indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    MTLIndexType indexType = (INDEX_BUFFER_WIDTH == /* DISABLES CODE */(2)) ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32;
#if TARGET_OS_SIMULATOR
    MTLVertexDescriptor* __unsafe_unretained desc = (__bridge id)reinterpret_cast<void*>(metalVertexAttribute);
    MTLVertexBufferLayoutDescriptor* __unsafe_unretained layout = desc.layouts[xxGraphicDescriptor::VERTEX_BUFFER];
    [commandEncoder setVertexBufferOffset:vertexOffset * layout.stride
                                  atIndex:xxGraphicDescriptor::VERTEX_BUFFER];
    [commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                               indexCount:indexCount
                                indexType:indexType
                              indexBuffer:indexBuffer
                        indexBufferOffset:firstIndex * INDEX_BUFFER_WIDTH];
#else
    [commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                               indexCount:indexCount
                                indexType:indexType
                              indexBuffer:indexBuffer
                        indexBufferOffset:firstIndex * INDEX_BUFFER_WIDTH
                            instanceCount:instanceCount
                               baseVertex:vertexOffset
                             baseInstance:firstInstance];
#endif
}
//==============================================================================
