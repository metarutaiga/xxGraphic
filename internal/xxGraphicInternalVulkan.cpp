//==============================================================================
// xxGraphic : Vulkan Internal Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphicInternalVulkan.h"
#include "vulkan/glslang_c_interface.h"

bool VK_EXT_debug_report;
bool VK_KHR_push_descriptor;
bool VK_MVK_moltenvk;
//==============================================================================
//  Loader
//==============================================================================
#define VK_PROTOTYPE(type, prototype, parameter, ...) \
extern type (VKAPI_CALL* prototype ## Entry) parameter; \
extern "C" type VKAPI_CALL prototype parameter \
{ \
    return prototype ## Entry(__VA_ARGS__); \
} \
static void* VKAPI_CALL prototype ## Dummy parameter \
{ \
    return NULL; \
} \
static type VKAPI_CALL prototype ## Trunk parameter \
{ \
    prototype ## Entry = (type (VKAPI_CALL*) parameter)vkGetProcAddress(#prototype); \
    if (prototype ## Entry == NULL) \
        prototype ## Entry = (type (VKAPI_CALL*) parameter)prototype ## Dummy; \
    return prototype ## Entry(__VA_ARGS__); \
} \
type (VKAPI_CALL* prototype ## Entry) parameter = prototype ## Trunk;
//------------------------------------------------------------------------------
VK_PROTOTYPE(VkResult, vkCreateInstance, (const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance), pCreateInfo, pAllocator, pInstance);
VK_PROTOTYPE(void, vkDestroyInstance, (VkInstance instance, const VkAllocationCallbacks* pAllocator), instance, pAllocator);
VK_PROTOTYPE(VkResult, vkEnumeratePhysicalDevices, (VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices), instance, pPhysicalDeviceCount, pPhysicalDevices);
VK_PROTOTYPE(void, vkGetPhysicalDeviceFeatures, (VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures), physicalDevice, pFeatures);
VK_PROTOTYPE(void, vkGetPhysicalDeviceFormatProperties, (VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties* pFormatProperties), physicalDevice, format, pFormatProperties);
VK_PROTOTYPE(VkResult, vkGetPhysicalDeviceImageFormatProperties, (VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties), physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
VK_PROTOTYPE(void, vkGetPhysicalDeviceProperties, (VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties), physicalDevice, pProperties);
VK_PROTOTYPE(void, vkGetPhysicalDeviceQueueFamilyProperties, (VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties), physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
VK_PROTOTYPE(void, vkGetPhysicalDeviceMemoryProperties, (VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties), physicalDevice, pMemoryProperties);
VK_PROTOTYPE(PFN_vkVoidFunction, vkGetInstanceProcAddr, (VkInstance instance, const char* pName), instance, pName);
VK_PROTOTYPE(PFN_vkVoidFunction, vkGetDeviceProcAddr, (VkDevice device, const char* pName), device, pName);
VK_PROTOTYPE(VkResult, vkCreateDevice, (VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice), physicalDevice, pCreateInfo, pAllocator, pDevice);
VK_PROTOTYPE(void, vkDestroyDevice, (VkDevice device, const VkAllocationCallbacks* pAllocator), device, pAllocator);
VK_PROTOTYPE(VkResult, vkEnumerateInstanceExtensionProperties, (const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties), pLayerName, pPropertyCount, pProperties);
VK_PROTOTYPE(VkResult, vkEnumerateDeviceExtensionProperties, (VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties), physicalDevice, pLayerName, pPropertyCount, pProperties);
VK_PROTOTYPE(VkResult, vkEnumerateInstanceLayerProperties, (uint32_t* pPropertyCount, VkLayerProperties* pProperties), pPropertyCount, pProperties);
VK_PROTOTYPE(VkResult, vkEnumerateDeviceLayerProperties, (VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkLayerProperties* pProperties), physicalDevice, pPropertyCount, pProperties);
VK_PROTOTYPE(void, vkGetDeviceQueue, (VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue), device, queueFamilyIndex, queueIndex, pQueue);
VK_PROTOTYPE(VkResult, vkQueueSubmit, (VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence), queue, submitCount, pSubmits, fence);
VK_PROTOTYPE(VkResult, vkQueueWaitIdle, (VkQueue queue), queue);
VK_PROTOTYPE(VkResult, vkDeviceWaitIdle, (VkDevice device), device);
VK_PROTOTYPE(VkResult, vkAllocateMemory, (VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory), device, pAllocateInfo, pAllocator, pMemory);
VK_PROTOTYPE(void, vkFreeMemory, (VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks* pAllocator), device, memory, pAllocator);
VK_PROTOTYPE(VkResult, vkMapMemory, (VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData), device, memory, offset, size, flags, ppData);
VK_PROTOTYPE(void, vkUnmapMemory, (VkDevice device, VkDeviceMemory memory), device, memory);
VK_PROTOTYPE(VkResult, vkFlushMappedMemoryRanges, (VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges), device, memoryRangeCount, pMemoryRanges);
VK_PROTOTYPE(VkResult, vkInvalidateMappedMemoryRanges, (VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges), device, memoryRangeCount, pMemoryRanges);
VK_PROTOTYPE(void, vkGetDeviceMemoryCommitment, (VkDevice device, VkDeviceMemory memory, VkDeviceSize* pCommittedMemoryInBytes), device, memory, pCommittedMemoryInBytes);
VK_PROTOTYPE(VkResult, vkBindBufferMemory, (VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset), device, buffer, memory, memoryOffset);
VK_PROTOTYPE(VkResult, vkBindImageMemory, (VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset), device, image, memory, memoryOffset);
VK_PROTOTYPE(void, vkGetBufferMemoryRequirements, (VkDevice device, VkBuffer buffer, VkMemoryRequirements* pMemoryRequirements), device, buffer, pMemoryRequirements);
VK_PROTOTYPE(void, vkGetImageMemoryRequirements, (VkDevice device, VkImage image, VkMemoryRequirements* pMemoryRequirements), device, image, pMemoryRequirements);
VK_PROTOTYPE(void, vkGetImageSparseMemoryRequirements, (VkDevice device, VkImage image, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements* pSparseMemoryRequirements), device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
VK_PROTOTYPE(void, vkGetPhysicalDeviceSparseImageFormatProperties, (VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling, uint32_t* pPropertyCount, VkSparseImageFormatProperties* pProperties), physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
VK_PROTOTYPE(VkResult, vkQueueBindSparse, (VkQueue queue, uint32_t bindInfoCount, const VkBindSparseInfo* pBindInfo, VkFence fence), queue, bindInfoCount, pBindInfo, fence);
VK_PROTOTYPE(VkResult, vkCreateFence, (VkDevice device, const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence), device, pCreateInfo, pAllocator, pFence);
VK_PROTOTYPE(void, vkDestroyFence, (VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator), device, fence, pAllocator);
VK_PROTOTYPE(VkResult, vkResetFences, (VkDevice device, uint32_t fenceCount, const VkFence* pFences), device, fenceCount, pFences);
VK_PROTOTYPE(VkResult, vkGetFenceStatus, (VkDevice device, VkFence fence), device, fence);
VK_PROTOTYPE(VkResult, vkWaitForFences, (VkDevice device, uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout), device, fenceCount, pFences, waitAll, timeout);
VK_PROTOTYPE(VkResult, vkCreateSemaphore, (VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore), device, pCreateInfo, pAllocator, pSemaphore);
VK_PROTOTYPE(void, vkDestroySemaphore, (VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator), device, semaphore, pAllocator);
VK_PROTOTYPE(VkResult, vkCreateEvent, (VkDevice device, const VkEventCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkEvent* pEvent), device, pCreateInfo, pAllocator, pEvent);
VK_PROTOTYPE(void, vkDestroyEvent, (VkDevice device, VkEvent event, const VkAllocationCallbacks* pAllocator), device, event, pAllocator);
VK_PROTOTYPE(VkResult, vkGetEventStatus, (VkDevice device, VkEvent event), device, event);
VK_PROTOTYPE(VkResult, vkSetEvent, (VkDevice device, VkEvent event), device, event);
VK_PROTOTYPE(VkResult, vkResetEvent, (VkDevice device, VkEvent event), device, event);
VK_PROTOTYPE(VkResult, vkCreateQueryPool, (VkDevice device, const VkQueryPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkQueryPool* pQueryPool), device, pCreateInfo, pAllocator, pQueryPool);
VK_PROTOTYPE(void, vkDestroyQueryPool, (VkDevice device, VkQueryPool queryPool, const VkAllocationCallbacks* pAllocator), device, queryPool, pAllocator);
VK_PROTOTYPE(VkResult, vkGetQueryPoolResults, (VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, size_t dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags), device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
VK_PROTOTYPE(VkResult, vkCreateBuffer, (VkDevice device, const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer), device, pCreateInfo, pAllocator, pBuffer);
VK_PROTOTYPE(void, vkDestroyBuffer, (VkDevice device, VkBuffer buffer, const VkAllocationCallbacks* pAllocator), device, buffer, pAllocator);
VK_PROTOTYPE(VkResult, vkCreateBufferView, (VkDevice device, const VkBufferViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBufferView* pView), device, pCreateInfo, pAllocator, pView);
VK_PROTOTYPE(void, vkDestroyBufferView, (VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks* pAllocator), device, bufferView, pAllocator);
VK_PROTOTYPE(VkResult, vkCreateImage, (VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage), device, pCreateInfo, pAllocator, pImage);
VK_PROTOTYPE(void, vkDestroyImage, (VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator), device, image, pAllocator);
VK_PROTOTYPE(void, vkGetImageSubresourceLayout, (VkDevice device, VkImage image, const VkImageSubresource* pSubresource, VkSubresourceLayout* pLayout), device, image, pSubresource, pLayout);
VK_PROTOTYPE(VkResult, vkCreateImageView, (VkDevice device, const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pView), device, pCreateInfo, pAllocator, pView);
VK_PROTOTYPE(void, vkDestroyImageView, (VkDevice device, VkImageView imageView, const VkAllocationCallbacks* pAllocator), device, imageView, pAllocator);
VK_PROTOTYPE(VkResult, vkCreateShaderModule, (VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule), device, pCreateInfo, pAllocator, pShaderModule);
VK_PROTOTYPE(void, vkDestroyShaderModule, (VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks* pAllocator), device, shaderModule, pAllocator);
VK_PROTOTYPE(VkResult, vkCreatePipelineCache, (VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineCache* pPipelineCache), device, pCreateInfo, pAllocator, pPipelineCache);
VK_PROTOTYPE(void, vkDestroyPipelineCache, (VkDevice device, VkPipelineCache pipelineCache, const VkAllocationCallbacks* pAllocator), device, pipelineCache, pAllocator);
VK_PROTOTYPE(VkResult, vkGetPipelineCacheData, (VkDevice device, VkPipelineCache pipelineCache, size_t* pDataSize, void* pData), device, pipelineCache, pDataSize, pData);
VK_PROTOTYPE(VkResult, vkMergePipelineCaches, (VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount, const VkPipelineCache* pSrcCaches), device, dstCache, srcCacheCount, pSrcCaches);
VK_PROTOTYPE(VkResult, vkCreateGraphicsPipelines, (VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines), device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
VK_PROTOTYPE(VkResult, vkCreateComputePipelines, (VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines), device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
VK_PROTOTYPE(void, vkDestroyPipeline, (VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* pAllocator), device, pipeline, pAllocator);
VK_PROTOTYPE(VkResult, vkCreatePipelineLayout, (VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineLayout* pPipelineLayout), device, pCreateInfo, pAllocator, pPipelineLayout);
VK_PROTOTYPE(void, vkDestroyPipelineLayout, (VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks* pAllocator), device, pipelineLayout, pAllocator);
VK_PROTOTYPE(VkResult, vkCreateSampler, (VkDevice device, const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSampler* pSampler), device, pCreateInfo, pAllocator, pSampler);
VK_PROTOTYPE(void, vkDestroySampler, (VkDevice device, VkSampler sampler, const VkAllocationCallbacks* pAllocator), device, sampler, pAllocator);
VK_PROTOTYPE(VkResult, vkCreateDescriptorSetLayout, (VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout), device, pCreateInfo, pAllocator, pSetLayout);
VK_PROTOTYPE(void, vkDestroyDescriptorSetLayout, (VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator), device, descriptorSetLayout, pAllocator);
VK_PROTOTYPE(VkResult, vkCreateDescriptorPool, (VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorPool* pDescriptorPool), device, pCreateInfo, pAllocator, pDescriptorPool);
VK_PROTOTYPE(void, vkDestroyDescriptorPool, (VkDevice device, VkDescriptorPool descriptorPool, const VkAllocationCallbacks* pAllocator), device, descriptorPool, pAllocator);
VK_PROTOTYPE(VkResult, vkResetDescriptorPool, (VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags), device, descriptorPool, flags);
VK_PROTOTYPE(VkResult, vkAllocateDescriptorSets, (VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets), device, pAllocateInfo, pDescriptorSets);
VK_PROTOTYPE(VkResult, vkFreeDescriptorSets, (VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets), device, descriptorPool, descriptorSetCount, pDescriptorSets);
VK_PROTOTYPE(void, vkUpdateDescriptorSets, (VkDevice device, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies), device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
VK_PROTOTYPE(VkResult, vkCreateFramebuffer, (VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer), device, pCreateInfo, pAllocator, pFramebuffer);
VK_PROTOTYPE(void, vkDestroyFramebuffer, (VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator), device, framebuffer, pAllocator);
VK_PROTOTYPE(VkResult, vkCreateRenderPass, (VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass), device, pCreateInfo, pAllocator, pRenderPass);
VK_PROTOTYPE(void, vkDestroyRenderPass, (VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks* pAllocator), device, renderPass, pAllocator);
VK_PROTOTYPE(void, vkGetRenderAreaGranularity, (VkDevice device, VkRenderPass renderPass, VkExtent2D* pGranularity), device, renderPass, pGranularity);
VK_PROTOTYPE(VkResult, vkCreateCommandPool, (VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool), device, pCreateInfo, pAllocator, pCommandPool);
VK_PROTOTYPE(void, vkDestroyCommandPool, (VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator), device, commandPool, pAllocator);
VK_PROTOTYPE(VkResult, vkResetCommandPool, (VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags), device, commandPool, flags);
VK_PROTOTYPE(VkResult, vkAllocateCommandBuffers, (VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers), device, pAllocateInfo, pCommandBuffers);
VK_PROTOTYPE(void, vkFreeCommandBuffers, (VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers), device, commandPool, commandBufferCount, pCommandBuffers);
VK_PROTOTYPE(VkResult, vkBeginCommandBuffer, (VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo), commandBuffer, pBeginInfo);
VK_PROTOTYPE(VkResult, vkEndCommandBuffer, (VkCommandBuffer commandBuffer), commandBuffer);
VK_PROTOTYPE(VkResult, vkResetCommandBuffer, (VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags), commandBuffer, flags);
VK_PROTOTYPE(void, vkCmdBindPipeline, (VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline), commandBuffer, pipelineBindPoint, pipeline);
VK_PROTOTYPE(void, vkCmdSetViewport, (VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports), commandBuffer, firstViewport, viewportCount, pViewports);
VK_PROTOTYPE(void, vkCmdSetScissor, (VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors), commandBuffer, firstScissor, scissorCount, pScissors);
VK_PROTOTYPE(void, vkCmdSetLineWidth, (VkCommandBuffer commandBuffer, float lineWidth), commandBuffer, lineWidth);
VK_PROTOTYPE(void, vkCmdSetDepthBias, (VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor), commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
VK_PROTOTYPE(void, vkCmdSetBlendConstants, (VkCommandBuffer commandBuffer, const float blendConstants[4]), commandBuffer, blendConstants);
VK_PROTOTYPE(void, vkCmdSetDepthBounds, (VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds), commandBuffer, minDepthBounds, maxDepthBounds);
VK_PROTOTYPE(void, vkCmdSetStencilCompareMask, (VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t compareMask), commandBuffer, faceMask, compareMask);
VK_PROTOTYPE(void, vkCmdSetStencilWriteMask, (VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t writeMask), commandBuffer, faceMask, writeMask);
VK_PROTOTYPE(void, vkCmdSetStencilReference, (VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t reference), commandBuffer, faceMask, reference);
VK_PROTOTYPE(void, vkCmdBindDescriptorSets, (VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets), commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
VK_PROTOTYPE(void, vkCmdBindIndexBuffer, (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType), commandBuffer, buffer, offset, indexType);
VK_PROTOTYPE(void, vkCmdBindVertexBuffers, (VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets), commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
VK_PROTOTYPE(void, vkCmdDraw, (VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance), commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
VK_PROTOTYPE(void, vkCmdDrawIndexed, (VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance), commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
VK_PROTOTYPE(void, vkCmdDrawIndirect, (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride), commandBuffer, buffer, offset, drawCount, stride);
VK_PROTOTYPE(void, vkCmdDrawIndexedIndirect, (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride), commandBuffer, buffer, offset, drawCount, stride);
VK_PROTOTYPE(void, vkCmdDispatch, (VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ), commandBuffer, groupCountX, groupCountY, groupCountZ);
VK_PROTOTYPE(void, vkCmdDispatchIndirect, (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset), commandBuffer, buffer, offset);
VK_PROTOTYPE(void, vkCmdCopyBuffer, (VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions), commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
VK_PROTOTYPE(void, vkCmdCopyImage, (VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy* pRegions), commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
VK_PROTOTYPE(void, vkCmdBlitImage, (VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter), commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
VK_PROTOTYPE(void, vkCmdCopyBufferToImage, (VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions), commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
VK_PROTOTYPE(void, vkCmdCopyImageToBuffer, (VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy* pRegions), commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
VK_PROTOTYPE(void, vkCmdUpdateBuffer, (VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData), commandBuffer, dstBuffer, dstOffset, dataSize, pData);
VK_PROTOTYPE(void, vkCmdFillBuffer, (VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data), commandBuffer, dstBuffer, dstOffset, size, data);
VK_PROTOTYPE(void, vkCmdClearColorImage, (VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint32_t rangeCount, const VkImageSubresourceRange* pRanges), commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
VK_PROTOTYPE(void, vkCmdClearDepthStencilImage, (VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange* pRanges), commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
VK_PROTOTYPE(void, vkCmdClearAttachments, (VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkClearAttachment* pAttachments, uint32_t rectCount, const VkClearRect* pRects), commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
VK_PROTOTYPE(void, vkCmdResolveImage, (VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve* pRegions), commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
VK_PROTOTYPE(void, vkCmdSetEvent, (VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask), commandBuffer, event, stageMask);
VK_PROTOTYPE(void, vkCmdResetEvent, (VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask), commandBuffer, event, stageMask);
VK_PROTOTYPE(void, vkCmdWaitEvents, (VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers), commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
VK_PROTOTYPE(void, vkCmdPipelineBarrier, (VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers), commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
VK_PROTOTYPE(void, vkCmdBeginQuery, (VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags), commandBuffer, queryPool, query, flags);
VK_PROTOTYPE(void, vkCmdEndQuery, (VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query), commandBuffer, queryPool, query);
VK_PROTOTYPE(void, vkCmdResetQueryPool, (VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount), commandBuffer, queryPool, firstQuery, queryCount);
VK_PROTOTYPE(void, vkCmdWriteTimestamp, (VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query), commandBuffer, pipelineStage, queryPool, query);
VK_PROTOTYPE(void, vkCmdCopyQueryPoolResults, (VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags), commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
VK_PROTOTYPE(void, vkCmdPushConstants, (VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues), commandBuffer, layout, stageFlags, offset, size, pValues);
VK_PROTOTYPE(void, vkCmdBeginRenderPass, (VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents), commandBuffer, pRenderPassBegin, contents);
VK_PROTOTYPE(void, vkCmdNextSubpass, (VkCommandBuffer commandBuffer, VkSubpassContents contents), commandBuffer, contents);
VK_PROTOTYPE(void, vkCmdEndRenderPass, (VkCommandBuffer commandBuffer), commandBuffer);
VK_PROTOTYPE(void, vkCmdExecuteCommands, (VkCommandBuffer commandBuffer, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers), commandBuffer, commandBufferCount, pCommandBuffers);
//------------------------------------------------------------------------------
VK_PROTOTYPE(VkResult, vkCreateDebugReportCallbackEXT, (VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback), instance, pCreateInfo, pAllocator, pCallback);
VK_PROTOTYPE(void, vkDestroyDebugReportCallbackEXT, (VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator), instance, callback, pAllocator);
VK_PROTOTYPE(void, vkDebugReportMessageEXT, (VkInstance instance, VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage), instance, flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
//------------------------------------------------------------------------------
VK_PROTOTYPE(void, vkDestroySurfaceKHR, (VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks* pAllocator), instance, surface, pAllocator);
VK_PROTOTYPE(VkResult, vkGetPhysicalDeviceSurfaceSupportKHR, (VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32* pSupported), physicalDevice, queueFamilyIndex, surface, pSupported);
VK_PROTOTYPE(VkResult, vkGetPhysicalDeviceSurfaceCapabilitiesKHR, (VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities), physicalDevice, surface, pSurfaceCapabilities);
VK_PROTOTYPE(VkResult, vkGetPhysicalDeviceSurfaceFormatsKHR, (VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats), physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
VK_PROTOTYPE(VkResult, vkGetPhysicalDeviceSurfacePresentModesKHR, (VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes), physicalDevice, surface, pPresentModeCount, pPresentModes);
//------------------------------------------------------------------------------
VK_PROTOTYPE(VkResult, vkCreateSwapchainKHR, (VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain), device, pCreateInfo, pAllocator, pSwapchain);
VK_PROTOTYPE(void, vkDestroySwapchainKHR, (VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator), device, swapchain, pAllocator);
VK_PROTOTYPE(VkResult, vkGetSwapchainImagesKHR, (VkDevice device, VkSwapchainKHR swapchain, uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages), device, swapchain, pSwapchainImageCount, pSwapchainImages);
VK_PROTOTYPE(VkResult, vkAcquireNextImageKHR, (VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex), device, swapchain, timeout, semaphore, fence, pImageIndex);
VK_PROTOTYPE(VkResult, vkQueuePresentKHR, (VkQueue queue, const VkPresentInfoKHR* pPresentInfo), queue, pPresentInfo);
VK_PROTOTYPE(VkResult, vkGetDeviceGroupPresentCapabilitiesKHR, (VkDevice device, VkDeviceGroupPresentCapabilitiesKHR* pDeviceGroupPresentCapabilities), device, pDeviceGroupPresentCapabilities);
VK_PROTOTYPE(VkResult, vkGetDeviceGroupSurfacePresentModesKHR, (VkDevice device, VkSurfaceKHR surface, VkDeviceGroupPresentModeFlagsKHR* pModes), device, surface, pModes);
VK_PROTOTYPE(VkResult, vkGetPhysicalDevicePresentRectanglesKHR, (VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pRectCount, VkRect2D* pRects), physicalDevice, surface, pRectCount, pRects);
VK_PROTOTYPE(VkResult, vkAcquireNextImage2KHR, (VkDevice device, const VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pImageIndex), device, pAcquireInfo, pImageIndex);
//------------------------------------------------------------------------------
VK_PROTOTYPE(void, vkCmdPushDescriptorSetKHR, (VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites), commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
VK_PROTOTYPE(void, vkCmdPushDescriptorSetWithTemplateKHR, (VkCommandBuffer commandBuffer, VkDescriptorUpdateTemplate descriptorUpdateTemplate, VkPipelineLayout layout, uint32_t set, const void* pData), commandBuffer, descriptorUpdateTemplate, layout, set, pData);
//------------------------------------------------------------------------------
#if VK_USE_PLATFORM_ANDROID_KHR
VK_PROTOTYPE(VkResult, vkCreateAndroidSurfaceKHR, (VkInstance instance, const VkAndroidSurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface), instance, pCreateInfo, pAllocator, pSurface);
#endif
//------------------------------------------------------------------------------
#if VK_USE_PLATFORM_IOS_MVK
VK_PROTOTYPE(VkResult, vkCreateIOSSurfaceMVK, (VkInstance instance, const VkIOSSurfaceCreateInfoMVK* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface), instance, pCreateInfo, pAllocator, pSurface);
#endif
//------------------------------------------------------------------------------
#if VK_USE_PLATFORM_MACOS_MVK
VK_PROTOTYPE(VkResult, vkCreateMacOSSurfaceMVK, (VkInstance instance, const VkMacOSSurfaceCreateInfoMVK* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface), instance, pCreateInfo, pAllocator, pSurface);
#endif
//------------------------------------------------------------------------------
#if VK_USE_PLATFORM_WIN32_KHR
VK_PROTOTYPE(VkResult, vkCreateWin32SurfaceKHR, (VkInstance instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface), instance, pCreateInfo, pAllocator, pSurface);
VK_PROTOTYPE(VkBool32, vkGetPhysicalDeviceWin32PresentationSupportKHR, (VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex), physicalDevice, queueFamilyIndex);
#endif
//------------------------------------------------------------------------------
#if VK_USE_PLATFORM_MACOS_MVK || VK_USE_PLATFORM_IOS_MVK
VK_PROTOTYPE(VkResult, vkSetMTLTextureMVK, (VkImage image, id<MTLTexture> pMTLTexture), image, pMTLTexture);
VK_PROTOTYPE(void, vkGetMTLTextureMVK, (VkImage image, id<MTLTexture>* pMTLTexture), image, pMTLTexture);
#endif
//==============================================================================
//  VK_KHR_push_descriptor
//==============================================================================
static VkDevice                 g_device = VK_NULL_HANDLE;
static VkDescriptorSetLayout    g_setLayout = VK_NULL_HANDLE;
static VkPipelineLayout         g_pipelineLayout = VK_NULL_HANDLE;
static VkDescriptorPool         g_descriptorPools[256] = {};
static VkDescriptorSet          g_descriptorSetArray[65536] = {};
static VkDescriptorSet          g_descriptorSetCurrent = VK_NULL_HANDLE;
static int                      g_descriptorSetAvailable = 0;
static int                      g_descriptorSetSwapIndex = 0;
//------------------------------------------------------------------------------
static VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetKHRInternal(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites)
{
    if (g_descriptorSetCurrent == VK_NULL_HANDLE)
    {
        g_descriptorSetAvailable--;
        g_descriptorSetCurrent = g_descriptorSetArray[g_descriptorSetAvailable];
        if (xxUnlikely(g_descriptorSetCurrent == VK_NULL_HANDLE))
        {
            int descriptorSetChunk = xxCountOf(g_descriptorSetArray) / xxCountOf(g_descriptorPools);
            int descriptorPoolIndex = g_descriptorSetAvailable / descriptorSetChunk;

            VkDescriptorPool pool = g_descriptorPools[descriptorPoolIndex];
            if (xxUnlikely(pool == VK_NULL_HANDLE))
            {
                unsigned int count = xxCountOf(g_descriptorSetArray) / xxCountOf(g_descriptorPools);

                VkDescriptorPoolSize sizes[3];
                sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                sizes[0].descriptorCount = (xxGraphicDescriptor::VERTEX_UNIFORM_COUNT + xxGraphicDescriptor::FRAGMENT_UNIFORM_COUNT) * count;
                sizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                sizes[1].descriptorCount = (xxGraphicDescriptor::VERTEX_TEXTURE_COUNT + xxGraphicDescriptor::FRAGMENT_TEXTURE_COUNT) * count;
                sizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLER;
                sizes[2].descriptorCount = (xxGraphicDescriptor::VERTEX_SAMPLER_COUNT + xxGraphicDescriptor::FRAGMENT_SAMPLER_COUNT) * count;

                VkDescriptorPoolCreateInfo info = {};
                info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                info.poolSizeCount = xxCountOf(sizes);
                info.pPoolSizes = sizes;
                info.maxSets = count;
                vkCreateDescriptorPool(g_device, &info, nullptr, &pool);
                if (pool == VK_NULL_HANDLE)
                    return;

                g_descriptorPools[descriptorPoolIndex] = pool;
            }

            VkDescriptorSetAllocateInfo setInfo = {};
            setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            setInfo.descriptorPool = pool;
            setInfo.descriptorSetCount = 1;
            setInfo.pSetLayouts = &g_setLayout;
            vkAllocateDescriptorSets(g_device, &setInfo, &g_descriptorSetCurrent);
            if (g_descriptorSetCurrent == VK_NULL_HANDLE)
                return;

            g_descriptorSetArray[g_descriptorSetAvailable] = g_descriptorSetCurrent;
        }
    }

    for (uint32_t i = 0; i < descriptorWriteCount; ++i)
    {
        VkWriteDescriptorSet& kWrite = *(VkWriteDescriptorSet*)&pDescriptorWrites[i];
        kWrite.dstSet = g_descriptorSetCurrent;
    }

    vkUpdateDescriptorSets(g_device, descriptorWriteCount, pDescriptorWrites, 0, nullptr);
}
//------------------------------------------------------------------------------
VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetKHREmulate(VkDevice device, VkDescriptorSetLayout setLayout, VkPipelineLayout pipelineLayout)
{
    g_device = device;
    g_setLayout = setLayout;
    g_pipelineLayout = pipelineLayout;
    g_descriptorSetAvailable = xxCountOf(g_descriptorSetArray);
    vkCmdPushDescriptorSetKHREntry = vkCmdPushDescriptorSetKHRInternal;

    // vkQueuePresentKHR
    static VkResult (VKAPI_CALL*queuePresent)(VkQueue queue, const VkPresentInfoKHR* pPresentInfo);
    auto present = [](VkQueue queue, const VkPresentInfoKHR* pPresentInfo)
    {
        g_descriptorSetSwapIndex++;
        if (g_descriptorSetSwapIndex >= 8)
        {
            g_descriptorSetSwapIndex = 0;
            g_descriptorSetAvailable = xxCountOf(g_descriptorSetArray);
        }
        g_descriptorSetCurrent = VK_NULL_HANDLE;
        return queuePresent(queue, pPresentInfo);
    };
    (void*&)queuePresent = vkGetProcAddress("vkQueuePresentKHR");
    vkQueuePresentKHREntry = present;

    // vkDestroyDevice
    static void (VKAPI_CALL*destroyDevice)(VkDevice device, const VkAllocationCallbacks* pAllocator);
    auto destroy = [](VkDevice device, const VkAllocationCallbacks* pAllocator)
    {
        for (int i = 0; i < xxCountOf(g_descriptorPools); ++i)
        {
            VkDescriptorPool pool = g_descriptorPools[i];
            if (pool == VK_NULL_HANDLE)
                continue;
            g_descriptorPools[i] = VK_NULL_HANDLE;
            vkDestroyDescriptorPool(device, pool, pAllocator);
        }
        memset(g_descriptorSetArray, 0, sizeof(g_descriptorSetArray));
        return destroyDevice(device, pAllocator);
    };
    (void*&)destroyDevice = vkGetProcAddress("vkDestroyDevice");
    vkDestroyDeviceEntry = destroy;

    // vkCmdDraw
    static void (VKAPI_CALL*cmdDraw)(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
    auto draw = [](VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
    {
        VkDescriptorSet set = g_descriptorSetCurrent;
        if (set != VK_NULL_HANDLE)
        {
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipelineLayout, 0, 1, &set, 0, nullptr);
            g_descriptorSetCurrent = VK_NULL_HANDLE;
        }
        return cmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    };
    (void*&)cmdDraw = vkGetProcAddress("vkCmdDraw");
    vkCmdDrawEntry = draw;

    // vkCmdDrawIndexed
    static void(VKAPI_CALL*cmdDrawIndexed)(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
    auto drawIndexed = [](VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
    {
        VkDescriptorSet set = g_descriptorSetCurrent;
        if (set != VK_NULL_HANDLE)
        {
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_pipelineLayout, 0, 1, &set, 0, nullptr);
            g_descriptorSetCurrent = VK_NULL_HANDLE;
        }
        return cmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    };
    (void*&)cmdDrawIndexed = vkGetProcAddress("vkCmdDrawIndexed");
    vkCmdDrawIndexedEntry = drawIndexed;
}
//==============================================================================
//  Shader
//==============================================================================
namespace glslang {
static void*                    glslangLibrary = nullptr;
static void*                    glslangResourceLibrary = nullptr;
static void*                    spirvLibrary = nullptr;
static int                      (*initialize_process)(void);
static void                     (*finalize_process)(void);
static const glslang_resource_t*(*default_resource)(void);
static glslang_shader_t*        (*shader_create)(const glslang_input_t* input);
static void                     (*shader_delete)(glslang_shader_t* shader);
static void                     (*shader_set_preamble)(glslang_shader_t* shader, const char* s);
static void                     (*shader_set_entry_point)(void*, const char* s);
static int                      (*shader_preprocess)(glslang_shader_t* shader, const glslang_input_t* input);
static int                      (*shader_parse)(glslang_shader_t* shader, const glslang_input_t* input);
static const char*              (*shader_get_info_log)(glslang_shader_t* shader);
static glslang_program_t*       (*program_create)(void);
static void                     (*program_delete)(glslang_program_t* program);
static void                     (*program_add_shader)(glslang_program_t* program, glslang_shader_t* shader);
static int                      (*program_link)(glslang_program_t* program, int messages);
static void                     (*program_SPIRV_generate)(glslang_program_t* program, glslang_stage_t stage);
static size_t                   (*program_SPIRV_get_size)(glslang_program_t* program);
static void                     (*program_SPIRV_get)(glslang_program_t* program, unsigned int*);
static const char*              (*program_get_info_log)(glslang_program_t* program);
}
//------------------------------------------------------------------------------
VKAPI_ATTR void VKAPI_CALL vkCompileShader(char const* code, char const*const* macro, int type, uint32_t** output, size_t* size)
{
    if (glslang::glslangLibrary == nullptr || glslang::glslangResourceLibrary == nullptr || glslang::spirvLibrary == nullptr)
    {
#if defined(xxMACOS)
        if (glslang::glslangLibrary == nullptr)
            glslang::glslangLibrary = xxLoadLibrary("libglslang.dylib");
        if (glslang::glslangResourceLibrary == nullptr)
            glslang::glslangResourceLibrary = xxLoadLibrary("libglslang-default-resource-limits.dylib");
        if (glslang::spirvLibrary == nullptr)
            glslang::spirvLibrary = xxLoadLibrary("libSPIRV.dylib");
        if (glslang::glslangLibrary == nullptr)
            glslang::glslangLibrary = xxLoadLibrary("/opt/homebrew/lib/libglslang.dylib");
        if (glslang::glslangResourceLibrary == nullptr)
            glslang::glslangResourceLibrary = xxLoadLibrary("/opt/homebrew/lib/libglslang-default-resource-limits.dylib");
        if (glslang::spirvLibrary == nullptr)
            glslang::spirvLibrary = xxLoadLibrary("/opt/homebrew/lib/libSPIRV.dylib");
#endif
        if (glslang::glslangLibrary == nullptr || glslang::glslangResourceLibrary == nullptr || glslang::spirvLibrary == nullptr)
            return;
        (void*&)glslang::initialize_process = xxGetProcAddress(glslang::glslangLibrary, "glslang_initialize_process");
        (void*&)glslang::finalize_process = xxGetProcAddress(glslang::glslangLibrary, "glslang_finalize_process");
        (void*&)glslang::default_resource =xxGetProcAddress(glslang::glslangResourceLibrary, "glslang_default_resource");
        (void*&)glslang::shader_create = xxGetProcAddress(glslang::glslangLibrary, "glslang_shader_create");
        (void*&)glslang::shader_delete = xxGetProcAddress(glslang::glslangLibrary, "glslang_shader_delete");
        (void*&)glslang::shader_set_preamble = xxGetProcAddress(glslang::glslangLibrary, "glslang_shader_set_preamble");
        (void*&)glslang::shader_set_entry_point = xxGetProcAddress(glslang::glslangLibrary, "_ZN7glslang7TShader13setEntryPointEPKc");
        (void*&)glslang::shader_preprocess = xxGetProcAddress(glslang::glslangLibrary, "glslang_shader_preprocess");
        (void*&)glslang::shader_parse = xxGetProcAddress(glslang::glslangLibrary, "glslang_shader_parse");
        (void*&)glslang::shader_get_info_log = xxGetProcAddress(glslang::glslangLibrary, "glslang_shader_get_info_log");
        (void*&)glslang::program_create = xxGetProcAddress(glslang::glslangLibrary, "glslang_program_create");
        (void*&)glslang::program_delete = xxGetProcAddress(glslang::glslangLibrary, "glslang_program_delete");
        (void*&)glslang::program_add_shader = xxGetProcAddress(glslang::glslangLibrary, "glslang_program_add_shader");
        (void*&)glslang::program_link = xxGetProcAddress(glslang::glslangLibrary, "glslang_program_link");
        (void*&)glslang::program_SPIRV_generate = xxGetProcAddress(glslang::spirvLibrary, "glslang_program_SPIRV_generate");
        (void*&)glslang::program_SPIRV_get_size = xxGetProcAddress(glslang::spirvLibrary, "glslang_program_SPIRV_get_size");
        (void*&)glslang::program_SPIRV_get = xxGetProcAddress(glslang::spirvLibrary, "glslang_program_SPIRV_get");
        (void*&)glslang::program_get_info_log = xxGetProcAddress(glslang::glslangLibrary, "glslang_program_get_info_log");
    }

    glslang_input_t input =
    {
        .language = GLSLANG_SOURCE_HLSL,
        .client = GLSLANG_CLIENT_VULKAN,
        .client_version = GLSLANG_TARGET_VULKAN_1_0,
        .target_language = GLSLANG_TARGET_SPV,
        .target_language_version = GLSLANG_TARGET_SPV_1_0,
        .code = code,
        .default_version = 100,
        .default_profile = GLSLANG_NO_PROFILE,
        .force_default_version_and_profile = false,
        .forward_compatible = false,
        .messages = GLSLANG_MSG_DEFAULT_BIT,
        .resource = glslang::default_resource(),
    };
    switch (type)
    {
    case 'vert':
        input.stage = GLSLANG_STAGE_VERTEX;
        break;
    case 'frag':
        input.stage = GLSLANG_STAGE_FRAGMENT;
        break;
    }

    glslang::initialize_process();
    glslang_shader_t* shader = glslang::shader_create(&input);

    char* preamble = nullptr;
    if (macro)
    {
        size_t length = sizeof("#define Main main");
        for (size_t i = 0; i < 256; i += 2)
        {
            if (macro[i + 0] == nullptr || macro[i + 1] == nullptr)
                break;
            length += sizeof("#define");
            length += strlen(macro[i + 0]) + 1;
            length += strlen(macro[i + 1]) + 1;
        }
        preamble = xxAlloc(char, length + 1);
        strcpy(preamble, "#define Main main");
        strcat(preamble, "\n");
        for (size_t i = 0; i < 256; i += 2)
        {
            if (macro[i + 0] == nullptr || macro[i + 1] == nullptr)
                break;
            strcat(preamble, "#define");
            strcat(preamble, " ");
            strcat(preamble, macro[i + 0]);
            strcat(preamble, " ");
            strcat(preamble, macro[i + 1]);
            strcat(preamble, "\n");
        }
        glslang::shader_set_preamble(shader, preamble);
    }

    glslang::shader_set_entry_point(*(void**)shader, "main");
    if (glslang::shader_preprocess(shader, &input) == 0 || glslang::shader_parse(shader, &input) == 0)
    {
        xxLog("Glslang", "%s", glslang::shader_get_info_log(shader));
        glslang::shader_delete(shader);
        glslang::finalize_process();
        xxFree(preamble);
        return;
    }

    glslang_program_t* program = glslang::program_create();
    glslang::program_add_shader(program, shader);
    if (glslang::program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT) == 0)
    {
        xxLog("Glslang", "%s", glslang::program_get_info_log(program));
        glslang::shader_delete(shader);
        glslang::program_delete(program);
        glslang::finalize_process();
        xxFree(preamble);
        return;
    }

    glslang::program_SPIRV_generate(program, input.stage);
    glslang::shader_delete(shader);

    (*output) = xxAlloc(uint32_t, glslang::program_SPIRV_get_size(program));
    (*size) = glslang::program_SPIRV_get_size(program) * sizeof(uint32_t);
    if (*output)
    {
        glslang::program_SPIRV_get(program, *output);
    }
    glslang::program_delete(program);
    glslang::finalize_process();
    xxFree(preamble);
}
//==============================================================================
