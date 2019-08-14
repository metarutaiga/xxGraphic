// dear imgui: standalone example application for DirectX 9
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <sys/stat.h>

#include "imgui/imgui.h"
#include "imgui/misc/freetype/imgui_freetype.h"
#include "graphic/imgui_impl_dx9.h"
#include "graphic/imgui_impl_dx10.h"
#include "graphic/imgui_impl_dx11.h"
#include "graphic/imgui_impl_win32.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

bool (*ImGui_ImplDraw_Init)(void* device, void* device_context);
void (*ImGui_ImplDraw_Shutdown)();
void (*ImGui_ImplDraw_NewFrame)();
void (*ImGui_ImplDraw_RenderDrawData)(ImDrawData* draw_data);
bool (*ImGui_ImplDraw_Create)(void* hWnd);
bool (*ImGui_ImplDraw_Cleanup)();
bool (*ImGui_ImplDraw_Reset)(int width, int height);
bool (*ImGui_ImplDraw_Begin)();
bool (*ImGui_ImplDraw_Clear)(const ImVec4& clear_color);
bool (*ImGui_ImplDraw_Finish)();
bool (*ImGui_ImplDraw_Present)();

// Forward declarations of helper functions
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int main(int, char**)
{
    ImGui_ImplWin32_EnableDpiAwareness();
    float scale = ImGui_ImplWin32_GetDpiScaleForHwnd(nullptr);

    // Create application window
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280 * scale, 800 * scale, NULL, NULL, wc.hInstance, NULL);

    int api = 11;
    switch (api)
    {
    case 9:
        ImGui_ImplDraw_Init           = (bool (*)(void*, void*))ImGui_ImplDX9_Init;
        ImGui_ImplDraw_Shutdown       = ImGui_ImplDX9_Shutdown;
        ImGui_ImplDraw_NewFrame       = ImGui_ImplDX9_NewFrame;
        ImGui_ImplDraw_RenderDrawData = ImGui_ImplDX9_RenderDrawData;
        ImGui_ImplDraw_Create         = ImGui_ImplDX9_Create;
        ImGui_ImplDraw_Cleanup        = ImGui_ImplDX9_Cleanup;
        ImGui_ImplDraw_Reset          = ImGui_ImplDX9_Reset;
        ImGui_ImplDraw_Begin          = ImGui_ImplDX9_Begin;
        ImGui_ImplDraw_Clear          = ImGui_ImplDX9_Clear;
        ImGui_ImplDraw_Finish         = ImGui_ImplDX9_Finish;
        ImGui_ImplDraw_Present        = ImGui_ImplDX9_Present;
        break;

    case 10:
        ImGui_ImplDraw_Init           = (bool (*)(void*, void*))ImGui_ImplDX10_Init;
        ImGui_ImplDraw_Shutdown       = ImGui_ImplDX10_Shutdown;
        ImGui_ImplDraw_NewFrame       = ImGui_ImplDX10_NewFrame;
        ImGui_ImplDraw_RenderDrawData = ImGui_ImplDX10_RenderDrawData;
        ImGui_ImplDraw_Create         = ImGui_ImplDX10_Create;
        ImGui_ImplDraw_Cleanup        = ImGui_ImplDX10_Cleanup;
        ImGui_ImplDraw_Reset          = ImGui_ImplDX10_Reset;
        ImGui_ImplDraw_Begin          = ImGui_ImplDX10_Begin;
        ImGui_ImplDraw_Clear          = ImGui_ImplDX10_Clear;
        ImGui_ImplDraw_Finish         = ImGui_ImplDX10_Finish;
        ImGui_ImplDraw_Present        = ImGui_ImplDX10_Present;
        break;

    case 11:
        ImGui_ImplDraw_Init           = (bool (*)(void*, void*))ImGui_ImplDX11_Init;
        ImGui_ImplDraw_Shutdown       = ImGui_ImplDX11_Shutdown;
        ImGui_ImplDraw_NewFrame       = ImGui_ImplDX11_NewFrame;
        ImGui_ImplDraw_RenderDrawData = ImGui_ImplDX11_RenderDrawData;
        ImGui_ImplDraw_Create         = ImGui_ImplDX11_Create;
        ImGui_ImplDraw_Cleanup        = ImGui_ImplDX11_Cleanup;
        ImGui_ImplDraw_Reset          = ImGui_ImplDX11_Reset;
        ImGui_ImplDraw_Begin          = ImGui_ImplDX11_Begin;
        ImGui_ImplDraw_Clear          = ImGui_ImplDX11_Clear;
        ImGui_ImplDraw_Finish         = ImGui_ImplDX11_Finish;
        ImGui_ImplDraw_Present        = ImGui_ImplDX11_Present;
        break;
    }

    // Initialize Direct3D
    if (!ImGui_ImplDraw_Create(hwnd))
    {
        ImGui_ImplDraw_Cleanup();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

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

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDraw_Init(NULL, NULL);

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
        if (stat("c:\\Windows\\Fonts\\msgothic.ttc", &st) == 0)
            io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f * io.FontGlobalScale, &font_config, io.Fonts->GetGlyphRangesJapanese());
        else if (stat("c:\\Windows\\Fonts\\mingliu.ttc", &st) == 0)
            io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\mingliu.ttc", 13.0f * io.FontGlobalScale, &font_config, io.Fonts->GetGlyphRangesJapanese());
    }
    else
    {
        struct stat st;
        if (stat("c:\\Windows\\Fonts\\meiryo.ttc", &st) == 0)
            io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 13.0f * io.FontGlobalScale, &font_config, io.Fonts->GetGlyphRangesJapanese());
        else if (stat("c:\\Windows\\Fonts\\msjh.ttc", &st) == 0)
            io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msjh.ttc", 13.0f * io.FontGlobalScale, &font_config, io.Fonts->GetGlyphRangesJapanese());
    }
#endif
    io.FontGlobalScale = 1.0f;
    ImGuiFreeType::BuildFontAtlas(io.Fonts);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
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

        // Start the Dear ImGui frame
        ImGui_ImplDraw_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

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
        if (ImGui_ImplDraw_Begin())
        {
            ImGui_ImplDraw_Clear(clear_color);
            ImGui::Render();
            ImGui_ImplDraw_RenderDrawData(ImGui::GetDrawData());
            ImGui_ImplDraw_Finish();
        }

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        ImGui_ImplDraw_Present();
    }

    ImGui_ImplDraw_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    ImGui_ImplDraw_Cleanup();
    ::DestroyWindow(hwnd);
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
        if (wParam != SIZE_MINIMIZED)
        {
            ImGui_ImplDraw_Reset(LOWORD(lParam), HIWORD(lParam));
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
