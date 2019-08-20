#pragma once

#define xxRegisterFunction(API) \
    xxCreateInstance = xxCreateInstance ## API; \
    xxDestroyInstance = xxDestroyInstance ## API; \
\
    xxCreateDevice = xxCreateDevice ## API; \
    xxDestroyDevice = xxDestroyDevice ## API; \
    xxResetDevice = xxResetDevice ## API; \
    xxTestDevice = xxTestDevice ## API; \
\
    xxCreateSwapchain = xxCreateSwapchain ## API; \
    xxDestroySwapchain = xxDestroySwapchain ## API; \
    xxPresentSwapchain = xxPresentSwapchain ## API; \
\
    xxGetCommandBuffer = xxGetCommandBuffer ## API; \
    xxBeginCommandBuffer = xxBeginCommandBuffer ## API; \
    xxEndCommandBuffer = xxEndCommandBuffer ## API; \
    xxSubmitCommandBuffer = xxSubmitCommandBuffer ## API; \
\
    xxCreateRenderPass = xxCreateRenderPass ## API; \
    xxDestroyRenderPass = xxDestroyRenderPass ## API; \
    xxBeginRenderPass = xxBeginRenderPass ## API; \
    xxEndRenderPass = xxEndRenderPass ## API; \
\
    xxCreateBuffer = xxCreateBuffer ## API; \
    xxDestroyBuffer = xxDestroyBuffer ## API; \
    xxMapBuffer = xxMapBuffer ## API; \
    xxUnmapBuffer = xxUnmapBuffer ## API; \
\
    xxCreateTexture = xxCreateTexture ## API; \
    xxDestroyTexture = xxDestroyTexture ## API; \
    xxMapTexture = xxMapTexture ## API; \
    xxUnmapTexture = xxUnmapTexture ## API; \
\
    xxCreateVertexAttribute = xxCreateVertexAttribute ## API; \
    xxDestroyVertexAttribute = xxDestroyVertexAttribute ## API; \
\
    xxSetViewport = xxSetViewport ## API; \
    xxSetScissor = xxSetScissor ## API; \
\
    xxSetIndexBuffer = xxSetIndexBuffer ## API; \
    xxSetVertexBuffers = xxSetVertexBuffers ## API; \
    xxSetFragmentBuffers = xxSetFragmentBuffers ## API; \
\
    xxSetVertexTextures = xxSetVertexTextures ## API; \
    xxSetFragmentTextures = xxSetFragmentTextures ## API; \
\
    xxSetVertexAttribute = xxSetVertexAttribute ## API; \
\
    xxDrawIndexed = xxDrawIndexed ## API; \
\
    xxSetOrthographicTransform = xxSetOrthographicTransform ## API;
