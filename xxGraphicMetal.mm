//==============================================================================
// xxGraphic : Metal Source
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternalMetal.h"
#include "xxGraphicMetal.h"

Class                   classMTLCompileOptions = nil;
Class                   classMTLDepthStencilDescriptor = nil;
Class                   classMTLRenderPassDescriptor = nil;
Class                   classMTLRenderPipelineColorAttachmentDescriptor = nil;
Class                   classMTLRenderPipelineDescriptor = nil;
Class                   classMTLSamplerDescriptor = nil;
Class                   classMTLTextureDescriptor = nil;
Class                   classMTLVertexDescriptor = nil;

static void*            g_metalLibrary = nullptr;
static MTLViewport      g_metalViewport;
static MTLScissorRect   g_metalScissor;
static bool             g_metalScissorEnable = false;
static uint64_t         g_metalVertexAttribute = 0;
static id <MTLDevice>   (*MTLCreateSystemDefaultDevice)() = nullptr;
static void*            (*MTLCopyAllDevices)() = nullptr;

//==============================================================================
//  Instance
//==============================================================================
static bool MTLSymbolFailed = false;
static void* MTLSymbol(char const* name)
{
    void* ptr = nullptr;

    if (ptr == nullptr && g_metalLibrary)
        ptr = xxGetProcAddress(g_metalLibrary, name);

    if (ptr == nullptr)
        xxLog("Metal", "%s is not found", name);

    MTLSymbolFailed |= (ptr == nullptr);

    return ptr;
}
#define MTLSymbol(var) (void*&)var = MTLSymbol(#var);
//------------------------------------------------------------------------------
uint64_t xxCreateInstanceMetal()
{
    if (g_metalLibrary == nullptr)
        g_metalLibrary = xxLoadLibrary("/System/Library/Frameworks/Metal.framework/Metal");
    if (g_metalLibrary == nullptr)
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

    classMTLCompileOptions = (__bridge Class)xxGetProcAddress(g_metalLibrary, "OBJC_CLASS_$_MTLCompileOptions");
    classMTLDepthStencilDescriptor = (__bridge Class)xxGetProcAddress(g_metalLibrary, "OBJC_CLASS_$_MTLDepthStencilDescriptor");
    classMTLRenderPassDescriptor = (__bridge Class)xxGetProcAddress(g_metalLibrary, "OBJC_CLASS_$_MTLRenderPassDescriptor");
    classMTLRenderPipelineColorAttachmentDescriptor = (__bridge Class)xxGetProcAddress(g_metalLibrary, "OBJC_CLASS_$_MTLRenderPipelineColorAttachmentDescriptor");
    classMTLRenderPipelineDescriptor = (__bridge Class)xxGetProcAddress(g_metalLibrary, "OBJC_CLASS_$_MTLRenderPipelineDescriptor");
    classMTLSamplerDescriptor = (__bridge Class)xxGetProcAddress(g_metalLibrary, "OBJC_CLASS_$_MTLSamplerDescriptor");
    classMTLTextureDescriptor = (__bridge Class)xxGetProcAddress(g_metalLibrary, "OBJC_CLASS_$_MTLTextureDescriptor");
    classMTLVertexDescriptor = (__bridge Class)xxGetProcAddress(g_metalLibrary, "OBJC_CLASS_$_MTLVertexDescriptor");

#if defined(xxMACOS)
    NSArray* allDevices = (__bridge_transfer NSArray*)MTLCopyAllDevices();
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

    return reinterpret_cast<uint64_t>((__bridge_retained void*)allDevices);
}
//------------------------------------------------------------------------------
void xxDestroyInstanceMetal(uint64_t instance)
{
    NSArray* allDevices = (__bridge_transfer NSArray*)reinterpret_cast<void*>(instance);

    allDevices = nil;

    if (g_metalLibrary)
    {
        xxFreeLibrary(g_metalLibrary);
        g_metalLibrary = nullptr;
    }

    xxUnregisterFunction();
}
//==============================================================================
//  Device
//==============================================================================
uint64_t xxCreateDeviceMetal(uint64_t instance)
{
    NSArray* allDevices = (__bridge NSArray*)reinterpret_cast<void*>(instance);
    if (allDevices.count == 0)
        return 0;

    id <MTLDevice> device = allDevices.firstObject;
    for (NSUInteger i = 0; i < allDevices.count; ++i)
    {
        id <MTLDevice> device = allDevices[i];
        xxLog("xxGraphic", "%s", [[device name] UTF8String]);
    }

    return reinterpret_cast<uint64_t>((__bridge_retained void*)device);
}
//------------------------------------------------------------------------------
void xxDestroyDeviceMetal(uint64_t device)
{
    id <MTLDevice> mtlDevice = (__bridge_transfer id)reinterpret_cast<void*>(device);

    mtlDevice = nil;
}
//------------------------------------------------------------------------------
bool xxResetDeviceMetal(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
bool xxTestDeviceMetal(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
char const* xxGetDeviceNameMetal()
{
    return "Metal";
}
//==============================================================================
//  Swapchain
//==============================================================================
uint64_t xxCreateSwapchainMetal(uint64_t device, uint64_t renderPass, void* view, int width, int height, uint64_t oldSwapchain)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;
    MTLSWAPCHAIN* mtlOldSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(oldSwapchain);
    if (mtlOldSwapchain && mtlOldSwapchain->view == view && mtlOldSwapchain->width == width && mtlOldSwapchain->height == height)
        return oldSwapchain;
#if defined(xxMACOS)
    NSWindow* nsWindow = (__bridge NSWindow*)view;
    if (nsWindow == nil)
        return 0;
    NSView* nsView = [[nsWindow contentViewController] view];
    if (nsView == nil)
        nsView = [nsWindow contentView];
    if (nsView == nil)
        return 0;
    float contentsScale = [nsWindow backingScaleFactor];
#elif defined(xxIOS)
    UIWindow* nsWindow = (__bridge UIWindow*)view;
    if (nsWindow == nil)
        return 0;
    UIView* nsView = [[nsWindow rootViewController] view];
    if (nsView == nil)
        nsView = nsWindow;
    if (nsView == nil)
        return 0;
    float contentsScale = [[nsWindow screen] nativeScale];
#endif

    MTLSWAPCHAIN* swapchain = new MTLSWAPCHAIN{};
    if (swapchain == nullptr)
        return 0;

    xxDestroySwapchainMetal(oldSwapchain);

#if defined(xxMACOS)
    CAMetalLayer* layer = [CAMetalLayer layer];
    layer.displaySyncEnabled = NO;
    [nsView setLayer:layer];
    [nsView setWantsLayer:YES];
#elif defined(xxIOS)
    CAMetalLayer* layer = (CAMetalLayer*)[nsView layer];
    layer.drawableSize = CGSizeMake(width * contentsScale, height * contentsScale);
#endif
    layer.contentsScale = contentsScale;
    layer.device = mtlDevice;
    layer.framebufferOnly = NO;
    layer.pixelFormat = MTLPixelFormatBGRA8Unorm;

    swapchain->metalLayer = layer;
    swapchain->drawable = [layer nextDrawable];
    swapchain->commandQueue = [mtlDevice newCommandQueue];
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
    swapchain->depthstencil = [mtlDevice newTextureWithDescriptor:desc];

    return reinterpret_cast<uint64_t>(swapchain);
}
//------------------------------------------------------------------------------
void xxDestroySwapchainMetal(uint64_t swapchain)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(swapchain);

    delete mtlSwapchain;
}
//------------------------------------------------------------------------------
void xxPresentSwapchainMetal(uint64_t swapchain)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(swapchain);

    id <MTLCommandBuffer> __unsafe_unretained mtlCommandBuffer = mtlSwapchain->commandBuffer;
    [mtlCommandBuffer presentDrawable:mtlSwapchain->drawable];
    [mtlCommandBuffer commit];

    mtlSwapchain->texture = nil;
    mtlSwapchain->drawable = nil;
    mtlSwapchain->commandBuffer = nil;
}
//------------------------------------------------------------------------------
uint64_t xxGetCommandBufferMetal(uint64_t device, uint64_t swapchain)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(swapchain);

    id <MTLCommandBuffer> commandBuffer = [mtlSwapchain->commandQueue commandBuffer];
    mtlSwapchain->commandBuffer = commandBuffer;

    return reinterpret_cast<uint64_t>((__bridge void*)commandBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferMetal(uint64_t device, uint64_t swapchain, float* scale)
{
    MTLSWAPCHAIN* mtlSwapchain = reinterpret_cast<MTLSWAPCHAIN*>(swapchain);

    if (scale)
    {
#if defined(xxMACOS)
        float scaleFactor = [[mtlSwapchain->view window] backingScaleFactor];
#else
        float scaleFactor = [mtlSwapchain->view contentScaleFactor];
#endif
        if (mtlSwapchain->scale != scaleFactor)
        {
            mtlSwapchain->scale = scaleFactor;
            mtlSwapchain->metalLayer.contentsScale = scaleFactor;
            mtlSwapchain->metalLayer.drawableSize = CGSizeMake(mtlSwapchain->width * scaleFactor, mtlSwapchain->height * scaleFactor);
        }

        (*scale) = scaleFactor;
    }

    mtlSwapchain->drawable = [mtlSwapchain->metalLayer nextDrawable];
    mtlSwapchain->texture = [mtlSwapchain->drawable texture];

    return reinterpret_cast<uint64_t>(mtlSwapchain);
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
uint64_t xxCreateRenderPassMetal(uint64_t device, bool clearColor, bool clearDepth, bool clearStencil, bool storeColor, bool storeDepth, bool storeStencil)
{
    MTLRenderPassDescriptor* renderPass = [classMTLRenderPassDescriptor new];

    renderPass.colorAttachments[0].loadAction = clearColor ? MTLLoadActionClear : MTLLoadActionDontCare;
    renderPass.depthAttachment.loadAction = clearDepth ? MTLLoadActionClear : MTLLoadActionDontCare;
    renderPass.stencilAttachment.loadAction = clearStencil ? MTLLoadActionClear : MTLLoadActionDontCare;

    renderPass.colorAttachments[0].storeAction = storeColor ? MTLStoreActionStore : MTLStoreActionDontCare;
    renderPass.depthAttachment.storeAction = storeDepth ? MTLStoreActionStore : MTLStoreActionDontCare;
    renderPass.stencilAttachment.storeAction = storeStencil ? MTLStoreActionStore : MTLStoreActionDontCare;

    return reinterpret_cast<uint64_t>((__bridge_retained void*)renderPass);
}
//------------------------------------------------------------------------------
void xxDestroyRenderPassMetal(uint64_t renderPass)
{
    MTLRenderPassDescriptor* mtlRenderPass = (__bridge_transfer id)reinterpret_cast<void*>(renderPass);

    mtlRenderPass = nil;
}
//------------------------------------------------------------------------------
uint64_t xxBeginRenderPassMetal(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass, int width, int height, float color[4], float depth, unsigned char stencil)
{
    id <MTLCommandBuffer> __unsafe_unretained mtlCommandBuffer = (__bridge id)reinterpret_cast<void*>(commandBuffer);
    MTLFRAMEBUFFER* mtlFramebuffer = reinterpret_cast<MTLFRAMEBUFFER*>(framebuffer);
    MTLRenderPassDescriptor* __unsafe_unretained mtlRenderPass = (__bridge MTLRenderPassDescriptor*)reinterpret_cast<void*>(renderPass);

    mtlRenderPass.colorAttachments[0].texture = mtlFramebuffer->texture;
    mtlRenderPass.depthAttachment.texture = mtlFramebuffer->depthstencil;

    mtlRenderPass.colorAttachments[0].clearColor = MTLClearColorMake(color[0], color[1], color[2], color[3]);
    mtlRenderPass.depthAttachment.clearDepth = depth;
    mtlRenderPass.stencilAttachment.clearStencil = stencil;

    id <MTLRenderCommandEncoder> commandEncoder = [mtlCommandBuffer renderCommandEncoderWithDescriptor:mtlRenderPass];

    MTLViewport vp;
    vp.originX = 0;
    vp.originY = 0;
    vp.width = width;
    vp.height = height;
    vp.znear = 0.0f;
    vp.zfar = 1.0f;
    [commandEncoder setViewport:vp];
    g_metalViewport = vp;

    MTLScissorRect rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = width;
    rect.height = height;
    [commandEncoder setScissorRect:rect];
    g_metalScissor = rect;

    return reinterpret_cast<uint64_t>((__bridge_retained void*)commandEncoder);
}
//------------------------------------------------------------------------------
void xxEndRenderPassMetal(uint64_t commandEncoder, uint64_t framebuffer, uint64_t renderPass)
{
    id <MTLRenderCommandEncoder> mtlCommandEncoder = (__bridge_transfer id)reinterpret_cast<void*>(commandEncoder);

    [mtlCommandEncoder endEncoding];
}
//==============================================================================
//  Vertex Attribute
//==============================================================================
uint64_t xxCreateVertexAttributeMetal(uint64_t device, int count, int* attribute)
{
    MTLVertexDescriptor* desc = [classMTLVertexDescriptor new];
    int stride = 0;

    for (int i = 0; i < count; ++i)
    {
        int stream = (*attribute++);
        int offset = (*attribute++);
        int element = (*attribute++);
        int size = (*attribute++);

        stride += size;

        MTLVertexAttributeDescriptor* attribute = desc.attributes[i];
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

    MTLVertexBufferLayoutDescriptor* layout = desc.layouts[xxGraphicDescriptor::VERTEX_BUFFER];
    layout.stride = stride;
    layout.stepRate = MTLVertexStepFunctionPerVertex;

    return reinterpret_cast<uint64_t>((__bridge_retained void*)desc);
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeMetal(uint64_t vertexAttribute)
{
    MTLVertexDescriptor* desc = (__bridge_transfer MTLVertexDescriptor*)reinterpret_cast<void*>(vertexAttribute);

    desc = nil;
}
//==============================================================================
//  Buffer
//==============================================================================
uint64_t xxCreateConstantBufferMetal(uint64_t device, int size)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    MTLBUFFER* mtlBuffer = new MTLBUFFER{};
    if (mtlBuffer == nullptr)
        return 0;

    id <MTLBuffer> buffer = [mtlDevice newBufferWithLength:size
                                                   options:MTLResourceStorageModeShared];

    mtlBuffer->buffer = buffer;

    return reinterpret_cast<uint64_t>(mtlBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferMetal(uint64_t device, int size)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    MTLBUFFER* mtlBuffer = new MTLBUFFER{};
    if (mtlBuffer == nullptr)
        return 0;

    id <MTLBuffer> buffer = [mtlDevice newBufferWithLength:size
                                                   options:MTLResourceStorageModeShared];

    mtlBuffer->buffer = buffer;

    return reinterpret_cast<uint64_t>(mtlBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferMetal(uint64_t device, int size, uint64_t vertexAttribute)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    MTLBUFFER* mtlBuffer = new MTLBUFFER{};
    if (mtlBuffer == nullptr)
        return 0;

    id <MTLBuffer> buffer = [mtlDevice newBufferWithLength:size
                                                   options:MTLResourceStorageModeShared];

    mtlBuffer->buffer = buffer;

    return reinterpret_cast<uint64_t>(mtlBuffer);
}
//------------------------------------------------------------------------------
void xxDestroyBufferMetal(uint64_t device, uint64_t buffer)
{
    MTLBUFFER* mtlBuffer = reinterpret_cast<MTLBUFFER*>(buffer);

    delete mtlBuffer;
}
//------------------------------------------------------------------------------
void* xxMapBufferMetal(uint64_t device, uint64_t buffer)
{
    MTLBUFFER* mtlBuffer = reinterpret_cast<MTLBUFFER*>(buffer);

    return [mtlBuffer->buffer contents];
}
//------------------------------------------------------------------------------
void xxUnmapBufferMetal(uint64_t device, uint64_t buffer)
{

}
//==============================================================================
//  Texture
//==============================================================================
uint64_t xxCreateTextureMetal(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, void const* external)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    MTLTEXTURE* mtlTexture = new MTLTEXTURE{};
    if (mtlTexture == nullptr)
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
            stride = (int)[buffer length] / height;
            options = [buffer storageMode] << MTLResourceStorageModeShift;
        }
    }
    else if (mipmap == 1)
    {
        int alignment = 256;
        if (@available(macOS 10.13, iOS 11.0, *))
        {
            alignment = (int)[mtlDevice minimumLinearTextureAlignmentForPixelFormat:pixelFormat];
        }
        stride = width * sizeof(int);
        stride = (stride + (alignment - 1)) & ~(alignment - 1);
        buffer = [mtlDevice newBufferWithLength:stride * height
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
        texture = [mtlDevice newTextureWithDescriptor:desc
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
        texture = [mtlDevice newTextureWithDescriptor:desc];
    }

    mtlTexture->texture = texture;
    mtlTexture->buffer = buffer;
    for (int i = 0; i < mipmap; ++i)
    {
        mtlTexture->strides[i] = (stride >> i);
        if (mtlTexture->strides[i] < sizeof(int))
            mtlTexture->strides[i] = sizeof(int);
    }

    return reinterpret_cast<uint64_t>(mtlTexture);
}
//------------------------------------------------------------------------------
void xxDestroyTextureMetal(uint64_t texture)
{
    MTLTEXTURE* mtlTexture = reinterpret_cast<MTLTEXTURE*>(texture);

    delete mtlTexture;
}
//------------------------------------------------------------------------------
void* xxMapTextureMetal(uint64_t device, uint64_t texture, int* stride, int level, int array)
{
    MTLTEXTURE* mtlTexture = reinterpret_cast<MTLTEXTURE*>(texture);

    (*stride) = mtlTexture->strides[level];
    if (mtlTexture->buffer)
    {
        return [mtlTexture->buffer contents];
    }
    NSUInteger count = [mtlTexture->texture width] * [mtlTexture->texture height] * sizeof(int);
    mtlTexture->temporary = xxRealloc(mtlTexture->temporary, int, count);
    return mtlTexture->temporary;
}
//------------------------------------------------------------------------------
void xxUnmapTextureMetal(uint64_t device, uint64_t texture, int level, int array)
{
    MTLTEXTURE* mtlTexture = reinterpret_cast<MTLTEXTURE*>(texture);
#if TARGET_OS_SIMULATOR
    [mtlTexture->texture replaceRegion:MTLRegionMake2D(0, 0, mtlTexture->texture.width, mtlTexture->texture.height)
                           mipmapLevel:0
                             withBytes:[mtlTexture->buffer contents]
                           bytesPerRow:[mtlTexture->buffer length] / mtlTexture->texture.height];
#elif defined(xxMACOS_LEGACY)
    [mtlTexture->buffer didModifyRange:NSMakeRange(0, [mtlTexture->buffer length])];
#else
    if (mtlTexture->temporary)
    {
        NSUInteger width = [mtlTexture->texture width] >> level;
        NSUInteger height = [mtlTexture->texture height] >> level;
        [mtlTexture->texture replaceRegion:MTLRegionMake2D(0, 0, width, height)
                               mipmapLevel:level
                                 withBytes:mtlTexture->temporary
                               bytesPerRow:mtlTexture->strides[level]];
        xxFree(mtlTexture->temporary);
        mtlTexture->temporary = nullptr;
    }
#endif
}
//==============================================================================
//  Sampler
//==============================================================================
uint64_t xxCreateSamplerMetal(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    MTLSamplerDescriptor* desc = [classMTLSamplerDescriptor new];
    desc.sAddressMode = clampU ? MTLSamplerAddressModeClampToEdge : MTLSamplerAddressModeRepeat;
    desc.tAddressMode = clampV ? MTLSamplerAddressModeClampToEdge : MTLSamplerAddressModeRepeat;
    desc.rAddressMode = clampW ? MTLSamplerAddressModeClampToEdge : MTLSamplerAddressModeRepeat;
    desc.magFilter = linearMag ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
    desc.minFilter = linearMin ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
    desc.mipFilter = linearMip ? MTLSamplerMipFilterLinear : MTLSamplerMipFilterNearest;
    desc.maxAnisotropy = anisotropy;

    id <MTLSamplerState> sampler = [mtlDevice newSamplerStateWithDescriptor:desc];

    return reinterpret_cast<uint64_t>((__bridge_retained void*)sampler);
}
//------------------------------------------------------------------------------
void xxDestroySamplerMetal(uint64_t sampler)
{
    id <MTLSamplerState> mtlSampler = (__bridge_transfer id)reinterpret_cast<void*>(sampler);

    mtlSampler = nil;
}
//==============================================================================
//  Shader
//==============================================================================
uint64_t xxCreateVertexShaderMetal(uint64_t device, char const* shader, uint64_t vertexAttribute)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        shader = mtlDefaultShaderCode;
    }

    NSError* error;
    MTLCompileOptions* options = [classMTLCompileOptions new];
    options.fastMathEnabled = YES;

    id <MTLLibrary> library = [mtlDevice newLibraryWithSource:[NSString stringWithUTF8String:shader]
                                                      options:options
                                                        error:&error];
    if (library == nil)
    {
        xxLog("xxGraphic", "%s", [[error localizedDescription] UTF8String]);
        return 0;
    }

    id <MTLFunction> function = [library newFunctionWithName:@"VSMain"];

    return reinterpret_cast<uint64_t>((__bridge_retained void*)function);
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderMetal(uint64_t device, char const* shader)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        shader = mtlDefaultShaderCode;
    }

    NSError* error;
    MTLCompileOptions* options = [classMTLCompileOptions new];
    options.fastMathEnabled = YES;

    id <MTLLibrary> library = [mtlDevice newLibraryWithSource:[NSString stringWithUTF8String:shader]
                                                      options:options
                                                        error:&error];
    if (library == nil)
    {
        xxLog("xxGraphic", "%s", [[error localizedDescription] UTF8String]);
        return 0;
    }

    id <MTLFunction> function = [library newFunctionWithName:@"FSMain"];

    return reinterpret_cast<uint64_t>((__bridge_retained void*)function);
}
//------------------------------------------------------------------------------
void xxDestroyShaderMetal(uint64_t device, uint64_t shader)
{
    id <MTLFunction> function = (__bridge_transfer id)reinterpret_cast<void*>(shader);

    function = nil;
}
//==============================================================================
//  Pipeline
//==============================================================================
uint64_t xxCreateBlendStateMetal(uint64_t device, char const* sourceColor, char const* operationColor, char const* destinationColor, char const* sourceAlpha, char const* operationAlpha, char const* destinationAlpha)
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

    return reinterpret_cast<uint64_t>((__bridge_retained void*)desc);
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateMetal(uint64_t device, char const* depthTest, bool depthWrite)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    MTLDepthStencilDescriptor* desc = [classMTLDepthStencilDescriptor new];
    desc.depthCompareFunction = mtlCompareOp(depthTest);
    desc.depthWriteEnabled = depthWrite;

    id <MTLDepthStencilState> depthStencilState = [mtlDevice newDepthStencilStateWithDescriptor:desc];

    return reinterpret_cast<uint64_t>((__bridge_retained void*)depthStencilState);
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateMetal(uint64_t device, bool cull, bool scissor)
{
    uint64_t state = 0;
    state |= (cull << 0);
    state |= (scissor << 1);
    return state;
}
//------------------------------------------------------------------------------
uint64_t xxCreatePipelineMetal(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;
    MTLRenderPipelineColorAttachmentDescriptor* mtlBlendState = (__bridge MTLRenderPipelineColorAttachmentDescriptor*)reinterpret_cast<void*>(blendState);
    id <MTLDepthStencilState> mtlDepthStencilState = (__bridge id)reinterpret_cast<void*>(depthStencilState);
    MTLVertexDescriptor* mtlVertexAttribute = (__bridge MTLVertexDescriptor*)reinterpret_cast<void*>(vertexAttribute);
    id <MTLFunction> mtlVertexShader = (__bridge id)reinterpret_cast<void*>(vertexShader);
    id <MTLFunction> mtlFragmentShader = (__bridge id)reinterpret_cast<void*>(fragmentShader);

    MTLPIPELINE* mtlPipeline = new MTLPIPELINE{};
    if (mtlPipeline == nullptr)
        return 0;

    MTLRenderPipelineDescriptor* desc = [classMTLRenderPipelineDescriptor new];
    desc.vertexFunction = mtlVertexShader;
    desc.fragmentFunction = mtlFragmentShader;
    desc.vertexDescriptor = mtlVertexAttribute;
    desc.colorAttachments[0] = mtlBlendState;
    desc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;

    id <MTLRenderPipelineState> pipeline = [mtlDevice newRenderPipelineStateWithDescriptor:desc
                                                                                     error:nil];

    mtlPipeline->pipeline = pipeline;
    mtlPipeline->depthStencil = mtlDepthStencilState;
    mtlPipeline->cullMode = (rasterizerState & 1) ? MTLCullModeBack : MTLCullModeNone;
    mtlPipeline->scissorEnable = (rasterizerState & 2) != 0;

    return reinterpret_cast<uint64_t>(mtlPipeline);
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateMetal(uint64_t blendState)
{
    MTLRenderPipelineColorAttachmentDescriptor* mtlBlendState = (__bridge_transfer MTLRenderPipelineColorAttachmentDescriptor*)reinterpret_cast<void*>(blendState);

    mtlBlendState = nil;
}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateMetal(uint64_t depthStencilState)
{
    id <MTLDepthStencilState> mtlDepthStencilState = (__bridge_transfer id)reinterpret_cast<void*>(depthStencilState);

    mtlDepthStencilState = nil;
}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateMetal(uint64_t rasterizerState)
{

}
//------------------------------------------------------------------------------
void xxDestroyPipelineMetal(uint64_t pipeline)
{
    MTLPIPELINE* mtlPipeline = reinterpret_cast<MTLPIPELINE*>(pipeline);

    delete mtlPipeline;
}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportMetal(uint64_t commandEncoder, int x, int y, int width, int height, float minZ, float maxZ)
{
    id <MTLRenderCommandEncoder> __unsafe_unretained mtlCommandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);

    MTLViewport vp;
    vp.originX = x;
    vp.originY = y;
    vp.width = width;
    vp.height = height;
    vp.znear = minZ;
    vp.zfar = maxZ;
    [mtlCommandEncoder setViewport:vp];

    g_metalViewport = vp;
}
//------------------------------------------------------------------------------
void xxSetScissorMetal(uint64_t commandEncoder, int x, int y, int width, int height)
{
    id <MTLRenderCommandEncoder> __unsafe_unretained mtlCommandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);

    MTLScissorRect rect;
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    [mtlCommandEncoder setScissorRect:rect];

    g_metalScissor = rect;
    g_metalScissorEnable = (width != g_metalViewport.width || height != g_metalViewport.height);
}
//------------------------------------------------------------------------------
void xxSetPipelineMetal(uint64_t commandEncoder, uint64_t pipeline)
{
    id <MTLRenderCommandEncoder> __unsafe_unretained mtlCommandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);
    MTLPIPELINE* mtlPipeline = reinterpret_cast<MTLPIPELINE*>(pipeline);

    [mtlCommandEncoder setRenderPipelineState:mtlPipeline->pipeline];
    [mtlCommandEncoder setDepthStencilState:mtlPipeline->depthStencil];
    [mtlCommandEncoder setCullMode:mtlPipeline->cullMode];

    if (g_metalScissorEnable != mtlPipeline->scissorEnable)
    {
        if (mtlPipeline->scissorEnable)
        {
            [mtlCommandEncoder setScissorRect:g_metalScissor];
            g_metalScissorEnable = true;
        }
        else
        {
            MTLScissorRect rect;
            rect.x = 0;
            rect.y = 0;
            rect.width = g_metalViewport.width;
            rect.height = g_metalViewport.height;
            [mtlCommandEncoder setScissorRect:rect];
            g_metalScissorEnable = false;
        }
    }
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersMetal(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    id <MTLRenderCommandEncoder> __unsafe_unretained mtlCommandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);
    id <MTLBuffer> __unsafe_unretained mtlBuffers[8];
    NSUInteger offsets[8];

    for (int i = 0; i < count; ++i)
    {
        MTLBUFFER* mtlBuffer = reinterpret_cast<MTLBUFFER*>(buffers[i]);
        mtlBuffers[i] = mtlBuffer->buffer;
        offsets[i] = 0;
    }

    [mtlCommandEncoder setVertexBuffers:mtlBuffers
                                offsets:offsets
                              withRange:NSMakeRange(xxGraphicDescriptor::VERTEX_BUFFER, count)];

    g_metalVertexAttribute = vertexAttribute;
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesMetal(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    id <MTLRenderCommandEncoder> __unsafe_unretained mtlCommandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);
    id <MTLTexture> __unsafe_unretained mtlTextures[8];

    for (int i = 0; i < count; ++i)
    {
        MTLTEXTURE* mtlTexture = reinterpret_cast<MTLTEXTURE*>(textures[i]);
        mtlTextures[i] = mtlTexture->texture;
    }

    [mtlCommandEncoder setVertexTextures:mtlTextures
                               withRange:NSMakeRange(0, count)];
}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesMetal(uint64_t commandEncoder, int count, const uint64_t* textures)
{
    id <MTLRenderCommandEncoder> __unsafe_unretained mtlCommandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);
    id <MTLTexture> __unsafe_unretained mtlTextures[8];

    for (int i = 0; i < count; ++i)
    {
        MTLTEXTURE* mtlTexture = reinterpret_cast<MTLTEXTURE*>(textures[i]);
        mtlTextures[i] = mtlTexture->texture;
    }

    [mtlCommandEncoder setFragmentTextures:mtlTextures
                                 withRange:NSMakeRange(0, count)];
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersMetal(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    id <MTLRenderCommandEncoder> __unsafe_unretained mtlCommandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);
    id <MTLSamplerState> __unsafe_unretained mtlSamplers[8];

    for (int i = 0; i < count; ++i)
    {
        mtlSamplers[i] = (__bridge id)reinterpret_cast<void*>(samplers[i]);
    }

    [mtlCommandEncoder setVertexSamplerStates:mtlSamplers
                                    withRange:NSMakeRange(0, count)];
}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersMetal(uint64_t commandEncoder, int count, const uint64_t* samplers)
{
    id <MTLRenderCommandEncoder> __unsafe_unretained mtlCommandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);
    id <MTLSamplerState> __unsafe_unretained mtlSamplers[8];

    for (int i = 0; i < count; ++i)
    {
        mtlSamplers[i] = (__bridge id)reinterpret_cast<void*>(samplers[i]);
    }

    [mtlCommandEncoder setFragmentSamplerStates:mtlSamplers
                                      withRange:NSMakeRange(0, count)];
}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferMetal(uint64_t commandEncoder, uint64_t buffer, int size)
{
    id <MTLRenderCommandEncoder> __unsafe_unretained mtlCommandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);
    MTLBUFFER* mtlBuffer = reinterpret_cast<MTLBUFFER*>(buffer);

    [mtlCommandEncoder setVertexBuffer:mtlBuffer->buffer
                                offset:0
                               atIndex:0];
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferMetal(uint64_t commandEncoder, uint64_t buffer, int size)
{
    id <MTLRenderCommandEncoder> __unsafe_unretained mtlCommandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);
    MTLBUFFER* mtlBuffer = reinterpret_cast<MTLBUFFER*>(buffer);

    [mtlCommandEncoder setFragmentBuffer:mtlBuffer->buffer
                                  offset:0
                                 atIndex:0];
}
//------------------------------------------------------------------------------
void xxDrawMetal(uint64_t commandEncoder, int vertexCount, int instanceCount, int firstVertex, int firstInstance)
{
    id <MTLRenderCommandEncoder> __unsafe_unretained mtlCommandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);

    [mtlCommandEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                          vertexStart:firstVertex
                          vertexCount:vertexCount
                        instanceCount:instanceCount
                         baseInstance:firstInstance];
}
//------------------------------------------------------------------------------
void xxDrawIndexedMetal(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    id <MTLRenderCommandEncoder> __unsafe_unretained mtlCommandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);
    MTLBUFFER* mtlIndexBuffer = reinterpret_cast<MTLBUFFER*>(indexBuffer);

    MTLIndexType indexType = (INDEX_BUFFER_WIDTH == /* DISABLES CODE */(2)) ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32;
#if TARGET_OS_SIMULATOR
    MTLVertexDescriptor* __unsafe_unretained desc = (__bridge id)reinterpret_cast<void*>(g_metalVertexAttribute);
    MTLVertexBufferLayoutDescriptor* __unsafe_unretained layout = desc.layouts[xxGraphicDescriptor::VERTEX_BUFFER];
    [mtlCommandEncoder setVertexBufferOffset:vertexOffset * layout.stride
                                     atIndex:xxGraphicDescriptor::VERTEX_BUFFER];
    [mtlCommandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                  indexCount:indexCount
                                   indexType:indexType
                                 indexBuffer:mtlIndexBuffer->buffer
                           indexBufferOffset:firstIndex * INDEX_BUFFER_WIDTH];
#else
    [mtlCommandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                  indexCount:indexCount
                                   indexType:indexType
                                 indexBuffer:mtlIndexBuffer->buffer
                           indexBufferOffset:firstIndex * INDEX_BUFFER_WIDTH
                               instanceCount:instanceCount
                                  baseVertex:vertexOffset
                                baseInstance:firstInstance];
#endif
}
//==============================================================================
