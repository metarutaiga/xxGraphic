// dear imgui: standalone example application for OSX + OpenGL2, using legacy fixed pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <sys/stat.h>

#include "imgui/imgui.h"
#include "imgui/misc/freetype/imgui_freetype.h"

#include "implement/imgui_impl_osx.h"
#include "implement/imgui_impl_xx.h"

#include "graphic/xxGraphicGLES2.h"
#include "graphic/xxGraphicNULL.h"

#include <stdio.h>
#import <Cocoa/Cocoa.h>

// Graphic data
static uint64_t g_instance = 0;
static uint64_t g_device = 0;
static uint64_t g_swapchain = 0;
static uint64_t g_renderPass = 0;

//-----------------------------------------------------------------------------------
// ImGuiExampleView
//-----------------------------------------------------------------------------------

@interface ImGuiExampleView : NSOpenGLView
{
    NSTimer*    animationTimer;
}
@end

@implementation ImGuiExampleView

-(void)animationTimerFired:(NSTimer*)timer
{
    [self setNeedsDisplay:YES];
}

-(void)updateAndDrawDemoView
{
    // Start the Dear ImGui frame
    ImGui_ImplXX_NewFrame();
    ImGui_ImplOSX_NewFrame(self);
    ImGui::NewFrame();

    // Graphic API
    uint64_t (*createInstance)() = nullptr;
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

                GRAPHIC(GLES2);
                GRAPHIC(NULL);
#undef GRAPHIC
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // Global data for the demo
    static bool show_demo_window = true;
    static bool show_another_window = false;
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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
    xxBeginRenderPass(commandBuffer, framebuffer, g_renderPass);

    ImGui_ImplXX_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

    xxEndRenderPass(commandBuffer, framebuffer, g_renderPass);
    xxEndCommandBuffer(commandBuffer);
    xxSubmitCommandBuffer(commandBuffer);

    xxPresentSwapchain(g_swapchain);

    if (xxTestDevice(g_device) == false)
    {
        ImGui_ImplXX_InvalidateDeviceObjects();
        xxDestroySwapchain(g_swapchain);
        xxResetDevice(g_device);
        g_swapchain = xxCreateSwapchain(g_device, (__bridge void*)self.window, 0, 0);
        ImGui_ImplXX_CreateDeviceObjects();
    }

    // Update and Render additional Platform Windows
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    if (!animationTimer)
        animationTimer = [NSTimer scheduledTimerWithTimeInterval:0 target:self selector:@selector(animationTimerFired:) userInfo:nil repeats:YES];
}

-(void)reshape
{
    [super reshape];
    if (g_swapchain)
    {
        xxDestroySwapchain(g_swapchain);
        g_swapchain = 0;
        g_swapchain = xxCreateSwapchain(g_device, (__bridge void*)[self window], 0, 0);
    }
}

-(void)drawRect:(NSRect)bounds
{
    [self updateAndDrawDemoView];
}

-(BOOL)acceptsFirstResponder
{
    return (YES);
}

-(BOOL)becomeFirstResponder
{
    return (YES);
}

-(BOOL)resignFirstResponder
{
    return (YES);
}

-(void)dealloc
{
    animationTimer = nil;
}

// Forward Mouse/Keyboard events to dear imgui OSX back-end. It returns true when imgui is expecting to use the event.
-(void)keyUp:(NSEvent *)event           { ImGui_ImplOSX_HandleEvent(event, self); }
-(void)keyDown:(NSEvent *)event         { ImGui_ImplOSX_HandleEvent(event, self); }
-(void)flagsChanged:(NSEvent *)event    { ImGui_ImplOSX_HandleEvent(event, self); }
-(void)mouseDown:(NSEvent *)event       { ImGui_ImplOSX_HandleEvent(event, self); }
-(void)mouseUp:(NSEvent *)event         { ImGui_ImplOSX_HandleEvent(event, self); }
-(void)mouseMoved:(NSEvent *)event      { ImGui_ImplOSX_HandleEvent(event, self); }
-(void)mouseDragged:(NSEvent *)event    { ImGui_ImplOSX_HandleEvent(event, self); }
-(void)scrollWheel:(NSEvent *)event     { ImGui_ImplOSX_HandleEvent(event, self); }

@end

//-----------------------------------------------------------------------------------
// ImGuiExampleAppDelegate
//-----------------------------------------------------------------------------------

