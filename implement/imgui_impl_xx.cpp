// dear imgui: Renderer for XX
// This needs to be used along with a Platform Binding (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'uint64_t' as ImTextureID. Read the FAQ about ImTextureID in imgui.cpp.
//  [X] Renderer: Multi-viewport support. Enable with 'io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable'.
//  [X] Renderer: Support for large meshes (64k+ vertices) with 16-bits indices.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you are new to dear imgui, read examples/README.txt and read the documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include "imgui/imgui.h"
#include "imgui_impl_xx.h"

#include "graphic/xxGraphic.h"

static uint64_t g_instance = 0;
static uint64_t g_physicalDevice = 0;
static uint64_t g_device = 0;
static int      g_bufferIndex = 0;
static uint64_t g_vertexBuffers[4] = {};
static int      g_vertexBufferSizes[4] = {};
static uint64_t g_indexBuffers[4] = {};
static int      g_indexBufferSizes[4] = {};
static uint64_t g_constantBuffers[4] = {};
static uint64_t g_fontTexture = 0;
static uint64_t g_fontSampler = 0;
static uint64_t g_vertexAttribute = 0;
static uint64_t g_vertexShader = 0;
static uint64_t g_fragmentShader = 0;
static uint64_t g_blendState = 0;
static uint64_t g_depthStencilState = 0;
static uint64_t g_rasterizerState = 0;
static uint64_t g_pipeline = 0;
static bool     g_halfPixel = false;

// Forward Declarations
static void ImGui_ImplXX_InitPlatformInterface();
static void ImGui_ImplXX_ShutdownPlatformInterface();
static void ImGui_ImplXX_CreateDeviceObjectsForPlatformWindows();
static void ImGui_ImplXX_InvalidateDeviceObjectsForPlatformWindows();

static void ImGui_ImplXX_SetupRenderState(ImDrawData* draw_data, uint64_t commandBuffer, uint64_t constantBuffer)
{
    xxSetViewport(commandBuffer, 0, 0, (int)draw_data->DisplaySize.x, (int)draw_data->DisplaySize.y, 0.0f, 1.0f);

    // Setup orthographic projection matrix
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
    {
        float L = draw_data->DisplayPos.x;
        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
        float T = draw_data->DisplayPos.y;
        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

        // Half-Pixel Offset in Direct3D 8.0 / 9.0
        if (g_halfPixel)
        {
            L += 0.5f;
            R += 0.5f;
            T += 0.5f;
            B += 0.5f;
        }

        static const float identity[] =
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        float projection[] =
        {
            2.0f/(R-L),  0.0f,        0.0f, 0.0f,
            0.0f,        2.0f/(T-B),  0.0f, 0.0f,
            0.0f,        0.0f,        0.5f, 0.0f,
            (L+R)/(L-R), (T+B)/(B-T), 0.5f, 1.0f
        };

        xxSetTransform(commandBuffer, identity, identity, projection);
        void* mapConstantBuffer = xxMapBuffer(g_device, constantBuffer);
        if (mapConstantBuffer)
        {
            memcpy(mapConstantBuffer, projection, sizeof(projection));
            xxUnmapBuffer(g_device, constantBuffer);
        }
    }

    xxSetPipeline(commandBuffer, g_pipeline);
    xxSetVertexConstantBuffer(commandBuffer, constantBuffer, 16 * sizeof(float));
}

