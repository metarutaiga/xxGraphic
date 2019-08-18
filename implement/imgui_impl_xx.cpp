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

static uint64_t                 g_instance = 0;
static uint64_t                 g_physicalDevice = 0;
static uint64_t                 g_device = 0;
static int                      g_bufferIndex = 0;
static uint64_t                 g_vertexBuffers[4] = {};
static int                      g_vertexBufferSizes[4] = {};
static uint64_t                 g_indexBuffers[4] = {};
static int                      g_indexBufferSizes[4] = {};
static uint64_t                 g_fontTexture = 0;

struct CUSTOMVERTEX
{
    float           pos[3];
    unsigned int    color;
    float           uv[2];
};

// Forward Declarations
static void ImGui_ImplXX_InitPlatformInterface();
static void ImGui_ImplXX_ShutdownPlatformInterface();
static void ImGui_ImplXX_CreateDeviceObjectsForPlatformWindows();
static void ImGui_ImplXX_InvalidateDeviceObjectsForPlatformWindows();

static void ImGui_ImplXX_SetupRenderState(ImDrawData* draw_data, uint64_t commandBuffer)
{
    xxSetViewport(commandBuffer, 0, 0, (int)draw_data->DisplaySize.x, (int)draw_data->DisplaySize.y, 0.0f, 1.0f);

    // Setup orthographic projection matrix
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
    {
        float L = draw_data->DisplayPos.x + 0.5f;
        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x + 0.5f;
        float T = draw_data->DisplayPos.y + 0.5f;
        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y + 0.5f;
        xxSetOrthographicTransform(commandBuffer, L, R, T, B);
    }
}

// Render function.
// (this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now call this directly from your main loop)
void ImGui_ImplXX_RenderDrawData(ImDrawData* draw_data, uint64_t commandBuffer)
{
    // Avoid rendering when minimized
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
        return;

    int         bufferIndex = g_bufferIndex;
    uint64_t&   vertexBuffer = g_vertexBuffers[bufferIndex];
    int&        vertexBufferSize = g_vertexBufferSizes[bufferIndex];
    uint64_t&   indexBuffer = g_indexBuffers[bufferIndex];
    int&        indexBufferSize = g_indexBufferSizes[bufferIndex];

    // Swap buffer
    g_bufferIndex++;
    if (g_bufferIndex >= 3)
        g_bufferIndex = 0;

    // Create and grow buffers if needed
    if (vertexBuffer == 0 || vertexBufferSize < draw_data->TotalVtxCount)
    {
        xxDestroyBuffer(vertexBuffer);
        vertexBufferSize = draw_data->TotalVtxCount + 5000;
        vertexBuffer = xxCreateBuffer(g_device, vertexBufferSize * sizeof(CUSTOMVERTEX));
        if (vertexBuffer == 0)
            return;
    }
    if (indexBuffer == 0 || indexBufferSize < draw_data->TotalIdxCount)
    {
        xxDestroyBuffer(indexBuffer);
        indexBufferSize = draw_data->TotalIdxCount + 10000;
        indexBuffer = xxCreateBuffer(g_device, indexBufferSize * sizeof(ImDrawIdx), true);
        if (indexBuffer == 0)
            return;
    }

    // Copy and convert all vertices into a single contiguous buffer, convert colors to DX9 default format.
    // FIXME-OPT: This is a waste of resource, the ideal is to use imconfig.h and
    //  1) to avoid repacking colors:   #define IMGUI_USE_BGRA_PACKED_COLOR
    //  2) to avoid repacking vertices: #define IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT struct ImDrawVert { ImVec2 pos; float z; ImU32 col; ImVec2 uv; }
    CUSTOMVERTEX* vtx_dst = (CUSTOMVERTEX*)xxMapBuffer(vertexBuffer);
    ImDrawIdx* idx_dst = (ImDrawIdx*)xxMapBuffer(indexBuffer);
    if (vtx_dst == nullptr || idx_dst == nullptr)
        return;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
        for (int i = 0; i < cmd_list->VtxBuffer.Size; i++)
        {
            vtx_dst->pos[0] = vtx_src->pos.x;
            vtx_dst->pos[1] = vtx_src->pos.y;
            vtx_dst->pos[2] = 0.0f;
            vtx_dst->color = (vtx_src->col & 0xFF00FF00) | ((vtx_src->col & 0xFF0000) >> 16) | ((vtx_src->col & 0xFF) << 16);
            vtx_dst->uv[0] = vtx_src->uv.x;
            vtx_dst->uv[1] = vtx_src->uv.y;
            vtx_dst++;
            vtx_src++;
        }
        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        idx_dst += cmd_list->IdxBuffer.Size;
    }
    xxUnmapBuffer(vertexBuffer);
    xxUnmapBuffer(indexBuffer);

    int offsets[] = { 0 };
    int strides[] = { sizeof(CUSTOMVERTEX) };
    xxSetVertexBuffers(commandBuffer, &vertexBuffer, offsets, strides, 1);
    xxSetIndexBuffer(commandBuffer, indexBuffer);

    // Setup desired xx state
    ImGui_ImplXX_SetupRenderState(draw_data, commandBuffer);

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
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
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplXX_SetupRenderState(draw_data, commandBuffer);
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

                // Draw
                xxSetFragmentTextures(commandBuffer, &pcmd->TextureId, 1);
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
    g_fontTexture = xxCreateTexture(g_device, 0, width, height);
    if (g_fontTexture == 0)
        return false;
    unsigned int stride = 0;
    void* map = xxMapTexture(g_fontTexture, stride);
    if (map == nullptr)
        return false;
    for (int y = 0; y < height; y++)
        memcpy((char*)map + stride * y, pixels + (width * bytes_per_pixel) * y, (width * bytes_per_pixel));
    xxUnmapTexture(g_fontTexture);

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
    return true;
}

void ImGui_ImplXX_InvalidateDeviceObjects()
{
    if (g_device == 0)
        return;
    for (unsigned int i = 0; i < 4; ++i)
    {
        xxDestroyBuffer(g_vertexBuffers[i]);
        xxDestroyBuffer(g_indexBuffers[i]);
        g_vertexBuffers[i] = 0;
        g_indexBuffers[i] = 0;
    }
    xxDestroyTexture(g_fontTexture);
    g_fontTexture = 0;
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
    xxBeginCommandBuffer(commandBuffer);
    xxBeginRenderPass(commandBuffer, data->RenderPass);

    ImGui_ImplXX_RenderDrawData(viewport->DrawData, commandBuffer);

    xxEndRenderPass(commandBuffer, data->RenderPass);
    xxEndCommandBuffer(commandBuffer);
    xxSubmitCommandBuffer(commandBuffer);
}

static void ImGui_ImplXX_SwapBuffers(ImGuiViewport* viewport, void*)
{
    ImGuiViewportDataXX* data = (ImGuiViewportDataXX*)viewport->RendererUserData;
    xxPresentSwapchain(data->Swapchain, data->Handle);
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
