//==============================================================================
// xxGraphic : Metal Source
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
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
static id <MTLDevice>   (*MTLCreateSystemDefaultDevice)() = nullptr;
static void*            (*MTLCopyAllDevices)() = nullptr;

//==============================================================================
//  Instance
//==============================================================================
static bool MTLSymbolFailed = false;
static void* MTLSymbol(const char* name)
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
const char* xxGetDeviceNameMetal()
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
        return 0;
    float contentsScale = [nsWindow backingScaleFactor];
#elif defined(xxIOS)
    UIWindow* nsWindow = (__bridge UIWindow*)view;
    if (nsWindow == nil)
        return 0;
    UIView* nsView = [[nsWindow rootViewController] view];
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

    MTLScissorRect rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = width;
    rect.height = height;
    [commandEncoder setScissorRect:rect];

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

    id <MTLBuffer> buffer = [mtlDevice newBufferWithLength:size
                                                   options:MTLResourceStorageModeShared];

    return reinterpret_cast<uint64_t>((__bridge_retained void*)buffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferMetal(uint64_t device, int size)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    id <MTLBuffer> buffer = [mtlDevice newBufferWithLength:size
                                                   options:MTLResourceStorageModeShared];

    return reinterpret_cast<uint64_t>((__bridge_retained void*)buffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferMetal(uint64_t device, int size, uint64_t vertexAttribute)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    id <MTLBuffer> buffer = [mtlDevice newBufferWithLength:size
                                                   options:MTLResourceStorageModeShared];

    return reinterpret_cast<uint64_t>((__bridge_retained void*)buffer);
}
//------------------------------------------------------------------------------
void xxDestroyBufferMetal(uint64_t device, uint64_t buffer)
{
    id <MTLBuffer> mtlBuffer = (__bridge_transfer id)reinterpret_cast<void*>(buffer);

    mtlBuffer = nil;
}
//------------------------------------------------------------------------------
void* xxMapBufferMetal(uint64_t device, uint64_t buffer)
{
    id <MTLBuffer> mtlBuffer = (__bridge id)reinterpret_cast<void*>(buffer);

    return [mtlBuffer contents];
}
//------------------------------------------------------------------------------
void xxUnmapBufferMetal(uint64_t device, uint64_t buffer)
{

}
//==============================================================================
//  Texture
//==============================================================================
uint64_t xxCreateTextureMetal(uint64_t device, int format, int width, int height, int depth, int mipmap, int array, const void* external)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    MTLTEXTURE* mtlTexture = new MTLTEXTURE{};
    if (mtlTexture == nullptr)
        return 0;

    MTLPixelFormat pixelFormat = MTLPixelFormatRGBA8Unorm;
#if defined(xxMACOS) || defined(xxMACCATALYST)
    MTLResourceOptions options = MTLResourceStorageModeManaged;
#elif defined(xxIOS)
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
        }
    }
    else
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

    MTLTextureDescriptor* desc = [classMTLTextureDescriptor texture2DDescriptorWithPixelFormat:pixelFormat
                                                                                         width:width
                                                                                        height:height
                                                                                     mipmapped:NO];
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
    else
    {
        texture = [buffer newTextureWithDescriptor:desc
                                            offset:0
                                       bytesPerRow:stride];
    }

    mtlTexture->texture = texture;
    mtlTexture->buffer = buffer;
    mtlTexture->stride = stride;

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

    (*stride) = mtlTexture->stride;
    return [mtlTexture->buffer contents];
}
//------------------------------------------------------------------------------
void xxUnmapTextureMetal(uint64_t device, uint64_t texture, int level, int array)
{
#if defined(xxMACOS) || defined(xxMACCATALYST)
    MTLTEXTURE* mtlTexture = reinterpret_cast<MTLTEXTURE*>(texture);

    [mtlTexture->buffer didModifyRange:NSMakeRange(0, [mtlTexture->buffer length])];
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
static const char* const defaultShaderCode =
R"(#include <metal_stdlib>
using namespace metal;

struct Uniforms
{
    float4x4 projectionMatrix;
};

struct VertexIn
{
    float3 position [[attribute(0)]];
    float4 color    [[attribute(1)]];
    float2 uv       [[attribute(2)]];
};

struct VertexOut
{
    float4 position [[position]];
    float4 color;
    float2 uv;
};

vertex VertexOut VSMain(VertexIn in [[stage_in]], constant Uniforms& uniforms [[buffer(0)]])
{
    VertexOut out;
    out.position = uniforms.projectionMatrix * float4(in.position, 1);
    out.color = in.color;
    out.uv = in.uv;
    return out;
}

fragment float4 FSMain(VertexOut in [[stage_in]], texture2d<float> tex [[texture(0)]], sampler sam [[sampler(0)]])
{
    return in.color * tex.sample(sam, in.uv);
})";
//------------------------------------------------------------------------------
uint64_t xxCreateVertexShaderMetal(uint64_t device, const char* shader, uint64_t vertexAttribute)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        shader = defaultShaderCode;
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
uint64_t xxCreateFragmentShaderMetal(uint64_t device, const char* shader)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    if (strcmp(shader, "default") == 0)
    {
        shader = defaultShaderCode;
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
uint64_t xxCreateBlendStateMetal(uint64_t device, bool blending)
{
    MTLRenderPipelineColorAttachmentDescriptor* desc = [classMTLRenderPipelineColorAttachmentDescriptor new];
    desc.pixelFormat = MTLPixelFormatBGRA8Unorm;
    desc.blendingEnabled = blending;
    desc.sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
    desc.destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    desc.rgbBlendOperation = MTLBlendOperationAdd;

    return reinterpret_cast<uint64_t>((__bridge_retained void*)desc);
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateMetal(uint64_t device, bool depthTest, bool depthWrite)
{
    id <MTLDevice> mtlDevice = (__bridge id)reinterpret_cast<void*>(device);
    if (mtlDevice == nil)
        return 0;

    MTLDepthStencilDescriptor* desc = [classMTLDepthStencilDescriptor new];
    desc.depthCompareFunction = depthTest ? MTLCompareFunctionLessEqual : MTLCompareFunctionAlways;
    desc.depthWriteEnabled = depthWrite;

    id <MTLDepthStencilState> depthStencilState = [mtlDevice newDepthStencilStateWithDescriptor:desc];

    return reinterpret_cast<uint64_t>((__bridge_retained void*)depthStencilState);
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateMetal(uint64_t device, bool cull, bool scissor)
{
    return 0;
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

    id <MTLRenderPipelineState> pipeline = [mtlDevice newRenderPipelineStateWithDescriptor:desc
                                                                                     error:nil];

    mtlPipeline->pipeline = pipeline;
    mtlPipeline->depthStencil = mtlDepthStencilState;

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
}
//------------------------------------------------------------------------------
void xxSetPipelineMetal(uint64_t commandEncoder, uint64_t pipeline)
{
    id <MTLRenderCommandEncoder> __unsafe_unretained mtlCommandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);
    MTLPIPELINE* mtlPipeline = reinterpret_cast<MTLPIPELINE*>(pipeline);

    [mtlCommandEncoder setRenderPipelineState:mtlPipeline->pipeline];
    [mtlCommandEncoder setDepthStencilState:mtlPipeline->depthStencil];
}
//------------------------------------------------------------------------------
void xxSetIndexBufferMetal(uint64_t commandEncoder, uint64_t buffer)
{

}
//------------------------------------------------------------------------------
void xxSetVertexBuffersMetal(uint64_t commandEncoder, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    id <MTLRenderCommandEncoder> __unsafe_unretained mtlCommandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);
    id <MTLBuffer> __unsafe_unretained mtlBuffers[8];
    NSUInteger offsets[8];

    for (int i = 0; i < count; ++i)
    {
        mtlBuffers[i] = (__bridge id)reinterpret_cast<void*>(buffers[i]);
        offsets[i] = 0;
    }

    [mtlCommandEncoder setVertexBuffers:mtlBuffers
                                offsets:offsets
                              withRange:NSMakeRange(xxGraphicDescriptor::VERTEX_BUFFER, count)];
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
    id <MTLBuffer> __unsafe_unretained mtlBuffer = (__bridge id)reinterpret_cast<void*>(buffer);

    [mtlCommandEncoder setVertexBuffer:mtlBuffer
                                offset:0
                               atIndex:0];
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferMetal(uint64_t commandEncoder, uint64_t buffer, int size)
{
    id <MTLRenderCommandEncoder> __unsafe_unretained mtlCommandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);
    id <MTLBuffer> __unsafe_unretained mtlBuffer = (__bridge id)reinterpret_cast<void*>(buffer);

    [mtlCommandEncoder setFragmentBuffer:mtlBuffer
                                  offset:0
                                 atIndex:0];
}
//------------------------------------------------------------------------------
void xxDrawIndexedMetal(uint64_t commandEncoder, uint64_t indexBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    id <MTLRenderCommandEncoder> __unsafe_unretained mtlCommandEncoder = (__bridge id)reinterpret_cast<void*>(commandEncoder);
    id <MTLBuffer> __unsafe_unretained mtlIndexBuffer = (__bridge id)reinterpret_cast<void*>(indexBuffer);

    MTLIndexType indexType = (INDEX_BUFFER_WIDTH == /* DISABLES CODE */(2)) ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32;
    [mtlCommandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                  indexCount:indexCount
                                   indexType:indexType
                                 indexBuffer:mtlIndexBuffer
                           indexBufferOffset:firstIndex * INDEX_BUFFER_WIDTH
                               instanceCount:instanceCount
                                  baseVertex:vertexOffset
                                baseInstance:firstInstance];
}
//==============================================================================
//  Fixed-Function
//==============================================================================
void xxSetTransformMetal(uint64_t commandEncoder, const float* world, const float* view, const float* projection)
{

}
//==============================================================================