// Render function.
// (this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now call this directly from your main loop)
void ImGui_ImplXX_RenderDrawData(ImDrawData* draw_data, uint64_t commandBuffer)
{
    // Avoid rendering when minimized
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
        return;

    int&        bufferIndex = g_bufferIndex;
    uint64_t&   vertexBuffer = g_vertexBuffers[bufferIndex];
    int&        vertexBufferSize = g_vertexBufferSizes[bufferIndex];
    uint64_t&   indexBuffer = g_indexBuffers[bufferIndex];
    int&        indexBufferSize = g_indexBufferSizes[bufferIndex];
    uint64_t&   constantBuffer = g_constantBuffers[bufferIndex];

    // Swap buffer
    bufferIndex++;
    if (bufferIndex >= 4)
        bufferIndex = 0;

    // Create and grow buffers if needed
    if (vertexBuffer == 0 || vertexBufferSize < draw_data->TotalVtxCount)
    {
        xxDestroyBuffer(g_device, vertexBuffer);
        vertexBufferSize = draw_data->TotalVtxCount + 5000;
        vertexBuffer = xxCreateVertexBuffer(g_device, vertexBufferSize * sizeof(ImDrawVert));
        if (vertexBuffer == 0)
            return;
    }
    if (indexBuffer == 0 || indexBufferSize < draw_data->TotalIdxCount)
    {
        xxDestroyBuffer(g_device, indexBuffer);
        indexBufferSize = draw_data->TotalIdxCount + 10000;
        indexBuffer = xxCreateIndexBuffer(g_device, indexBufferSize * sizeof(ImDrawIdx));
        if (indexBuffer == 0)
            return;
    }

    // Copy and convert all vertices into a swapped buffer.
    ImDrawVert* vtx_dst = (ImDrawVert*)xxMapBuffer(g_device, vertexBuffer);
    ImDrawIdx* idx_dst = (ImDrawIdx*)xxMapBuffer(g_device, indexBuffer);
    if (vtx_dst == nullptr || idx_dst == nullptr)
        return;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        vtx_dst += cmd_list->VtxBuffer.Size;
        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        idx_dst += cmd_list->IdxBuffer.Size;
    }
    xxUnmapBuffer(g_device, vertexBuffer);
    xxUnmapBuffer(g_device, indexBuffer);

    xxSetVertexBuffers(commandBuffer, 1, &vertexBuffer, g_vertexAttribute);
    xxSetIndexBuffer(commandBuffer, indexBuffer);

    // Setup desired xx state
    ImGui_ImplXX_SetupRenderState(draw_data, commandBuffer, constantBuffer);

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    bool boundTexture = false;
    int global_vtx_offset = 0;
    int global_idx_offset = 0;
    ImVec2 clip_off = draw_data->DisplayPos;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != NULL)
            {
                boundTexture = false;

                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplXX_SetupRenderState(draw_data, commandBuffer, constantBuffer);
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec4 clip_rect;
                clip_rect.x = (pcmd->ClipRect.x - clip_off.x);
                clip_rect.y = (pcmd->ClipRect.y - clip_off.y);
                clip_rect.z = (pcmd->ClipRect.z - clip_off.x);
                clip_rect.w = (pcmd->ClipRect.w - clip_off.y);

                // Apply scissor/clipping rectangle
                int clip_x = (int)clip_rect.x;
                int clip_y = (int)clip_rect.y;
                int clip_width = (int)(clip_rect.z - clip_rect.x);
                int clip_height = (int)(clip_rect.w - clip_rect.y);
                xxSetScissor(commandBuffer, clip_x, clip_y, clip_width, clip_height);

                // Texture
                if (boundTexture == false)
                {
                    boundTexture = true;
                    xxSetFragmentTextures(commandBuffer, 1, &pcmd->TextureId);
                    xxSetFragmentSamplers(commandBuffer, 1, &g_fontSampler);
                }

                // Draw
                xxDrawIndexed(commandBuffer, pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
            }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }
}

bool ImGui_ImplXX_Init(uint64_t instance, uint64_t physicalDevice, uint64_t device)
{
    g_instance = instance;
    g_physicalDevice = physicalDevice;
    g_device = device;

    const char* deviceString = xxGetDeviceString(device);
    g_halfPixel = (strncmp(deviceString, "Direct3D 8", 10) == 0 || strncmp(deviceString, "Direct3D 9", 10) == 0);

    // Setup back-end capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendRendererName = "imgui_impl_xx";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        ImGui_ImplXX_InitPlatformInterface();

    return true;
}

void ImGui_ImplXX_Shutdown()
{
    ImGui_ImplXX_ShutdownPlatformInterface();
    ImGui_ImplXX_InvalidateDeviceObjects();
    g_device = 0;
    g_physicalDevice = 0;
    g_instance = 0;
}

