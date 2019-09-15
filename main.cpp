// dear imgui: standalone example application
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <sys/stat.h>

#include "imgui/imgui.h"
#include "imgui/misc/freetype/imgui_freetype.h"

#include "implement/imgui_impl_xx.h"
#include "implement/imgui_impl_win32.h"

#include "graphic/xxGraphicD3D8.h"
#include "graphic/xxGraphicD3D8PS.h"
#include "graphic/xxGraphicD3D9.h"
#include "graphic/xxGraphicD3D9PS.h"
#include "graphic/xxGraphicD3D9Ex.h"
#include "graphic/xxGraphicD3D9On12.h"
#include "graphic/xxGraphicD3D10.h"
#include "graphic/xxGraphicD3D10_1.h"
#include "graphic/xxGraphicD3D11.h"
#include "graphic/xxGraphicD3D11On12.h"
#include "graphic/xxGraphicD3D12.h"
#include "graphic/xxGraphicGLES2.h"
#include "graphic/xxGraphicMantle.h"
#include "graphic/xxGraphicVulkan.h"
#include "graphic/xxGraphicNULL.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Forward declarations of helper functions
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Graphic data
static uint64_t g_instance = 0;
static uint64_t g_device = 0;
static uint64_t g_renderPass = 0;
static uint64_t g_swapchain = 0;

// Main code
int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    ImGui_ImplWin32_EnableDpiAwareness();
    float scale = ImGui_ImplWin32_GetDpiScaleForHwnd(nullptr);

    // Create application window
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), CS_OWNDC, WndProc, 0, 0, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL };
    ::RegisterClassExW(&wc);
    HWND hWnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui XX Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280 * scale, 800 * scale, NULL, NULL, wc.hInstance, NULL);

    g_instance = xxCreateInstanceD3D11();
    g_device = xxCreateDevice(g_instance);
    g_renderPass = xxCreateRenderPass(g_device, true, true, true, true, true, true);
    g_swapchain = xxCreateSwapchain(g_device, g_renderPass, hWnd, 0, 0);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    io.IniFilename = NULL;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    io.FontGlobalScale = scale;
    style.ScaleAllSizes(scale);

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplXX_Init(g_instance, 0, g_device, g_renderPass);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Load / Merge Fonts
    ImFontConfig font_config;
    font_config.SizePixels          = 13.0f * io.FontGlobalScale;
    font_config.RasterizerMultiply  = 1.0f;
    io.Fonts->AddFontDefault(&font_config);

    font_config.OversampleH         = 1;
    font_config.OversampleV         = 1;
    font_config.PixelSnapH          = true;
    font_config.MergeMode           = true;
#if defined(_MSC_VER)
    font_config.SizePixels          = 13.0f * io.FontGlobalScale;
    font_config.RasterizerMultiply  = 2.0f / io.FontGlobalScale;
    font_config.RasterizerFlags     = ImGuiFreeType::Bitmap;
    if (io.FontGlobalScale == 1.0f)
    {
        struct stat st;
        if (stat("C:\\Windows\\Fonts\\msgothic.ttc", &st) == 0)
            io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &font_config, io.Fonts->GetGlyphRangesJapanese());
        else if (stat("C:\\Windows\\Fonts\\mingliu.ttc", &st) == 0)
            io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\mingliu.ttc", 13.0f, &font_config, io.Fonts->GetGlyphRangesJapanese());
    }
    else
    {
        struct stat st;
        if (stat("C:\\Windows\\Fonts\\meiryo.ttc", &st) == 0)
            io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\meiryo.ttc", 13.0f * io.FontGlobalScale, &font_config, io.Fonts->GetGlyphRangesJapanese());
        else if (stat("C:\\Windows\\Fonts\\msjh.ttc", &st) == 0)
            io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msjh.ttc", 13.0f * io.FontGlobalScale, &font_config, io.Fonts->GetGlyphRangesJapanese());
    }
