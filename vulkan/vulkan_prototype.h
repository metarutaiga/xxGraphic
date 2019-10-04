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

#if VK_PROTOTYPES
PFN_vkCreateInstance                                vkCreateInstance;
PFN_vkDestroyInstance                               vkDestroyInstance;
PFN_vkEnumeratePhysicalDevices                      vkEnumeratePhysicalDevices;
PFN_vkGetPhysicalDeviceFeatures                     vkGetPhysicalDeviceFeatures;
PFN_vkGetPhysicalDeviceFormatProperties             vkGetPhysicalDeviceFormatProperties;
PFN_vkGetPhysicalDeviceImageFormatProperties        vkGetPhysicalDeviceImageFormatProperties;
PFN_vkGetPhysicalDeviceProperties                   vkGetPhysicalDeviceProperties;
PFN_vkGetPhysicalDeviceQueueFamilyProperties        vkGetPhysicalDeviceQueueFamilyProperties;
PFN_vkGetPhysicalDeviceMemoryProperties             vkGetPhysicalDeviceMemoryProperties;
PFN_vkGetInstanceProcAddr                           vkGetInstanceProcAddr;
PFN_vkGetDeviceProcAddr                             vkGetDeviceProcAddr;
PFN_vkCreateDevice                                  vkCreateDevice;
PFN_vkDestroyDevice                                 vkDestroyDevice;
PFN_vkEnumerateInstanceExtensionProperties          vkEnumerateInstanceExtensionProperties;
PFN_vkEnumerateDeviceExtensionProperties            vkEnumerateDeviceExtensionProperties;
PFN_vkEnumerateInstanceLayerProperties              vkEnumerateInstanceLayerProperties;
PFN_vkEnumerateDeviceLayerProperties                vkEnumerateDeviceLayerProperties;
PFN_vkGetDeviceQueue                                vkGetDeviceQueue;
PFN_vkQueueSubmit                                   vkQueueSubmit;
PFN_vkQueueWaitIdle                                 vkQueueWaitIdle;
PFN_vkDeviceWaitIdle                                vkDeviceWaitIdle;
PFN_vkAllocateMemory                                vkAllocateMemory;
PFN_vkFreeMemory                                    vkFreeMemory;
PFN_vkMapMemory                                     vkMapMemory;
PFN_vkUnmapMemory                                   vkUnmapMemory;
PFN_vkFlushMappedMemoryRanges                       vkFlushMappedMemoryRanges;
PFN_vkInvalidateMappedMemoryRanges                  vkInvalidateMappedMemoryRanges;
PFN_vkGetDeviceMemoryCommitment                     vkGetDeviceMemoryCommitment;
PFN_vkBindBufferMemory                              vkBindBufferMemory;
PFN_vkBindImageMemory                               vkBindImageMemory;
PFN_vkGetBufferMemoryRequirements                   vkGetBufferMemoryRequirements;
PFN_vkGetImageMemoryRequirements                    vkGetImageMemoryRequirements;
PFN_vkGetImageSparseMemoryRequirements              vkGetImageSparseMemoryRequirements;
PFN_vkGetPhysicalDeviceSparseImageFormatProperties  vkGetPhysicalDeviceSparseImageFormatProperties;
PFN_vkQueueBindSparse                               vkQueueBindSparse;
PFN_vkCreateFence                                   vkCreateFence;
PFN_vkDestroyFence                                  vkDestroyFence;
PFN_vkResetFences                                   vkResetFences;
PFN_vkGetFenceStatus                                vkGetFenceStatus;
PFN_vkWaitForFences                                 vkWaitForFences;
PFN_vkCreateSemaphore                               vkCreateSemaphore;
PFN_vkDestroySemaphore                              vkDestroySemaphore;
PFN_vkCreateEvent                                   vkCreateEvent;
PFN_vkDestroyEvent                                  vkDestroyEvent;
PFN_vkGetEventStatus                                vkGetEventStatus;
PFN_vkSetEvent                                      vkSetEvent;
PFN_vkResetEvent                                    vkResetEvent;
PFN_vkCreateQueryPool                               vkCreateQueryPool;
PFN_vkDestroyQueryPool                              vkDestroyQueryPool;
PFN_vkGetQueryPoolResults                           vkGetQueryPoolResults;
PFN_vkCreateBuffer                                  vkCreateBuffer;
PFN_vkDestroyBuffer                                 vkDestroyBuffer;
PFN_vkCreateBufferView                              vkCreateBufferView;
PFN_vkDestroyBufferView                             vkDestroyBufferView;
PFN_vkCreateImage                                   vkCreateImage;
PFN_vkDestroyImage                                  vkDestroyImage;
PFN_vkGetImageSubresourceLayout                     vkGetImageSubresourceLayout;
PFN_vkCreateImageView                               vkCreateImageView;
PFN_vkDestroyImageView                              vkDestroyImageView;
PFN_vkCreateShaderModule                            vkCreateShaderModule;
PFN_vkDestroyShaderModule                           vkDestroyShaderModule;
PFN_vkCreatePipelineCache                           vkCreatePipelineCache;
PFN_vkDestroyPipelineCache                          vkDestroyPipelineCache;
PFN_vkGetPipelineCacheData                          vkGetPipelineCacheData;
PFN_vkMergePipelineCaches                           vkMergePipelineCaches;
PFN_vkCreateGraphicsPipelines                       vkCreateGraphicsPipelines;
PFN_vkCreateComputePipelines                        vkCreateComputePipelines;
PFN_vkDestroyPipeline                               vkDestroyPipeline;
PFN_vkCreatePipelineLayout                          vkCreatePipelineLayout;
PFN_vkDestroyPipelineLayout                         vkDestroyPipelineLayout;
PFN_vkCreateSampler                                 vkCreateSampler;
PFN_vkDestroySampler                                vkDestroySampler;
PFN_vkCreateDescriptorSetLayout                     vkCreateDescriptorSetLayout;
PFN_vkDestroyDescriptorSetLayout                    vkDestroyDescriptorSetLayout;
PFN_vkCreateDescriptorPool                          vkCreateDescriptorPool;
PFN_vkDestroyDescriptorPool                         vkDestroyDescriptorPool;
PFN_vkResetDescriptorPool                           vkResetDescriptorPool;
PFN_vkAllocateDescriptorSets                        vkAllocateDescriptorSets;
PFN_vkFreeDescriptorSets                            vkFreeDescriptorSets;
PFN_vkUpdateDescriptorSets                          vkUpdateDescriptorSets;
PFN_vkCreateFramebuffer                             vkCreateFramebuffer;
PFN_vkDestroyFramebuffer                            vkDestroyFramebuffer;
PFN_vkCreateRenderPass                              vkCreateRenderPass;
PFN_vkDestroyRenderPass                             vkDestroyRenderPass;
PFN_vkGetRenderAreaGranularity                      vkGetRenderAreaGranularity;
PFN_vkCreateCommandPool                             vkCreateCommandPool;
PFN_vkDestroyCommandPool                            vkDestroyCommandPool;
PFN_vkResetCommandPool                              vkResetCommandPool;
PFN_vkAllocateCommandBuffers                        vkAllocateCommandBuffers;
PFN_vkFreeCommandBuffers                            vkFreeCommandBuffers;
PFN_vkBeginCommandBuffer                            vkBeginCommandBuffer;
PFN_vkEndCommandBuffer                              vkEndCommandBuffer;
PFN_vkResetCommandBuffer                            vkResetCommandBuffer;
PFN_vkCmdBindPipeline                               vkCmdBindPipeline;
PFN_vkCmdSetViewport                                vkCmdSetViewport;
PFN_vkCmdSetScissor                                 vkCmdSetScissor;
PFN_vkCmdSetLineWidth                               vkCmdSetLineWidth;
PFN_vkCmdSetDepthBias                               vkCmdSetDepthBias;
PFN_vkCmdSetBlendConstants                          vkCmdSetBlendConstants;
PFN_vkCmdSetDepthBounds                             vkCmdSetDepthBounds;
PFN_vkCmdSetStencilCompareMask                      vkCmdSetStencilCompareMask;
PFN_vkCmdSetStencilWriteMask                        vkCmdSetStencilWriteMask;
PFN_vkCmdSetStencilReference                        vkCmdSetStencilReference;
PFN_vkCmdBindDescriptorSets                         vkCmdBindDescriptorSets;
PFN_vkCmdBindIndexBuffer                            vkCmdBindIndexBuffer;
PFN_vkCmdBindVertexBuffers                          vkCmdBindVertexBuffers;
PFN_vkCmdDraw                                       vkCmdDraw;
PFN_vkCmdDrawIndexed                                vkCmdDrawIndexed;
PFN_vkCmdDrawIndirect                               vkCmdDrawIndirect;
PFN_vkCmdDrawIndexedIndirect                        vkCmdDrawIndexedIndirect;
PFN_vkCmdDispatch                                   vkCmdDispatch;
PFN_vkCmdDispatchIndirect                           vkCmdDispatchIndirect;
PFN_vkCmdCopyBuffer                                 vkCmdCopyBuffer;
PFN_vkCmdCopyImage                                  vkCmdCopyImage;
PFN_vkCmdBlitImage                                  vkCmdBlitImage;
PFN_vkCmdCopyBufferToImage                          vkCmdCopyBufferToImage;
PFN_vkCmdCopyImageToBuffer                          vkCmdCopyImageToBuffer;
PFN_vkCmdUpdateBuffer                               vkCmdUpdateBuffer;
PFN_vkCmdFillBuffer                                 vkCmdFillBuffer;
PFN_vkCmdClearColorImage                            vkCmdClearColorImage;
PFN_vkCmdClearDepthStencilImage                     vkCmdClearDepthStencilImage;
PFN_vkCmdClearAttachments                           vkCmdClearAttachments;
PFN_vkCmdResolveImage                               vkCmdResolveImage;
PFN_vkCmdSetEvent                                   vkCmdSetEvent;
PFN_vkCmdResetEvent                                 vkCmdResetEvent;
PFN_vkCmdWaitEvents                                 vkCmdWaitEvents;
PFN_vkCmdPipelineBarrier                            vkCmdPipelineBarrier;
PFN_vkCmdBeginQuery                                 vkCmdBeginQuery;
PFN_vkCmdEndQuery                                   vkCmdEndQuery;
PFN_vkCmdResetQueryPool                             vkCmdResetQueryPool;
PFN_vkCmdWriteTimestamp                             vkCmdWriteTimestamp;
PFN_vkCmdCopyQueryPoolResults                       vkCmdCopyQueryPoolResults;
PFN_vkCmdPushConstants                              vkCmdPushConstants;
PFN_vkCmdBeginRenderPass                            vkCmdBeginRenderPass;
PFN_vkCmdNextSubpass                                vkCmdNextSubpass;
PFN_vkCmdEndRenderPass                              vkCmdEndRenderPass;
PFN_vkCmdExecuteCommands                            vkCmdExecuteCommands;