static bool ImGui_ImplXX_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height, bytes_per_pixel;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

    // Upload texture to graphics system
    xxDestroyTexture(g_fontTexture);
    g_fontTexture = xxCreateTexture(g_device, 0, width, height, 1, 1, 1);
    if (g_fontTexture == 0)
        return false;
    unsigned int stride = 0;
    void* map = xxMapTexture(g_device, g_fontTexture, stride, 0, 0, 1);
    if (map == nullptr)
        return false;
    for (int y = 0; y < height; y++)
        memcpy((char*)map + stride * y, pixels + (width * bytes_per_pixel) * y, (width * bytes_per_pixel));
    xxUnmapTexture(g_device, g_fontTexture, 0, 0, 1);

    // Store our identifier
    io.Fonts->TexID = (ImTextureID)g_fontTexture;

    return true;
}

bool ImGui_ImplXX_CreateDeviceObjects()
{
    if (g_device == 0)
        return false;
    if (ImGui_ImplXX_CreateFontsTexture() == false)
        return false;
    ImGui_ImplXX_CreateDeviceObjectsForPlatformWindows();

    ImDrawVert vert;
    g_fontSampler = xxCreateSampler(g_device, false, false, false, true, true, true, 1);
    g_vertexAttribute = xxCreateVertexAttribute(g_device, 3, 0, xxOffsetOf(vert, pos),  3, xxSizeOf(vert.pos) + xxSizeOf(vert.z),
                                                             0, xxOffsetOf(vert, col),  4, xxSizeOf(vert.col),
                                                             0, xxOffsetOf(vert, uv),   2, xxSizeOf(vert.uv));
    g_vertexShader = xxCreateVertexShader(g_device, "default", g_vertexAttribute);
    g_fragmentShader = xxCreateFragmentShader(g_device, "default");
    for (unsigned int i = 0; i < 4; ++i)
    {
        g_constantBuffers[i] = xxCreateConstantBuffer(g_device, 16 * sizeof(float));
    }
    g_blendState = xxCreateBlendState(g_device, true);
    g_depthStencilState = xxCreateDepthStencilState(g_device, false, false);
    g_rasterizerState = xxCreateRasterizerState(g_device, false, true);
    g_pipeline = xxCreatePipeline(g_device, g_blendState, g_depthStencilState, g_rasterizerState, g_vertexAttribute, g_vertexShader, g_fragmentShader);

    return true;
}

void ImGui_ImplXX_InvalidateDeviceObjects()
{
    if (g_device == 0)
        return;
    for (unsigned int i = 0; i < 4; ++i)
    {
        xxDestroyBuffer(g_device, g_vertexBuffers[i]);
        xxDestroyBuffer(g_device, g_indexBuffers[i]);
        xxDestroyBuffer(g_device, g_constantBuffers[i]);
        g_vertexBuffers[i] = 0;
        g_indexBuffers[i] = 0;
        g_constantBuffers[i] = 0;
    }
    xxDestroyTexture(g_fontTexture);
    xxDestroySampler(g_fontSampler);
    xxDestroyVertexAttribute(g_vertexAttribute);
    xxDestroyShader(g_device, g_vertexShader);
    xxDestroyShader(g_device, g_fragmentShader);
    xxDestroyBlendState(g_blendState);
    xxDestroyDepthStencilState(g_depthStencilState);
    xxDestroyRasterizerState(g_rasterizerState);
    xxDestroyPipeline(g_pipeline);
    g_fontTexture = 0;
    g_fontSampler = 0;
    g_vertexAttribute = 0;
    g_vertexShader = 0;
    g_fragmentShader = 0;
    g_blendState = 0;
    g_depthStencilState = 0;
    g_rasterizerState = 0;
    g_pipeline = 0;
    ImGui_ImplXX_InvalidateDeviceObjectsForPlatformWindows();
}

void ImGui_ImplXX_NewFrame()
{
    if (g_fontTexture == 0)
        ImGui_ImplXX_CreateDeviceObjects();
}

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the back-end to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

struct ImGuiViewportDataXX
{
    uint64_t                Swapchain;
    uint64_t                RenderPass;
    HWND                    Handle;

    ImGuiViewportDataXX()   { Swapchain = 0; Handle = nullptr; }
    ~ImGuiViewportDataXX()  { IM_ASSERT(Swapchain == 0); IM_ASSERT(RenderPass == 0); }
};

