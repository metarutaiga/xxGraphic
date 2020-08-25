#ifndef VULKAN_PROTOTYPE_H_
#define VULKAN_PROTOTYPE_H_ 1

/*
** Copyright (c) 2015-2019 The Khronos Group Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include "vk_platform.h"
#include "vulkan_core.h"

#ifndef VKEXPORT
#   if defined(_MSC_VER)
#       define VKEXPORT
#   else
#       define VKEXPORT __attribute__((visibility("hidden")))
#   endif
#endif

#if VK_PROTOTYPES
VKEXPORT PFN_vkCreateInstance                               vkCreateInstance;
VKEXPORT PFN_vkDestroyInstance                              vkDestroyInstance;
VKEXPORT PFN_vkEnumeratePhysicalDevices                     vkEnumeratePhysicalDevices;
VKEXPORT PFN_vkGetPhysicalDeviceFeatures                    vkGetPhysicalDeviceFeatures;
VKEXPORT PFN_vkGetPhysicalDeviceFormatProperties            vkGetPhysicalDeviceFormatProperties;
VKEXPORT PFN_vkGetPhysicalDeviceImageFormatProperties       vkGetPhysicalDeviceImageFormatProperties;
VKEXPORT PFN_vkGetPhysicalDeviceProperties                  vkGetPhysicalDeviceProperties;
VKEXPORT PFN_vkGetPhysicalDeviceQueueFamilyProperties       vkGetPhysicalDeviceQueueFamilyProperties;
VKEXPORT PFN_vkGetPhysicalDeviceMemoryProperties            vkGetPhysicalDeviceMemoryProperties;
VKEXPORT PFN_vkGetInstanceProcAddr                          vkGetInstanceProcAddr;
VKEXPORT PFN_vkGetDeviceProcAddr                            vkGetDeviceProcAddr;
VKEXPORT PFN_vkCreateDevice                                 vkCreateDevice;
VKEXPORT PFN_vkDestroyDevice                                vkDestroyDevice;
VKEXPORT PFN_vkEnumerateInstanceExtensionProperties         vkEnumerateInstanceExtensionProperties;
VKEXPORT PFN_vkEnumerateDeviceExtensionProperties           vkEnumerateDeviceExtensionProperties;
VKEXPORT PFN_vkEnumerateInstanceLayerProperties             vkEnumerateInstanceLayerProperties;
VKEXPORT PFN_vkEnumerateDeviceLayerProperties               vkEnumerateDeviceLayerProperties;
VKEXPORT PFN_vkGetDeviceQueue                               vkGetDeviceQueue;
VKEXPORT PFN_vkQueueSubmit                                  vkQueueSubmit;
VKEXPORT PFN_vkQueueWaitIdle                                vkQueueWaitIdle;
VKEXPORT PFN_vkDeviceWaitIdle                               vkDeviceWaitIdle;
VKEXPORT PFN_vkAllocateMemory                               vkAllocateMemory;
VKEXPORT PFN_vkFreeMemory                                   vkFreeMemory;
VKEXPORT PFN_vkMapMemory                                    vkMapMemory;
VKEXPORT PFN_vkUnmapMemory                                  vkUnmapMemory;
VKEXPORT PFN_vkFlushMappedMemoryRanges                      vkFlushMappedMemoryRanges;
VKEXPORT PFN_vkInvalidateMappedMemoryRanges                 vkInvalidateMappedMemoryRanges;
VKEXPORT PFN_vkGetDeviceMemoryCommitment                    vkGetDeviceMemoryCommitment;
VKEXPORT PFN_vkBindBufferMemory                             vkBindBufferMemory;
VKEXPORT PFN_vkBindImageMemory                              vkBindImageMemory;
VKEXPORT PFN_vkGetBufferMemoryRequirements                  vkGetBufferMemoryRequirements;
VKEXPORT PFN_vkGetImageMemoryRequirements                   vkGetImageMemoryRequirements;
VKEXPORT PFN_vkGetImageSparseMemoryRequirements             vkGetImageSparseMemoryRequirements;
VKEXPORT PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties;
VKEXPORT PFN_vkQueueBindSparse                              vkQueueBindSparse;
VKEXPORT PFN_vkCreateFence                                  vkCreateFence;
VKEXPORT PFN_vkDestroyFence                                 vkDestroyFence;
VKEXPORT PFN_vkResetFences                                  vkResetFences;
VKEXPORT PFN_vkGetFenceStatus                               vkGetFenceStatus;
VKEXPORT PFN_vkWaitForFences                                vkWaitForFences;
VKEXPORT PFN_vkCreateSemaphore                              vkCreateSemaphore;
VKEXPORT PFN_vkDestroySemaphore                             vkDestroySemaphore;
VKEXPORT PFN_vkCreateEvent                                  vkCreateEvent;
VKEXPORT PFN_vkDestroyEvent                                 vkDestroyEvent;
VKEXPORT PFN_vkGetEventStatus                               vkGetEventStatus;
VKEXPORT PFN_vkSetEvent                                     vkSetEvent;
VKEXPORT PFN_vkResetEvent                                   vkResetEvent;
VKEXPORT PFN_vkCreateQueryPool                              vkCreateQueryPool;
VKEXPORT PFN_vkDestroyQueryPool                             vkDestroyQueryPool;
VKEXPORT PFN_vkGetQueryPoolResults                          vkGetQueryPoolResults;
VKEXPORT PFN_vkCreateBuffer                                 vkCreateBuffer;
VKEXPORT PFN_vkDestroyBuffer                                vkDestroyBuffer;
VKEXPORT PFN_vkCreateBufferView                             vkCreateBufferView;
VKEXPORT PFN_vkDestroyBufferView                            vkDestroyBufferView;
VKEXPORT PFN_vkCreateImage                                  vkCreateImage;
VKEXPORT PFN_vkDestroyImage                                 vkDestroyImage;
VKEXPORT PFN_vkGetImageSubresourceLayout                    vkGetImageSubresourceLayout;
VKEXPORT PFN_vkCreateImageView                              vkCreateImageView;
VKEXPORT PFN_vkDestroyImageView                             vkDestroyImageView;
VKEXPORT PFN_vkCreateShaderModule                           vkCreateShaderModule;
VKEXPORT PFN_vkDestroyShaderModule                          vkDestroyShaderModule;
VKEXPORT PFN_vkCreatePipelineCache                          vkCreatePipelineCache;
VKEXPORT PFN_vkDestroyPipelineCache                         vkDestroyPipelineCache;
VKEXPORT PFN_vkGetPipelineCacheData                         vkGetPipelineCacheData;
VKEXPORT PFN_vkMergePipelineCaches                          vkMergePipelineCaches;
VKEXPORT PFN_vkCreateGraphicsPipelines                      vkCreateGraphicsPipelines;
VKEXPORT PFN_vkCreateComputePipelines                       vkCreateComputePipelines;
VKEXPORT PFN_vkDestroyPipeline                              vkDestroyPipeline;
VKEXPORT PFN_vkCreatePipelineLayout                         vkCreatePipelineLayout;
VKEXPORT PFN_vkDestroyPipelineLayout                        vkDestroyPipelineLayout;
VKEXPORT PFN_vkCreateSampler                                vkCreateSampler;
VKEXPORT PFN_vkDestroySampler                               vkDestroySampler;
VKEXPORT PFN_vkCreateDescriptorSetLayout                    vkCreateDescriptorSetLayout;
VKEXPORT PFN_vkDestroyDescriptorSetLayout                   vkDestroyDescriptorSetLayout;
VKEXPORT PFN_vkCreateDescriptorPool                         vkCreateDescriptorPool;
VKEXPORT PFN_vkDestroyDescriptorPool                        vkDestroyDescriptorPool;
VKEXPORT PFN_vkResetDescriptorPool                          vkResetDescriptorPool;
VKEXPORT PFN_vkAllocateDescriptorSets                       vkAllocateDescriptorSets;
VKEXPORT PFN_vkFreeDescriptorSets                           vkFreeDescriptorSets;
VKEXPORT PFN_vkUpdateDescriptorSets                         vkUpdateDescriptorSets;
VKEXPORT PFN_vkCreateFramebuffer                            vkCreateFramebuffer;
VKEXPORT PFN_vkDestroyFramebuffer                           vkDestroyFramebuffer;
VKEXPORT PFN_vkCreateRenderPass                             vkCreateRenderPass;
VKEXPORT PFN_vkDestroyRenderPass                            vkDestroyRenderPass;
VKEXPORT PFN_vkGetRenderAreaGranularity                     vkGetRenderAreaGranularity;
VKEXPORT PFN_vkCreateCommandPool                            vkCreateCommandPool;
VKEXPORT PFN_vkDestroyCommandPool                           vkDestroyCommandPool;
VKEXPORT PFN_vkResetCommandPool                             vkResetCommandPool;
VKEXPORT PFN_vkAllocateCommandBuffers                       vkAllocateCommandBuffers;
VKEXPORT PFN_vkFreeCommandBuffers                           vkFreeCommandBuffers;
VKEXPORT PFN_vkBeginCommandBuffer                           vkBeginCommandBuffer;
VKEXPORT PFN_vkEndCommandBuffer                             vkEndCommandBuffer;
VKEXPORT PFN_vkResetCommandBuffer                           vkResetCommandBuffer;
VKEXPORT PFN_vkCmdBindPipeline                              vkCmdBindPipeline;
VKEXPORT PFN_vkCmdSetViewport                               vkCmdSetViewport;
VKEXPORT PFN_vkCmdSetScissor                                vkCmdSetScissor;
VKEXPORT PFN_vkCmdSetLineWidth                              vkCmdSetLineWidth;
VKEXPORT PFN_vkCmdSetDepthBias                              vkCmdSetDepthBias;
VKEXPORT PFN_vkCmdSetBlendConstants                         vkCmdSetBlendConstants;
VKEXPORT PFN_vkCmdSetDepthBounds                            vkCmdSetDepthBounds;
VKEXPORT PFN_vkCmdSetStencilCompareMask                     vkCmdSetStencilCompareMask;
VKEXPORT PFN_vkCmdSetStencilWriteMask                       vkCmdSetStencilWriteMask;
VKEXPORT PFN_vkCmdSetStencilReference                       vkCmdSetStencilReference;
VKEXPORT PFN_vkCmdBindDescriptorSets                        vkCmdBindDescriptorSets;
VKEXPORT PFN_vkCmdBindIndexBuffer                           vkCmdBindIndexBuffer;
VKEXPORT PFN_vkCmdBindVertexBuffers                         vkCmdBindVertexBuffers;
VKEXPORT PFN_vkCmdDraw                                      vkCmdDraw;
VKEXPORT PFN_vkCmdDrawIndexed                               vkCmdDrawIndexed;
VKEXPORT PFN_vkCmdDrawIndirect                              vkCmdDrawIndirect;
VKEXPORT PFN_vkCmdDrawIndexedIndirect                       vkCmdDrawIndexedIndirect;
VKEXPORT PFN_vkCmdDispatch                                  vkCmdDispatch;
VKEXPORT PFN_vkCmdDispatchIndirect                          vkCmdDispatchIndirect;
VKEXPORT PFN_vkCmdCopyBuffer                                vkCmdCopyBuffer;
VKEXPORT PFN_vkCmdCopyImage                                 vkCmdCopyImage;
VKEXPORT PFN_vkCmdBlitImage                                 vkCmdBlitImage;
VKEXPORT PFN_vkCmdCopyBufferToImage                         vkCmdCopyBufferToImage;
VKEXPORT PFN_vkCmdCopyImageToBuffer                         vkCmdCopyImageToBuffer;
VKEXPORT PFN_vkCmdUpdateBuffer                              vkCmdUpdateBuffer;
VKEXPORT PFN_vkCmdFillBuffer                                vkCmdFillBuffer;
VKEXPORT PFN_vkCmdClearColorImage                           vkCmdClearColorImage;
VKEXPORT PFN_vkCmdClearDepthStencilImage                    vkCmdClearDepthStencilImage;
VKEXPORT PFN_vkCmdClearAttachments                          vkCmdClearAttachments;
VKEXPORT PFN_vkCmdResolveImage                              vkCmdResolveImage;
VKEXPORT PFN_vkCmdSetEvent                                  vkCmdSetEvent;
VKEXPORT PFN_vkCmdResetEvent                                vkCmdResetEvent;
VKEXPORT PFN_vkCmdWaitEvents                                vkCmdWaitEvents;
VKEXPORT PFN_vkCmdPipelineBarrier                           vkCmdPipelineBarrier;
VKEXPORT PFN_vkCmdBeginQuery                                vkCmdBeginQuery;
VKEXPORT PFN_vkCmdEndQuery                                  vkCmdEndQuery;
VKEXPORT PFN_vkCmdResetQueryPool                            vkCmdResetQueryPool;
VKEXPORT PFN_vkCmdWriteTimestamp                            vkCmdWriteTimestamp;
VKEXPORT PFN_vkCmdCopyQueryPoolResults                      vkCmdCopyQueryPoolResults;
VKEXPORT PFN_vkCmdPushConstants                             vkCmdPushConstants;
VKEXPORT PFN_vkCmdBeginRenderPass                           vkCmdBeginRenderPass;
VKEXPORT PFN_vkCmdNextSubpass                               vkCmdNextSubpass;
VKEXPORT PFN_vkCmdEndRenderPass                             vkCmdEndRenderPass;
VKEXPORT PFN_vkCmdExecuteCommands                           vkCmdExecuteCommands;

#if VK_EXT_debug_report
#undef VK_EXT_debug_report
VKEXPORT bool                                               VK_EXT_debug_report;
VKEXPORT PFN_vkCreateDebugReportCallbackEXT                 vkCreateDebugReportCallbackEXT;
VKEXPORT PFN_vkDestroyDebugReportCallbackEXT                vkDestroyDebugReportCallbackEXT;
VKEXPORT PFN_vkDebugReportMessageEXT                        vkDebugReportMessageEXT;
#endif

#if VK_KHR_surface
VKEXPORT PFN_vkDestroySurfaceKHR                            vkDestroySurfaceKHR;
VKEXPORT PFN_vkGetPhysicalDeviceSurfaceSupportKHR           vkGetPhysicalDeviceSurfaceSupportKHR;
VKEXPORT PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR      vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
VKEXPORT PFN_vkGetPhysicalDeviceSurfaceFormatsKHR           vkGetPhysicalDeviceSurfaceFormatsKHR;
VKEXPORT PFN_vkGetPhysicalDeviceSurfacePresentModesKHR      vkGetPhysicalDeviceSurfacePresentModesKHR;
#endif

#if VK_KHR_swapchain
VKEXPORT PFN_vkCreateSwapchainKHR                           vkCreateSwapchainKHR;
VKEXPORT PFN_vkDestroySwapchainKHR                          vkDestroySwapchainKHR;
VKEXPORT PFN_vkGetSwapchainImagesKHR                        vkGetSwapchainImagesKHR;
VKEXPORT PFN_vkAcquireNextImageKHR                          vkAcquireNextImageKHR;
VKEXPORT PFN_vkQueuePresentKHR                              vkQueuePresentKHR;
VKEXPORT PFN_vkGetDeviceGroupPresentCapabilitiesKHR         vkGetDeviceGroupPresentCapabilitiesKHR;
VKEXPORT PFN_vkGetDeviceGroupSurfacePresentModesKHR         vkGetDeviceGroupSurfacePresentModesKHR;
VKEXPORT PFN_vkGetPhysicalDevicePresentRectanglesKHR        vkGetPhysicalDevicePresentRectanglesKHR;
VKEXPORT PFN_vkAcquireNextImage2KHR                         vkAcquireNextImage2KHR;
#endif

#if VK_KHR_android_surface
VKEXPORT PFN_vkCreateAndroidSurfaceKHR                      vkCreateAndroidSurfaceKHR;
#endif

#if VK_MVK_ios_surface
VKEXPORT PFN_vkCreateIOSSurfaceMVK                          vkCreateIOSSurfaceMVK;
#endif

#if VK_MVK_macos_surface
VKEXPORT PFN_vkCreateMacOSSurfaceMVK                        vkCreateMacOSSurfaceMVK;
#endif

#if VK_KHR_win32_surface
VKEXPORT PFN_vkCreateWin32SurfaceKHR                        vkCreateWin32SurfaceKHR;
VKEXPORT PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR;
#endif

#if VK_KHR_push_descriptor
#undef VK_KHR_push_descriptor
VKEXPORT bool                                               VK_KHR_push_descriptor;
VKEXPORT PFN_vkCmdPushDescriptorSetKHR                      vkCmdPushDescriptorSetKHR;
VKEXPORT PFN_vkCmdPushDescriptorSetWithTemplateKHR          vkCmdPushDescriptorSetWithTemplateKHR;
#endif
#endif

#if VK_MVK_moltenvk
#undef VK_MVK_moltenvk
typedef VkResult (VKAPI_PTR *PFN_vkSetMTLTextureMVK)(VkImage image, id<MTLTexture> mtlTexture);
VKEXPORT bool                                               VK_MVK_moltenvk;
VKEXPORT PFN_vkSetMTLTextureMVK                             vkSetMTLTextureMVK;
#endif

#endif // VULKAN_PROTOTYPE_H_
