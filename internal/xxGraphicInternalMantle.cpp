//==============================================================================
// xxGraphic : Mantle Internal Source
//
// Copyright (c) 2019-2023 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphicInternalMantle.h"

//==============================================================================
//  Loader
//==============================================================================
#define GR_PROTOTYPE(type, prototype, parameter, ...) \
extern type (GR_STDCALL* prototype ## Entry) parameter; \
extern "C" type GR_STDCALL prototype parameter \
{ \
    return prototype ## Entry(__VA_ARGS__); \
} \
static void* GR_STDCALL prototype ## Dummy parameter \
{ \
    return NULL; \
} \
static type GR_STDCALL prototype ## Trunk parameter \
{ \
    prototype ## Entry = (type (GR_STDCALL*) parameter)grGetProcAddress(#prototype); \
    if (prototype ## Entry == NULL) \
        prototype ## Entry = (type (GR_STDCALL*) parameter)prototype ## Dummy; \
    return prototype ## Entry(__VA_ARGS__); \
} \
type (GR_STDCALL* prototype ## Entry) parameter = prototype ## Trunk;
//------------------------------------------------------------------------------
GR_PROTOTYPE(GR_RESULT, grInitAndEnumerateGpus, (const GR_APPLICATION_INFO* pAppInfo, const GR_ALLOC_CALLBACKS* pAllocCb, GR_UINT* pGpuCount, GR_PHYSICAL_GPU gpus[GR_MAX_PHYSICAL_GPUS]), pAppInfo, pAllocCb, pGpuCount, gpus);
GR_PROTOTYPE(GR_RESULT, grGetGpuInfo, (GR_PHYSICAL_GPU gpu, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData), gpu, infoType, pDataSize, pData);
GR_PROTOTYPE(GR_RESULT, grCreateDevice, (GR_PHYSICAL_GPU gpu, const GR_DEVICE_CREATE_INFO* pCreateInfo, GR_DEVICE* pDevice), gpu, pCreateInfo, pDevice);
GR_PROTOTYPE(GR_RESULT, grDestroyDevice, (GR_DEVICE device), device);
GR_PROTOTYPE(GR_RESULT, grGetExtensionSupport, (GR_PHYSICAL_GPU gpu, const GR_CHAR* pExtName), gpu, pExtName);
GR_PROTOTYPE(GR_RESULT, grGetDeviceQueue, (GR_DEVICE device, GR_ENUM queueType, GR_UINT queueId, GR_QUEUE* pQueue), device, queueType, queueId, pQueue);
GR_PROTOTYPE(GR_RESULT, grQueueWaitIdle, (GR_QUEUE queue), queue);
GR_PROTOTYPE(GR_RESULT, grDeviceWaitIdle, (GR_QUEUE queue), queue);
GR_PROTOTYPE(GR_RESULT, grQueueSubmit, (GR_QUEUE queue, GR_UINT cmdBufferCount, const GR_CMD_BUFFER* pCmdBuffers, GR_UINT memRefCount, const GR_MEMORY_REF* pMemRefs, GR_FENCE fence), queue, cmdBufferCount, pCmdBuffers, memRefCount, pMemRefs, fence);
GR_PROTOTYPE(GR_RESULT, grQueueSetGlobalMemReferences, (GR_QUEUE queue, GR_UINT memRefCount, const GR_MEMORY_REF* pMemRefs), queue, memRefCount, pMemRefs);
GR_PROTOTYPE(GR_RESULT, grGetMemoryHeapCount, (GR_DEVICE device, GR_UINT* pCount), device, pCount);
GR_PROTOTYPE(GR_RESULT, grGetMemoryHeapInfo, (GR_DEVICE device, GR_UINT heapId, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData), device, heapId, infoType, pDataSize, pData);
GR_PROTOTYPE(GR_RESULT, grAllocMemory, (GR_DEVICE device, const GR_MEMORY_ALLOC_INFO* pAllocInfo, GR_GPU_MEMORY* pMem), device, pAllocInfo, pMem);
GR_PROTOTYPE(GR_RESULT, grFreeMemory, (GR_GPU_MEMORY mem), mem);
GR_PROTOTYPE(GR_RESULT, grSetMemoryPriority, (GR_GPU_MEMORY mem, GR_ENUM priority), mem, priority);
GR_PROTOTYPE(GR_RESULT, grMapMemory, (GR_GPU_MEMORY mem, GR_FLAGS flags, GR_VOID** ppData), mem, flags, ppData);
GR_PROTOTYPE(GR_RESULT, grUnmapMemory, (GR_GPU_MEMORY mem), mem);
GR_PROTOTYPE(GR_RESULT, grRemapVirtualMemoryPages, (GR_DEVICE device, GR_UINT rangeCount, const GR_VIRTUAL_MEMORY_REMAP_RANGE* pRanges, GR_UINT preWaitSemaphoreCount, const GR_QUEUE_SEMAPHORE* pPreWatSemaphores, GR_UINT postSignalSemaphoreCount, const GR_QUEUE_SEMAPHORE* pPostSignalSemaphores), device, rangeCount, pRanges, preWaitSemaphoreCount, pPreWatSemaphores, postSignalSemaphoreCount, pPostSignalSemaphores);
GR_PROTOTYPE(GR_RESULT, grPinSystemMemory, (GR_DEVICE device, const GR_VOID* pSysMem, GR_SIZE memSize, GR_GPU_MEMORY* pMem), device, pSysMem, memSize, pMem);
GR_PROTOTYPE(GR_RESULT, grDestroyObject, (GR_OBJECT object), object);
GR_PROTOTYPE(GR_RESULT, grGetObjectInfo, (GR_BASE_OBJECT object, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData), object, infoType, pDataSize, pData);
GR_PROTOTYPE(GR_RESULT, grBindObjectMemory, (GR_OBJECT object, GR_GPU_MEMORY mem, GR_GPU_SIZE offset), object, mem, offset);
GR_PROTOTYPE(GR_RESULT, grGetFormatInfo, (GR_DEVICE device, GR_FORMAT format, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData), device, format, infoType, pDataSize, pData);
GR_PROTOTYPE(GR_RESULT, grCreateImage, (GR_DEVICE device, const GR_IMAGE_CREATE_INFO* pCreateInfo, GR_IMAGE* pImage), device, pCreateInfo, pImage);
GR_PROTOTYPE(GR_RESULT, grGetImageSubresourceInfo, (GR_IMAGE image, const GR_IMAGE_SUBRESOURCE* pSubresource, GR_ENUM infoType, GR_SIZE* pDataSize, GR_VOID* pData), image, pSubresource, infoType, pDataSize, pData);
GR_PROTOTYPE(GR_RESULT, grCreateSampler, (GR_DEVICE device, const GR_SAMPLER_CREATE_INFO* pCreateInfo, GR_SAMPLER* pSampler), device, pCreateInfo, pSampler);
GR_PROTOTYPE(GR_RESULT, grCreateImageView, (GR_DEVICE device, const GR_IMAGE_VIEW_CREATE_INFO* pCreateInfo, GR_IMAGE_VIEW* pView), device, pCreateInfo, pView);
GR_PROTOTYPE(GR_RESULT, grCreateColorTargetView, (GR_DEVICE device, const GR_COLOR_TARGET_VIEW_CREATE_INFO* pCreateInfo, GR_COLOR_TARGET_VIEW* pView), device, pCreateInfo, pView);
GR_PROTOTYPE(GR_RESULT, grCreateShader, (GR_DEVICE device, const GR_SHADER_CREATE_INFO* pCreateInfo, GR_SHADER* pShader), device, pCreateInfo, pShader);
GR_PROTOTYPE(GR_RESULT, grCreateGraphicsPipeline, (GR_DEVICE device, const GR_GRAPHICS_PIPELINE_CREATE_INFO* pCreateInfo, GR_PIPELINE* pPipeline), device, pCreateInfo, pPipeline);
GR_PROTOTYPE(GR_RESULT, grCreateComputePipeline, (GR_DEVICE device, const GR_COMPUTE_PIPELINE_CREATE_INFO* pCreateInfo, GR_PIPELINE* pPipeline), device, pCreateInfo, pPipeline);
GR_PROTOTYPE(GR_RESULT, grStorePipeline, (GR_PIPELINE pipeline, GR_SIZE* pDataSize, GR_VOID* pData), pipeline, pDataSize, pData);
GR_PROTOTYPE(GR_RESULT, grLoadPipeline, (GR_DEVICE device, GR_SIZE dataSize, const GR_VOID* pData, GR_PIPELINE* pPipeline), device, dataSize, pData, pPipeline);
GR_PROTOTYPE(GR_RESULT, grCreateDescriptorSet, (GR_DEVICE device, const GR_DESCRIPTOR_SET_CREATE_INFO* pCreateInfo, GR_DESCRIPTOR_SET* pDescriptorSet), device, pCreateInfo, pDescriptorSet);
GR_PROTOTYPE(GR_VOID, grBeginDescriptorSetUpdate, (GR_DESCRIPTOR_SET descriptorSet), descriptorSet);
GR_PROTOTYPE(GR_VOID, grEndDescriptorSetUpdate, (GR_DESCRIPTOR_SET descriptorSet), descriptorSet);
GR_PROTOTYPE(GR_VOID, grAttachSamplerDescriptors, (GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount, const GR_SAMPLER* pSamplers), descriptorSet, startSlot, slotCount, pSamplers);
GR_PROTOTYPE(GR_VOID, grAttachImageViewDescriptors, (GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount, const GR_IMAGE_VIEW_ATTACH_INFO* pImageViews), descriptorSet, startSlot, slotCount, pImageViews);
GR_PROTOTYPE(GR_VOID, grAttachMemoryViewDescriptors, (GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount, const GR_MEMORY_VIEW_ATTACH_INFO* pMemViews), descriptorSet, startSlot, slotCount, pMemViews);
GR_PROTOTYPE(GR_VOID, grAttachNestedDescriptors, (GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount, const GR_DESCRIPTOR_SET_ATTACH_INFO* pNestedDescriptorSets), descriptorSet, startSlot, slotCount, pNestedDescriptorSets);
GR_PROTOTYPE(GR_VOID, grClearDescriptorSetSlots, (GR_DESCRIPTOR_SET descriptorSet, GR_UINT startSlot, GR_UINT slotCount), descriptorSet, startSlot, slotCount);
GR_PROTOTYPE(GR_RESULT, grCreateViewportState, (GR_DEVICE device, const GR_VIEWPORT_STATE_CREATE_INFO* pCreateInfo, GR_VIEWPORT_STATE_OBJECT* pState), device, pCreateInfo, pState);
GR_PROTOTYPE(GR_RESULT, grCreateRasterState, (GR_DEVICE device, const GR_RASTER_STATE_CREATE_INFO* pCreateInfo, GR_RASTER_STATE_OBJECT* pState), device, pCreateInfo, pState);
GR_PROTOTYPE(GR_RESULT, grCreateColorBlendState, (GR_DEVICE device, const GR_COLOR_BLEND_STATE_CREATE_INFO* pCreateInfo, GR_COLOR_BLEND_STATE_OBJECT* pState), device, pCreateInfo, pState);
GR_PROTOTYPE(GR_RESULT, grCreateDepthStencilState, (GR_DEVICE device, const GR_DEPTH_STENCIL_STATE_CREATE_INFO* pCreateInfo, GR_DEPTH_STENCIL_STATE_OBJECT* pState), device, pCreateInfo, pState);
GR_PROTOTYPE(GR_RESULT, grCreateMsaaState, (GR_DEVICE device, const GR_MSAA_STATE_CREATE_INFO* pCreateInfo, GR_MSAA_STATE_OBJECT* pState), device, pCreateInfo, pState);
GR_PROTOTYPE(GR_RESULT, grCreateQueryPool, (GR_DEVICE device, const GR_QUERY_POOL_CREATE_INFO* pCreateInfo, GR_QUERY_POOL* pQueryPool), device, pCreateInfo, pQueryPool);
GR_PROTOTYPE(GR_RESULT, grGetQueryPoolResults, (GR_QUERY_POOL queryPool, GR_UINT startQuery, GR_UINT queryCount, GR_SIZE* pDataSize, GR_VOID* pData), queryPool, startQuery, queryCount, pDataSize, pData);
GR_PROTOTYPE(GR_RESULT, grCreateFence, (GR_DEVICE device, const GR_FENCE_CREATE_INFO* pCreateInfo, GR_FENCE* pFence), device, pCreateInfo, pFence);
GR_PROTOTYPE(GR_RESULT, grGetFenceStatus, (GR_FENCE fence), fence);
GR_PROTOTYPE(GR_RESULT, grWaitForFences, (GR_DEVICE device, GR_UINT fenceCount, const GR_FENCE* pFences, GR_BOOL waitAll, GR_FLOAT timeout), device, fenceCount, pFences, waitAll, timeout);
GR_PROTOTYPE(GR_RESULT, grCreateQueueSemaphore, (GR_DEVICE device, const GR_QUEUE_SEMAPHORE_CREATE_INFO* pCreateInfo, GR_QUEUE_SEMAPHORE* pSemaphore), device, pCreateInfo, pSemaphore);
GR_PROTOTYPE(GR_RESULT, grSignalQueueSemaphore, (GR_QUEUE queue, GR_QUEUE_SEMAPHORE semaphore), queue, semaphore);
GR_PROTOTYPE(GR_RESULT, grWaitQueueSemaphore, (GR_QUEUE queue, GR_QUEUE_SEMAPHORE semaphore), queue, semaphore);
GR_PROTOTYPE(GR_RESULT, grCreateEvent, (GR_DEVICE device, const GR_EVENT_CREATE_INFO* pCreateInfo, GR_EVENT* pEvent), device, pCreateInfo, pEvent);
GR_PROTOTYPE(GR_RESULT, grGetEventStatus, (GR_EVENT event), event);
GR_PROTOTYPE(GR_RESULT, grSetEvent, (GR_EVENT event), event);
GR_PROTOTYPE(GR_RESULT, grResetEvent, (GR_EVENT event), event);
GR_PROTOTYPE(GR_RESULT, grGetMultiGpuCompatibility, (GR_PHYSICAL_GPU gpu0, GR_PHYSICAL_GPU gpu1, GR_GPU_COMPATIBILITY_INFO* pInfo), gpu0, gpu1, pInfo);
GR_PROTOTYPE(GR_RESULT, grOpenSharedMemory, (GR_DEVICE device, const GR_MEMORY_OPEN_INFO* pOpenInfo, GR_GPU_MEMORY* pMem), device, pOpenInfo, pMem);
GR_PROTOTYPE(GR_RESULT, grOpenSharedQueueSemaphore, (GR_DEVICE device, const GR_QUEUE_SEMAPHORE_OPEN_INFO* pOpenInfo, GR_QUEUE_SEMAPHORE* pSemaphore), device, pOpenInfo, pSemaphore);
GR_PROTOTYPE(GR_RESULT, grOpenPeerMemory, (GR_DEVICE device, const GR_PEER_MEMORY_OPEN_INFO* pOpenInfo, GR_GPU_MEMORY* pMem), device, pOpenInfo, pMem);
GR_PROTOTYPE(GR_RESULT, grOpenPeerImage, (GR_DEVICE device, const GR_PEER_IMAGE_OPEN_INFO* pOpenInfo, GR_IMAGE* pImage, GR_GPU_MEMORY* pMem), device, pOpenInfo, pImage, pMem);
GR_PROTOTYPE(GR_RESULT, grCreateCommandBuffer, (GR_DEVICE device, const GR_CMD_BUFFER_CREATE_INFO* pCreateInfo, GR_CMD_BUFFER* pCmdBuffer), device, pCreateInfo, pCmdBuffer);
GR_PROTOTYPE(GR_RESULT, grBeginCommandBuffer, (GR_CMD_BUFFER cmdBuffer, GR_FLAGS flags), cmdBuffer, flags);
GR_PROTOTYPE(GR_RESULT, grEndCommandBuffer, (GR_CMD_BUFFER cmdBuffer), cmdBuffer);
GR_PROTOTYPE(GR_RESULT, grResetCommandBuffer, (GR_CMD_BUFFER cmdBuffer), cmdBuffer);
GR_PROTOTYPE(GR_VOID, grCmdBindPipeline, (GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, GR_PIPELINE pipeline), cmdBuffer, pipelineBindPoint, pipeline);
GR_PROTOTYPE(GR_VOID, grCmdBindStateObject, (GR_CMD_BUFFER cmdBuffer, GR_ENUM stateBindPoint, GR_STATE_OBJECT state), cmdBuffer, stateBindPoint, state);
GR_PROTOTYPE(GR_VOID, grCmdBindDescriptorSet, (GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, GR_UINT index, GR_DESCRIPTOR_SET descriptorSet, GR_UINT slotOffset), cmdBuffer, pipelineBindPoint, index, descriptorSet, slotOffset);
GR_PROTOTYPE(GR_VOID, grCmdBindDynamicMemoryView, (GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, const GR_MEMORY_VIEW_ATTACH_INFO* pMemView), cmdBuffer, pipelineBindPoint, pMemView);
GR_PROTOTYPE(GR_VOID, grCmdBindIndexData, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY mem, GR_GPU_SIZE offset, GR_ENUM indexType), cmdBuffer, mem, offset, indexType);
GR_PROTOTYPE(GR_VOID, grCmdBindTargets, (GR_CMD_BUFFER cmdBuffer, GR_UINT colorTargetCount, const GR_COLOR_TARGET_BIND_INFO* pColorTargets, const GR_DEPTH_STENCIL_BIND_INFO* pDepthTarget), cmdBuffer, colorTargetCount, pColorTargets, pDepthTarget);
GR_PROTOTYPE(GR_VOID, grCmdPrepareMemoryRegions, (GR_CMD_BUFFER cmdBuffer, GR_UINT transitionCount, const GR_MEMORY_STATE_TRANSITION* pStateTransitions), cmdBuffer, transitionCount, pStateTransitions);
GR_PROTOTYPE(GR_VOID, grCmdPrepareImages, (GR_CMD_BUFFER cmdBuffer, GR_UINT transitionCount, const GR_IMAGE_STATE_TRANSITION* pStateTransitions), cmdBuffer, transitionCount, pStateTransitions);
GR_PROTOTYPE(GR_VOID, grCmdDraw, (GR_CMD_BUFFER cmdBuffer, GR_UINT firstVertex, GR_UINT vertexCount, GR_UINT firstInstance, GR_UINT instanceCount), cmdBuffer, firstVertex, vertexCount, firstInstance, instanceCount);
GR_PROTOTYPE(GR_VOID, grCmdDrawIndexed, (GR_CMD_BUFFER cmdBuffer, GR_UINT firstIndex, GR_UINT indexCount, GR_INT vertexOffset, GR_UINT firstInstance, GR_UINT instanceCount), cmdBuffer, firstIndex, indexCount, vertexOffset, firstInstance, instanceCount);
GR_PROTOTYPE(GR_VOID, grCmdDrawIndirect, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY mem, GR_GPU_SIZE offset), cmdBuffer, mem, offset);
GR_PROTOTYPE(GR_VOID, grCmdDrawIndexedIndirect, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY mem, GR_GPU_SIZE offset), cmdBuffer, mem, offset);
GR_PROTOTYPE(GR_VOID, grCmdDispatch, (GR_CMD_BUFFER cmdBuffer, GR_UINT x, GR_UINT y, GR_UINT z), cmdBuffer, x, y, z);
GR_PROTOTYPE(GR_VOID, grCmdDispatchIndirect, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY mem, GR_GPU_SIZE offset), cmdBuffer, mem, offset);
GR_PROTOTYPE(GR_VOID, grCmdCopyMemory, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY srcMem, GR_GPU_MEMORY destMem, GR_UINT regionCount, const GR_MEMORY_COPY* pRegions), cmdBuffer, srcMem, destMem, regionCount, pRegions);
GR_PROTOTYPE(GR_VOID, grCmdCopyImage, (GR_CMD_BUFFER cmdBuffer, GR_IMAGE srcImage, GR_IMAGE destImage, GR_UINT regionCount, const GR_IMAGE_COPY* pRegions), cmdBuffer, srcImage, destImage, regionCount, pRegions);
GR_PROTOTYPE(GR_VOID, grCmdCopyMemoryToImage, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY srcMem, GR_IMAGE destImage, GR_UINT regionCount, const GR_MEMORY_IMAGE_COPY* pRegions), cmdBuffer, srcMem, destImage, regionCount, pRegions);
GR_PROTOTYPE(GR_VOID, grCmdCopyImageToMemory, (GR_CMD_BUFFER cmdBuffer, GR_IMAGE srcImage, GR_GPU_MEMORY destMem, GR_UINT regionCount, const GR_MEMORY_IMAGE_COPY* pRegions), cmdBuffer, srcImage, destMem, regionCount, pRegions);
GR_PROTOTYPE(GR_VOID, grCmdResolveImage, (GR_CMD_BUFFER cmdBuffer, GR_IMAGE srcImage, GR_IMAGE destImage, GR_UINT regionCount, const GR_IMAGE_RESOLVE* pRegions), cmdBuffer, srcImage, destImage, regionCount, pRegions);
GR_PROTOTYPE(GR_VOID, grCmdCloneImageData, (GR_CMD_BUFFER cmdBuffer, GR_IMAGE srcImage, GR_ENUM srcImageState, GR_IMAGE destImage, GR_ENUM destImageState), cmdBuffer, srcImage, srcImageState, destImage, destImageState);
GR_PROTOTYPE(GR_VOID, grCmdUpdateMemory, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY destMem, GR_GPU_SIZE destOffset, GR_GPU_SIZE dataSize, const GR_UINT32* pData), cmdBuffer, destMem, destOffset, dataSize, pData);
GR_PROTOTYPE(GR_VOID, grCmdFillMemory, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY destMem, GR_GPU_SIZE destOffset, GR_GPU_SIZE fillSize, GR_UINT32 data), cmdBuffer, destMem, destOffset, fillSize, data);
GR_PROTOTYPE(GR_VOID, grCmdClearColorImage, (GR_CMD_BUFFER cmdBuffer, GR_IMAGE image, const GR_FLOAT color[4], GR_UINT rangeCount, const GR_IMAGE_SUBRESOURCE_RANGE* pRanges), cmdBuffer, image, color, rangeCount, pRanges);
GR_PROTOTYPE(GR_VOID, grCmdClearColorImageRaw, (GR_CMD_BUFFER cmdBuffer, GR_IMAGE image, const GR_UINT32 color[4], GR_UINT rangeCount, const GR_IMAGE_SUBRESOURCE_RANGE* pRanges), cmdBuffer, image, color, rangeCount, pRanges);
GR_PROTOTYPE(GR_VOID, grCmdClearDepthStencil, (GR_CMD_BUFFER cmdBuffer, GR_IMAGE image, GR_FLOAT depth, GR_UINT8 stencil, GR_UINT rangeCount, const GR_IMAGE_SUBRESOURCE_RANGE* pRanges), cmdBuffer, image, depth, stencil, rangeCount, pRanges);
GR_PROTOTYPE(GR_VOID, grCmdSetEvent, (GR_CMD_BUFFER cmdBuffer, GR_EVENT event), cmdBuffer, event);
GR_PROTOTYPE(GR_VOID, grCmdResetEvent, (GR_CMD_BUFFER cmdBuffer, GR_EVENT event), cmdBuffer, event);
GR_PROTOTYPE(GR_VOID, grCmdMemoryAtomic, (GR_CMD_BUFFER cmdBuffer, GR_GPU_MEMORY destMem, GR_GPU_SIZE destOffset, GR_UINT64 srcData, GR_ENUM atomicOp), cmdBuffer, destMem, destOffset, srcData, atomicOp);
GR_PROTOTYPE(GR_VOID, grCmdBeginQuery, (GR_CMD_BUFFER cmdBuffer, GR_QUERY_POOL queryPool, GR_UINT slot, GR_FLAGS flags), cmdBuffer, queryPool, slot, flags);
GR_PROTOTYPE(GR_VOID, grCmdEndQuery, (GR_CMD_BUFFER cmdBuffer, GR_QUERY_POOL queryPool, GR_UINT slot), cmdBuffer, queryPool, slot);
GR_PROTOTYPE(GR_VOID, grCmdResetQueryPool, (GR_CMD_BUFFER cmdBuffer, GR_QUERY_POOL queryPool, GR_UINT startQuery, GR_UINT queryCount), cmdBuffer, queryPool, startQuery, queryCount);
GR_PROTOTYPE(GR_VOID, grCmdWriteTimestamp, (GR_CMD_BUFFER cmdBuffer, GR_ENUM timestampType, GR_GPU_MEMORY destMem, GR_GPU_SIZE destOffset), cmdBuffer, timestampType, destMem, destOffset);
GR_PROTOTYPE(GR_VOID, grCmdInitAtomicCounters, (GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, GR_UINT startCounter, GR_UINT counterCount, const GR_UINT32* pData), cmdBuffer, pipelineBindPoint, startCounter, counterCount, pData);
GR_PROTOTYPE(GR_VOID, grCmdLoadAtomicCounters, (GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, GR_UINT startCounter, GR_UINT counterCount, GR_GPU_MEMORY srcMem, GR_GPU_SIZE srcOffset), cmdBuffer, pipelineBindPoint, startCounter, counterCount, srcMem, srcOffset);
GR_PROTOTYPE(GR_VOID, grCmdSaveAtomicCounters, (GR_CMD_BUFFER cmdBuffer, GR_ENUM pipelineBindPoint, GR_UINT startCounter, GR_UINT counterCount, GR_GPU_MEMORY destMem, GR_GPU_SIZE destOffset), cmdBuffer, pipelineBindPoint, startCounter, counterCount, destMem, destOffset);
//------------------------------------------------------------------------------
GR_PROTOTYPE(GR_RESULT, grWsiWinGetDisplays, (GR_DEVICE device, GR_UINT* pDisplayCount, GR_WSI_WIN_DISPLAY* pDisplayList), device, pDisplayCount, pDisplayList);
GR_PROTOTYPE(GR_RESULT, grWsiWinGetDisplayModeList, (GR_WSI_WIN_DISPLAY display, GR_UINT* pDisplayModeCount, GR_WSI_WIN_DISPLAY_MODE* pDisplayModeList), display, pDisplayModeCount, pDisplayModeList);
GR_PROTOTYPE(GR_RESULT, grWsiWinTakeFullscreenOwnership, (GR_WSI_WIN_DISPLAY display, GR_IMAGE image), display, image);
GR_PROTOTYPE(GR_RESULT, grWsiWinReleaseFullscreenOwnership, (GR_WSI_WIN_DISPLAY display), display);
GR_PROTOTYPE(GR_RESULT, grWsiWinSetGammaRamp, (GR_WSI_WIN_DISPLAY display, const GR_WSI_WIN_GAMMA_RAMP* pGammaRamp), display, pGammaRamp);
GR_PROTOTYPE(GR_RESULT, grWsiWinWaitForVerticalBlank, (GR_WSI_WIN_DISPLAY display), display);
GR_PROTOTYPE(GR_RESULT, grWsiWinGetScanLine, (GR_WSI_WIN_DISPLAY display, GR_INT* pScanLine), display, pScanLine);
GR_PROTOTYPE(GR_RESULT, grWsiWinCreatePresentableImage, (GR_DEVICE device, const GR_WSI_WIN_PRESENTABLE_IMAGE_CREATE_INFO* pCreateInfo, GR_IMAGE* pImage, GR_GPU_MEMORY* pMem), device, pCreateInfo, pImage, pMem);
GR_PROTOTYPE(GR_RESULT, grWsiWinQueuePresent, (GR_QUEUE queue, const GR_WSI_WIN_PRESENT_INFO* pPresentInfo), queue, pPresentInfo);
GR_PROTOTYPE(GR_RESULT, grWsiWinSetMaxQueuedFrames, (GR_DEVICE device, GR_UINT maxFrames), device, maxFrames);
//==============================================================================