static void ImGui_ImplXX_CreateWindow(ImGuiViewport* viewport)
{
    ImGuiViewportDataXX* data = IM_NEW(ImGuiViewportDataXX)();
    viewport->RendererUserData = data;

    // PlatformHandleRaw should always be a HWND, whereas PlatformHandle might be a higher-level handle (e.g. GLFWWindow*, SDL_Window*).
    // Some back-ends will leave PlatformHandleRaw NULL, in which case we assume PlatformHandle will contain the HWND.
    HWND hWnd = viewport->PlatformHandleRaw ? (HWND)viewport->PlatformHandleRaw : (HWND)viewport->PlatformHandle;
    IM_ASSERT(hWnd != 0);

    data->Swapchain = xxCreateSwapchain(g_device, hWnd, (int)viewport->Size.x, (int)viewport->Size.y);
    data->RenderPass = xxCreateRenderPass(g_device, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0);
    data->Handle = hWnd;
    IM_ASSERT(data->Swapchain != 0);
    IM_ASSERT(data->RenderPass != 0);
}

static void ImGui_ImplXX_DestroyWindow(ImGuiViewport* viewport)
{
    // The main viewport (owned by the application) will always have RendererUserData == NULL since we didn't create the data for it.
    if (ImGuiViewportDataXX* data = (ImGuiViewportDataXX*)viewport->RendererUserData)
    {
        xxDestroySwapchain(data->Swapchain);
        xxDestroyRenderPass(data->RenderPass);
        data->Swapchain = 0;
        data->RenderPass = 0;
        IM_DELETE(data);
    }
    viewport->RendererUserData = NULL;
}

static void ImGui_ImplXX_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
    ImGuiViewportDataXX* data = (ImGuiViewportDataXX*)viewport->RendererUserData;
    xxDestroySwapchain(data->Swapchain);
    data->Swapchain = xxCreateSwapchain(g_device, data->Handle, (int)size.x, (int)size.y);
    IM_ASSERT(data->Swapchain != 0);
}

static void ImGui_ImplXX_RenderWindow(ImGuiViewport* viewport, void*)
{
    ImGuiViewportDataXX* data = (ImGuiViewportDataXX*)viewport->RendererUserData;

    uint64_t commandBuffer = xxGetCommandBuffer(g_device, data->Swapchain);
    uint64_t framebuffer = xxGetFramebuffer(g_device, data->Swapchain);
    xxBeginCommandBuffer(commandBuffer);
    xxBeginRenderPass(commandBuffer, framebuffer, data->RenderPass);

    ImGui_ImplXX_RenderDrawData(viewport->DrawData, commandBuffer);

    xxEndRenderPass(commandBuffer, framebuffer, data->RenderPass);
    xxEndCommandBuffer(commandBuffer);
    xxSubmitCommandBuffer(commandBuffer);
}

static void ImGui_ImplXX_SwapBuffers(ImGuiViewport* viewport, void*)
{
    ImGuiViewportDataXX* data = (ImGuiViewportDataXX*)viewport->RendererUserData;
    xxPresentSwapchain(data->Swapchain);
}

static void ImGui_ImplXX_InitPlatformInterface()
{
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Renderer_CreateWindow = ImGui_ImplXX_CreateWindow;
    platform_io.Renderer_DestroyWindow = ImGui_ImplXX_DestroyWindow;
    platform_io.Renderer_SetWindowSize = ImGui_ImplXX_SetWindowSize;
    platform_io.Renderer_RenderWindow = ImGui_ImplXX_RenderWindow;
    platform_io.Renderer_SwapBuffers = ImGui_ImplXX_SwapBuffers;
}

static void ImGui_ImplXX_ShutdownPlatformInterface()
{
    ImGui::DestroyPlatformWindows();
}

static void ImGui_ImplXX_CreateDeviceObjectsForPlatformWindows()
{
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    for (int i = 1; i < platform_io.Viewports.Size; i++)
        if (!platform_io.Viewports[i]->RendererUserData)
            ImGui_ImplXX_CreateWindow(platform_io.Viewports[i]);
}

static void ImGui_ImplXX_InvalidateDeviceObjectsForPlatformWindows()
{
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    for (int i = 1; i < platform_io.Viewports.Size; i++)
        if (platform_io.Viewports[i]->RendererUserData)
            ImGui_ImplXX_DestroyWindow(platform_io.Viewports[i]);
}