@interface ImGuiExampleAppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@property (nonatomic, readonly) NSWindow* window;
@end

@implementation ImGuiExampleAppDelegate
@synthesize window = _window;

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
    return YES;
}

-(NSWindow*)window
{
    if (_window != nil)
        return (_window);

    NSRect viewRect = NSMakeRect(100.0, 100.0, 100.0 + 1280.0, 100 + 720.0);

    _window = [[NSWindow alloc] initWithContentRect:viewRect styleMask:NSWindowStyleMaskTitled|NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskResizable|NSWindowStyleMaskClosable backing:NSBackingStoreBuffered defer:YES];
    [_window setTitle:@"Dear ImGui XX Example"];
    [_window setAcceptsMouseMovedEvents:YES];
    [_window setOpaque:YES];
    [_window makeKeyAndOrderFront:NSApp];
    [_window setDelegate:self];

    return (_window);
}

-(void)windowWillClose:(NSNotification *)notification
{
    [NSApp terminate:self];
}

-(void)setupMenu
{
	NSMenu* mainMenuBar = [[NSMenu alloc] init];
    NSMenu* appMenu;
    NSMenuItem* menuItem;

    appMenu = [[NSMenu alloc] initWithTitle:@"Dear ImGui XX Example"];
    menuItem = [appMenu addItemWithTitle:@"Quit Dear ImGui XX Example" action:@selector(terminate:) keyEquivalent:@"q"];
    [menuItem setKeyEquivalentModifierMask:NSEventModifierFlagCommand];

    menuItem = [[NSMenuItem alloc] init];
    [menuItem setSubmenu:appMenu];

    [mainMenuBar addItem:menuItem];

    appMenu = nil;
    [NSApp setMainMenu:mainMenuBar];
}

-(void)dealloc
{
    _window = nil;
}

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    float scale = [self.window backingScaleFactor];

    // Make the application a foreground application (else it won't receive keyboard events)
    ProcessSerialNumber psn = {0, kCurrentProcess};
    TransformProcessType(&psn, kProcessTransformToForegroundApplication);

    // Menu
    [self setupMenu];

    ImGuiExampleView* view = [[ImGuiExampleView alloc] initWithFrame:self.window.frame];
#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1070
    if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6)
        [view setWantsBestResolutionOpenGLSurface:YES];
#endif // MAC_OS_X_VERSION_MAX_ALLOWED >= 1070
    [self.window setContentView:view];

    g_instance = xxCreateInstanceGLES2();
    g_device = xxCreateDevice(g_instance);
    g_swapchain = xxCreateSwapchain(g_device, (__bridge void*)self.window, 0, 0);
    g_renderPass = xxCreateRenderPass(g_device, 0.45f, 0.55f, 0.60f, 1.0f, 1.0f, 0);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
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
    ImGui_ImplOSX_Init(self.window);
    ImGui_ImplXX_Init(g_instance, 0, g_device);

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
#if defined(xxMACOS)
    font_config.SizePixels          = 13.0f * io.FontGlobalScale;
    font_config.RasterizerMultiply  = 2.0f / io.FontGlobalScale;
    font_config.RasterizerFlags     = ImGuiFreeType::Bitmap;
    io.Fonts->AddFontFromFileTTF("/System/Library/Fonts/PingFang.ttc", 16.0f * io.FontGlobalScale, &font_config, io.Fonts->GetGlyphRangesJapanese());
#endif
    io.FontGlobalScale              = 1.0f;
    ImGuiFreeType::BuildFontAtlas(io.Fonts);
}

-(void)applicationWillTerminate:(NSNotification *)notification;
{
    ImGui_ImplXX_Shutdown();
    ImGui_ImplOSX_Shutdown();
    ImGui::DestroyContext();
    
    xxDestroyRenderPass(g_renderPass);
    xxDestroySwapchain(g_swapchain);
    xxDestroyDevice(g_device);
    xxDestroyInstance(g_instance);
}

@end

int main(int argc, const char* argv[])
{
	@autoreleasepool
	{
		NSApp = [NSApplication sharedApplication];
		ImGuiExampleAppDelegate* delegate = [[ImGuiExampleAppDelegate alloc] init];
		[[NSApplication sharedApplication] setDelegate:delegate];
		[NSApp run];
	}
	return NSApplicationMain(argc, argv);
}