#endif
    io.FontGlobalScale = 1.0f;
    ImGuiFreeType::BuildFontAtlas(io.Fonts);

    // Show the window
    ::ShowWindow(hWnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hWnd);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    MSG msg = {};
    bool recreateWindow = false;
    while (msg.message != WM_QUIT || recreateWindow == true)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        if (::PeekMessageW(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
            continue;
        }

        // Recreate window when SwapEffect is changed.
        if (recreateWindow == true)
        {
            recreateWindow = false;
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplXX_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Graphic API
        uint64_t(*createInstance)() = nullptr;
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Graphic"))
            {
                const char* deviceStringCurrent = xxGetDeviceString(g_device);
                const char* deviceStringTarget = "";
                bool selected = false;

                xxLocalBreak()
                {
#define GRAPHIC(api) \
                    deviceStringTarget = xxGetDeviceString ## api(g_device); \
                    selected = (deviceStringCurrent == deviceStringTarget); \
                    if (ImGui::MenuItem(deviceStringTarget, nullptr, &selected)) \
                    { \
                        createInstance = xxCreateInstance ## api; \
                        break; \
                    }

#if defined(_M_IX86)
                    GRAPHIC(D3D8);
                    GRAPHIC(D3D8PS);
#endif
                    GRAPHIC(D3D9);
                    GRAPHIC(D3D9PS);
                    GRAPHIC(D3D9Ex);
                    GRAPHIC(D3D9ExPS);
                    GRAPHIC(D3D9On12);
                    GRAPHIC(D3D9On12PS);
                    GRAPHIC(D3D9On12Ex);
                    GRAPHIC(D3D9On12ExPS);
                    GRAPHIC(D3D10);
                    GRAPHIC(D3D10_1);
                    GRAPHIC(D3D11);
                    GRAPHIC(D3D11On12);
                    GRAPHIC(D3D12);
                    GRAPHIC(GLES2);
                    GRAPHIC(Mantle);
                    GRAPHIC(NULL);
                    GRAPHIC(Vulkan);
#undef GRAPHIC
                }

                if (createInstance != nullptr)
                {
                    bool flipCurrent =  strstr(deviceStringCurrent, "Ex") || \
                                        strstr(deviceStringCurrent, "10") || \
                                        strstr(deviceStringCurrent, "11") || \
                                        strstr(deviceStringCurrent, "12.");
                    bool flipTarget =   strstr(deviceStringTarget, "Ex") || \
                                        strstr(deviceStringTarget, "10") || \
                                        strstr(deviceStringTarget, "11") || \
                                        strstr(deviceStringTarget, "12.");
                    recreateWindow = (flipCurrent && flipTarget == false);
                }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::EndFrame();
        ImGui::Render();

        uint64_t commandBuffer = xxGetCommandBuffer(g_device, g_swapchain);
        uint64_t framebuffer = xxGetFramebuffer(g_device, g_swapchain);
        xxBeginCommandBuffer(commandBuffer);
        xxBeginRenderPass(commandBuffer, framebuffer, g_renderPass, clear_color.x, clear_color.y, clear_color.z, clear_color.w, 1.0f, 0);

        ImGui_ImplXX_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

        xxEndRenderPass(commandBuffer, framebuffer, g_renderPass);
        xxEndCommandBuffer(commandBuffer);
        xxSubmitCommandBuffer(commandBuffer, g_swapchain);

        xxPresentSwapchain(g_swapchain);

        if (xxTestDevice(g_device) == false)
        {
            ImGui_ImplXX_InvalidateDeviceObjects();
            xxDestroySwapchain(g_swapchain);
            xxResetDevice(g_device);
            g_swapchain = xxCreateSwapchain(g_device, g_renderPass, hWnd, 0, 0);
            ImGui_ImplXX_CreateDeviceObjects();
        }

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        // Recreate Graphic API
        if (createInstance != nullptr)
        {
            ImGui_ImplXX_Shutdown();
            ImGui_ImplWin32_Shutdown();
            xxDestroySwapchain(g_swapchain);
            xxDestroyRenderPass(g_renderPass);
            xxDestroyDevice(g_device);
            xxDestroyInstance(g_instance);
            g_swapchain = 0;
            g_renderPass = 0;
            g_device = 0;
            g_instance = 0;
            if (recreateWindow)
            {
                ::DestroyWindow(hWnd);
                hWnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui XX Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280 * scale, 800 * scale, NULL, NULL, wc.hInstance, NULL);
                ::ShowWindow(hWnd, SW_SHOWDEFAULT);
                ::UpdateWindow(hWnd);
            }
            g_instance = createInstance();
            g_device = xxCreateDevice(g_instance);
            g_renderPass = xxCreateRenderPass(g_device, true, true, true, true, true, true);
            g_swapchain = xxCreateSwapchain(g_device, g_renderPass, hWnd, 0, 0);
            ImGui_ImplWin32_Init(hWnd);
            ImGui_ImplXX_Init(g_instance, 0, g_device, g_renderPass);
        }
    }

    ImGui_ImplXX_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    xxDestroySwapchain(g_swapchain);
    xxDestroyRenderPass(g_renderPass);
    xxDestroyDevice(g_device);
    xxDestroyInstance(g_instance);

    ::DestroyWindow(hWnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

// Win32 message handler
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED && g_swapchain)
        {
            xxDestroySwapchain(g_swapchain);
            g_swapchain = 0;
            g_swapchain = xxCreateSwapchain(g_device, g_renderPass, hWnd, LOWORD(lParam), HIWORD(lParam));
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            //const int dpi = HIWORD(wParam);
            //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
            const RECT* suggested_rect = (RECT*)lParam;
            ::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        break;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