#if VK_EXT_debug_report
#undef VK_EXT_debug_report
bool                                                VK_EXT_debug_report;
PFN_vkCreateDebugReportCallbackEXT                  vkCreateDebugReportCallbackEXT;
PFN_vkDestroyDebugReportCallbackEXT                 vkDestroyDebugReportCallbackEXT;
PFN_vkDebugReportMessageEXT                         vkDebugReportMessageEXT;
#endif

#if VK_KHR_surface
PFN_vkDestroySurfaceKHR                             vkDestroySurfaceKHR;
PFN_vkGetPhysicalDeviceSurfaceSupportKHR            vkGetPhysicalDeviceSurfaceSupportKHR;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR       vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR            vkGetPhysicalDeviceSurfaceFormatsKHR;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR       vkGetPhysicalDeviceSurfacePresentModesKHR;
#endif

#if VK_KHR_swapchain
PFN_vkCreateSwapchainKHR                            vkCreateSwapchainKHR;
PFN_vkDestroySwapchainKHR                           vkDestroySwapchainKHR;
PFN_vkGetSwapchainImagesKHR                         vkGetSwapchainImagesKHR;
PFN_vkAcquireNextImageKHR                           vkAcquireNextImageKHR;
PFN_vkQueuePresentKHR                               vkQueuePresentKHR;
PFN_vkGetDeviceGroupPresentCapabilitiesKHR          vkGetDeviceGroupPresentCapabilitiesKHR;
PFN_vkGetDeviceGroupSurfacePresentModesKHR          vkGetDeviceGroupSurfacePresentModesKHR;
PFN_vkGetPhysicalDevicePresentRectanglesKHR         vkGetPhysicalDevicePresentRectanglesKHR;
PFN_vkAcquireNextImage2KHR                          vkAcquireNextImage2KHR;
#endif

#if VK_KHR_android_surface
PFN_vkCreateAndroidSurfaceKHR                       vkCreateAndroidSurfaceKHR;
#endif

#if VK_MVK_ios_surface
PFN_vkCreateIOSSurfaceMVK                           vkCreateIOSSurfaceMVK;
#endif

#if VK_MVK_macos_surface
PFN_vkCreateMacOSSurfaceMVK                         vkCreateMacOSSurfaceMVK;
#endif

#if VK_KHR_win32_surface
PFN_vkCreateWin32SurfaceKHR                         vkCreateWin32SurfaceKHR;
PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR  vkGetPhysicalDeviceWin32PresentationSupportKHR;
#endif

#if VK_KHR_push_descriptor
#undef VK_KHR_push_descriptor
bool                                                VK_KHR_push_descriptor;
PFN_vkCmdPushDescriptorSetKHR                       vkCmdPushDescriptorSetKHR;
PFN_vkCmdPushDescriptorSetWithTemplateKHR           vkCmdPushDescriptorSetWithTemplateKHR;
#endif
#endif

#endif // VULKAN_PROTOTYPE_H_
