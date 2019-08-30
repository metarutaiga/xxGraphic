#include "xxGraphicGL.h"

static const wchar_t* const g_dummy = L"xxGraphicDummyWindow";
static HMODULE              g_glLibrary = nullptr;
static HWND                 g_hWnd = nullptr;

static HGLRC                (WINAPI *wglCreateContext)(HDC);
static BOOL                 (WINAPI *wglDeleteContext)(HGLRC);
static HGLRC                (WINAPI *wglGetCurrentContext)(VOID);
static HDC                  (WINAPI *wglGetCurrentDC)(VOID);
static PROC                 (WINAPI *wglGetProcAddress)(LPCSTR);
static BOOL                 (WINAPI *wglMakeCurrent)(HDC, HGLRC);
static BOOL                 (WINAPI *wglShareLists)(HGLRC, HGLRC);
static HGLRC                (WINAPI *wglCreateContextAttribsARB)(HDC, HGLRC, const int*);
static BOOL                 (WINAPI *wglChoosePixelFormatARB)(HDC, const int*, const FLOAT*, UINT, int*, UINT*);
#define WGL_CONTEXT_MAJOR_VERSION_ARB               0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB               0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB                0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB            0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB   0x00000002
#define WGL_CONTEXT_ES2_PROFILE_BIT_EXT             0x00000004
#define WGL_DRAW_TO_WINDOW_ARB                      0x2001
#define WGL_ACCELERATION_ARB                        0x2003
#define WGL_SUPPORT_OPENGL_ARB                      0x2010
#define WGL_DOUBLE_BUFFER_ARB                       0x2011
#define WGL_PIXEL_TYPE_ARB                          0x2013
#define WGL_COLOR_BITS_ARB                          0x2014
#define WGL_DEPTH_BITS_ARB                          0x2022
#define WGL_STENCIL_BITS_ARB                        0x2023
#define WGL_FULL_ACCELERATION_ARB                   0x2027
#define WGL_TYPE_RGBA_ARB                           0x202B

//==============================================================================
//  Initialize - WGL
//==============================================================================
static void* getWGLSymbol(const char* name, bool& failed)
{
    void* ptr = nullptr;

    if (ptr == nullptr && wglGetProcAddress)
        ptr = wglGetProcAddress(name);

    if (ptr == nullptr && g_glLibrary)
        ptr = GetProcAddress(g_glLibrary, name);

    if (ptr == nullptr)
        xxLog("WGL", "%s is not found", name);

    failed |= (ptr == nullptr);
    return ptr;
}
//------------------------------------------------------------------------------
uint64_t xxglCreateContextWGL(uint64_t instance, void* view, void** display)
{
    HGLRC hInstance = reinterpret_cast<HGLRC>(instance);
    HWND hWnd = reinterpret_cast<HWND>(view);
    HDC hDC = GetDC(hWnd);

    PIXELFORMATDESCRIPTOR desc = {};
    desc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    desc.nVersion = 1;
    desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    desc.iPixelType = PFD_TYPE_RGBA;
    desc.cColorBits = 32;
    desc.cDepthBits = 24;
    desc.cStencilBits = 8;
    desc.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat;
    if (wglChoosePixelFormatARB)
    {
        int pixelFormatAttribs[] =
        {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
            WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
            WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB,     32,
            WGL_DEPTH_BITS_ARB,     24,
            WGL_STENCIL_BITS_ARB,   8,
            0
        };

        UINT numFormats;
        wglChoosePixelFormatARB(hDC, pixelFormatAttribs, 0, 1, &pixelFormat, &numFormats);
        DescribePixelFormat(hDC, pixelFormat, sizeof(desc), &desc);
    }
    else
    {
        pixelFormat = ChoosePixelFormat(hDC, &desc);
    }
    SetPixelFormat(hDC, pixelFormat, &desc);

    HGLRC hGLRC;
    if (wglChoosePixelFormatARB)
    {
        int attribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 2,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0,
        };

        hGLRC = wglCreateContextAttribsARB(hDC, hInstance, attribs);
        if (hGLRC == nullptr)
        {
            ReleaseDC(hWnd, hDC);
            return 0;
        }
    }
    else
    {
        hGLRC = wglCreateContext(hDC);
        if (hGLRC == nullptr)
        {
            ReleaseDC(hWnd, hDC);
            return 0;
        }

        if (hInstance)
        {
            if (wglShareLists(hInstance, hGLRC) == FALSE)
            {
                xxLog("xxGraphic", "%s is failed", "wglShareLists");
            }
        }
    }
    if (wglMakeCurrent(hDC, hGLRC) == FALSE)
    {
        xxLog("xxGraphic", "%s is failed", "wglMakeCurrent");
    }

    if (display)
    {
        (*display) = hDC;
    }
    else
    {
        ReleaseDC(hWnd, hDC);
    }

    return reinterpret_cast<uint64_t>(hGLRC);
}
//------------------------------------------------------------------------------
void xxglDestroyContextWGL(uint64_t context, void* view, void* display)
{
    HGLRC hGLRC = reinterpret_cast<HGLRC>(context);
    if (hGLRC == nullptr)
        return;
    HWND hWnd = reinterpret_cast<HWND>(view);
    HDC hDC = reinterpret_cast<HDC>(display);

    wglMakeCurrent(hDC, nullptr);
    wglDeleteContext(hGLRC);
    ReleaseDC(hWnd, hDC);
}
//------------------------------------------------------------------------------
void xxglMakeCurrentContextWGL(uint64_t context, void* display)
{
    HGLRC hGLRC = reinterpret_cast<HGLRC>(context);
    HDC hDC = reinterpret_cast<HDC>(display);

    if (wglMakeCurrent(hDC, hGLRC) == FALSE)
    {
        xxLog("xxGraphic", "%s is failed", "wglMakeCurrent");
    }
}
//------------------------------------------------------------------------------
void xxglPresentContextWGL(uint64_t context, void* display)
{
    HDC hDC = reinterpret_cast<HDC>(display);

    if (SwapBuffers(hDC) == FALSE)
    {
        xxLog("xxGraphic", "%s is failed", "SwapBuffers");
    }
}
//------------------------------------------------------------------------------
uint64_t xxGraphicCreateWGL()
{
    if (g_glLibrary == nullptr)
        g_glLibrary = LoadLibraryW(L"opengl32.dll");
    if (g_glLibrary == nullptr)
        return 0;

    if (g_hWnd == nullptr)
    {
        WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), CS_OWNDC, DefWindowProcW, 0, 0, GetModuleHandleW(nullptr), nullptr, nullptr, nullptr, nullptr, g_dummy, nullptr };
        RegisterClassExW(&wc);
        g_hWnd = CreateWindowW(g_dummy, g_dummy, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);
    }

    bool failed = false;

#define symbol(var) (void*&)var = getWGLSymbol(#var, failed);
    failed = false;
    symbol(wglCreateContext);
    symbol(wglDeleteContext);
    symbol(wglGetCurrentContext);
    symbol(wglGetCurrentDC);
    symbol(wglGetProcAddress);
    symbol(wglMakeCurrent);
    symbol(wglShareLists);
#undef symbol
    if (failed)
        return 0;

    void* display = nullptr;
    uint64_t context = xxglCreateContextWGL(0, g_hWnd, &display);
    if (context == 0)
        return 0;

#define symbol(var) (void*&)var = getWGLSymbol(#var, failed);
    failed = false;
    symbol(wglCreateContextAttribsARB);
    symbol(wglChoosePixelFormatARB);
#undef symbol
    if (failed == false)
    {
        xxglDestroyContextWGL(context, g_hWnd, display);
        context = xxglCreateContextWGL(0, g_hWnd, &display);
    }

    if (xxGraphicCreateGL(getWGLSymbol) == false)
    {
        xxGraphicDestroyWGL(context, g_hWnd, display);
        return 0;
    }

    xxglCreateContext = xxglCreateContextWGL;
    xxglDestroyContext = xxglDestroyContextWGL;
    xxglMakeCurrentContext = xxglMakeCurrentContextWGL;
    xxglPresentContext = xxglPresentContextWGL;

    return context;
}
//------------------------------------------------------------------------------
void xxGraphicDestroyWGL(uint64_t context, void* view, void* display)
{
    xxglDestroyContextWGL(context, view, display);
    xxGraphicDestroyGL();

    if (g_hWnd)
    {
        DestroyWindow(g_hWnd);
        UnregisterClassW(g_dummy, GetModuleHandleW(nullptr));
        g_hWnd = nullptr;
    }
}
//==============================================================================
//  Initialize
//==============================================================================
bool xxGraphicCreateGL(void* (*getSymbol)(const char* name, bool& failed))
{
    bool failed = false;

#define symbol(var) (void*&)var = getSymbol(#var, failed);
    failed = false;
    symbol(glActiveTexture);
    symbol(glAttachShader);
    symbol(glBindAttribLocation);
    symbol(glBindBuffer);
    symbol(glBindFramebuffer);
    symbol(glBindRenderbuffer);
    symbol(glBindTexture);
    symbol(glBlendColor);
    symbol(glBlendEquation);
    symbol(glBlendEquationSeparate);
    symbol(glBlendFunc);
    symbol(glBlendFuncSeparate);
    symbol(glBufferData);
    symbol(glBufferSubData);
    symbol(glCheckFramebufferStatus);
    symbol(glClear);
    symbol(glClearColor);
    symbol(glClearDepthf);
    symbol(glClearStencil);
    symbol(glColorMask);
    symbol(glCompileShader);
    symbol(glCompressedTexImage2D);
    symbol(glCompressedTexSubImage2D);
    symbol(glCopyTexImage2D);
    symbol(glCopyTexSubImage2D);
    symbol(glCreateProgram);
    symbol(glCreateShader);
    symbol(glCullFace);
    symbol(glDeleteBuffers);
    symbol(glDeleteFramebuffers);
    symbol(glDeleteProgram);
    symbol(glDeleteRenderbuffers);
    symbol(glDeleteShader);
    symbol(glDeleteTextures);
    symbol(glDepthFunc);
    symbol(glDepthMask);
    symbol(glDepthRangef);
    symbol(glDetachShader);
    symbol(glDisable);
    symbol(glDisableVertexAttribArray);
    symbol(glDrawArrays);
    symbol(glDrawElements);
    symbol(glEnable);
    symbol(glEnableVertexAttribArray);
    symbol(glFinish);
    symbol(glFlush);
    symbol(glFramebufferRenderbuffer);
    symbol(glFramebufferTexture2D);
    symbol(glFrontFace);
    symbol(glGenBuffers);
    symbol(glGenerateMipmap);
    symbol(glGenFramebuffers);
    symbol(glGenRenderbuffers);
    symbol(glGenTextures);
    symbol(glGetActiveAttrib);
    symbol(glGetActiveUniform);
    symbol(glGetAttachedShaders);
    symbol(glGetAttribLocation);
    symbol(glGetBooleanv);
    symbol(glGetBufferParameteriv);
    symbol(glGetError);
    symbol(glGetFloatv);
    symbol(glGetFramebufferAttachmentParameteriv);
    symbol(glGetIntegerv);
    symbol(glGetProgramiv);
    symbol(glGetProgramInfoLog);
    symbol(glGetRenderbufferParameteriv);
    symbol(glGetShaderiv);
    symbol(glGetShaderInfoLog);
    symbol(glGetShaderPrecisionFormat);
    symbol(glGetShaderSource);
    symbol(glGetString);
    symbol(glGetTexParameterfv);
    symbol(glGetTexParameteriv);
    symbol(glGetUniformfv);
    symbol(glGetUniformiv);
    symbol(glGetUniformLocation);
    symbol(glGetVertexAttribfv);
    symbol(glGetVertexAttribiv);
    symbol(glGetVertexAttribPointerv);
    symbol(glHint);
    symbol(glIsBuffer);
    symbol(glIsEnabled);
    symbol(glIsFramebuffer);
    symbol(glIsProgram);
    symbol(glIsRenderbuffer);
    symbol(glIsShader);
    symbol(glIsTexture);
    symbol(glLineWidth);
    symbol(glLinkProgram);
    symbol(glPixelStorei);
    symbol(glPolygonOffset);
    symbol(glReadPixels);
    symbol(glReleaseShaderCompiler);
    symbol(glRenderbufferStorage);
    symbol(glSampleCoverage);
    symbol(glScissor);
    symbol(glShaderBinary);
    symbol(glShaderSource);
    symbol(glStencilFunc);
    symbol(glStencilFuncSeparate);
    symbol(glStencilMask);
    symbol(glStencilMaskSeparate);
    symbol(glStencilOp);
    symbol(glStencilOpSeparate);
    symbol(glTexImage2D);
    symbol(glTexParameterf);
    symbol(glTexParameterfv);
    symbol(glTexParameteri);
    symbol(glTexParameteriv);
    symbol(glTexSubImage2D);
    symbol(glUniform1f);
    symbol(glUniform1fv);
    symbol(glUniform1i);
    symbol(glUniform1iv);
    symbol(glUniform2f);
    symbol(glUniform2fv);
    symbol(glUniform2i);
    symbol(glUniform2iv);
    symbol(glUniform3f);
    symbol(glUniform3fv);
    symbol(glUniform3i);
    symbol(glUniform3iv);
    symbol(glUniform4f);
    symbol(glUniform4fv);
    symbol(glUniform4i);
    symbol(glUniform4iv);
    symbol(glUniformMatrix2fv);
    symbol(glUniformMatrix3fv);
    symbol(glUniformMatrix4fv);
    symbol(glUseProgram);
    symbol(glValidateProgram);
    symbol(glVertexAttrib1f);
    symbol(glVertexAttrib1fv);
    symbol(glVertexAttrib2f);
    symbol(glVertexAttrib2fv);
    symbol(glVertexAttrib3f);
    symbol(glVertexAttrib3fv);
    symbol(glVertexAttrib4f);
    symbol(glVertexAttrib4fv);
    symbol(glVertexAttribPointer);
    symbol(glViewport);

    symbol(glDrawElementsBaseVertex);

    symbol(glGenVertexArrays);
    symbol(glDeleteVertexArrays);
    symbol(glBindVertexArray);
#undef symbol

    return (failed == false);
}
//------------------------------------------------------------------------------
static void* getNULLSymbol(const char* name, bool& failed)
{
    return nullptr;
}
//------------------------------------------------------------------------------
void xxGraphicDestroyGL()
{
    xxGraphicCreateGL(getNULLSymbol);
}
//==============================================================================
//  GLSL Shader
//==============================================================================
const char* vertexShaderCode = 
    "uniform vec4 uniformBuffer[4];\n"
    "attribute vec3 position;\n"
    "attribute vec4 color;\n"
    "attribute vec2 uv;\n"
    "varying vec2 varyUV;\n"
    "varying vec4 varyColor;\n"
    "void main()\n"
    "{\n"
    "    vec4 projection;\n"
    "    projection = uniformBuffer[0] * position.x;\n"
    "    projection += uniformBuffer[1] * position.y;\n"
    "    projection += uniformBuffer[2] * position.z;\n"
    "    projection += uniformBuffer[3];\n"
    "    gl_Position = projection;\n"
    "    varyUV = uv;\n"
    "    varyColor = color;\n"
    "}\n";
//------------------------------------------------------------------------------
const char* fragmentShaderCode =
    "uniform sampler2D tex;\n"
    "varying vec2 varyUV;\n"
    "varying vec4 varyColor;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = varyColor * texture2D(tex, varyUV);\n"
    "}\n";
//==============================================================================
//  Function
//==============================================================================
uint64_t                                        (*xxglCreateContext)(uint64_t instance, void* view, void** display);
void                                            (*xxglDestroyContext)(uint64_t context, void* view, void* display);
void                                            (*xxglMakeCurrentContext)(uint64_t context, void* display);
void                                            (*xxglPresentContext)(uint64_t context, void* display);
//------------------------------------------------------------------------------
PFNGLACTIVETEXTUREPROC                          glActiveTexture;
PFNGLATTACHSHADERPROC                           glAttachShader;
PFNGLBINDATTRIBLOCATIONPROC                     glBindAttribLocation;
PFNGLBINDBUFFERPROC                             glBindBuffer;
PFNGLBINDFRAMEBUFFERPROC                        glBindFramebuffer;
PFNGLBINDRENDERBUFFERPROC                       glBindRenderbuffer;
PFNGLBINDTEXTUREPROC                            glBindTexture;
PFNGLBLENDCOLORPROC                             glBlendColor;
PFNGLBLENDEQUATIONPROC                          glBlendEquation;
PFNGLBLENDEQUATIONSEPARATEPROC                  glBlendEquationSeparate;
PFNGLBLENDFUNCPROC                              glBlendFunc;
PFNGLBLENDFUNCSEPARATEPROC                      glBlendFuncSeparate;
PFNGLBUFFERDATAPROC                             glBufferData;
PFNGLBUFFERSUBDATAPROC                          glBufferSubData;
PFNGLCHECKFRAMEBUFFERSTATUSPROC                 glCheckFramebufferStatus;
PFNGLCLEARPROC                                  glClear;
PFNGLCLEARCOLORPROC                             glClearColor;
PFNGLCLEARDEPTHFPROC                            glClearDepthf;
PFNGLCLEARSTENCILPROC                           glClearStencil;
PFNGLCOLORMASKPROC                              glColorMask;
PFNGLCOMPILESHADERPROC                          glCompileShader;
PFNGLCOMPRESSEDTEXIMAGE2DPROC                   glCompressedTexImage2D;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC                glCompressedTexSubImage2D;
PFNGLCOPYTEXIMAGE2DPROC                         glCopyTexImage2D;
PFNGLCOPYTEXSUBIMAGE2DPROC                      glCopyTexSubImage2D;
PFNGLCREATEPROGRAMPROC                          glCreateProgram;
PFNGLCREATESHADERPROC                           glCreateShader;
PFNGLCULLFACEPROC                               glCullFace;
PFNGLDELETEBUFFERSPROC                          glDeleteBuffers;
PFNGLDELETEFRAMEBUFFERSPROC                     glDeleteFramebuffers;
PFNGLDELETEPROGRAMPROC                          glDeleteProgram;
PFNGLDELETERENDERBUFFERSPROC                    glDeleteRenderbuffers;
PFNGLDELETESHADERPROC                           glDeleteShader;
PFNGLDELETETEXTURESPROC                         glDeleteTextures;
PFNGLDEPTHFUNCPROC                              glDepthFunc;
PFNGLDEPTHMASKPROC                              glDepthMask;
PFNGLDEPTHRANGEFPROC                            glDepthRangef;
PFNGLDETACHSHADERPROC                           glDetachShader;
PFNGLDISABLEPROC                                glDisable;
PFNGLDISABLEVERTEXATTRIBARRAYPROC               glDisableVertexAttribArray;
PFNGLDRAWARRAYSPROC                             glDrawArrays;
PFNGLDRAWELEMENTSPROC                           glDrawElements;
PFNGLENABLEPROC                                 glEnable;
PFNGLENABLEVERTEXATTRIBARRAYPROC                glEnableVertexAttribArray;
PFNGLFINISHPROC                                 glFinish;
PFNGLFLUSHPROC                                  glFlush;
PFNGLFRAMEBUFFERRENDERBUFFERPROC                glFramebufferRenderbuffer;
PFNGLFRAMEBUFFERTEXTURE2DPROC                   glFramebufferTexture2D;
PFNGLFRONTFACEPROC                              glFrontFace;
PFNGLGENBUFFERSPROC                             glGenBuffers;
PFNGLGENERATEMIPMAPPROC                         glGenerateMipmap;
PFNGLGENFRAMEBUFFERSPROC                        glGenFramebuffers;
PFNGLGENRENDERBUFFERSPROC                       glGenRenderbuffers;
PFNGLGENTEXTURESPROC                            glGenTextures;
PFNGLGETACTIVEATTRIBPROC                        glGetActiveAttrib;
PFNGLGETACTIVEUNIFORMPROC                       glGetActiveUniform;
PFNGLGETATTACHEDSHADERSPROC                     glGetAttachedShaders;
PFNGLGETATTRIBLOCATIONPROC                      glGetAttribLocation;
PFNGLGETBOOLEANVPROC                            glGetBooleanv;
PFNGLGETBUFFERPARAMETERIVPROC                   glGetBufferParameteriv;
PFNGLGETERRORPROC                               glGetError;
PFNGLGETFLOATVPROC                              glGetFloatv;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC    glGetFramebufferAttachmentParameteriv;
PFNGLGETINTEGERVPROC                            glGetIntegerv;
PFNGLGETPROGRAMIVPROC                           glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC                      glGetProgramInfoLog;
PFNGLGETRENDERBUFFERPARAMETERIVPROC             glGetRenderbufferParameteriv;
PFNGLGETSHADERIVPROC                            glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC                       glGetShaderInfoLog;
PFNGLGETSHADERPRECISIONFORMATPROC               glGetShaderPrecisionFormat;
PFNGLGETSHADERSOURCEPROC                        glGetShaderSource;
PFNGLGETSTRINGPROC                              glGetString;
PFNGLGETTEXPARAMETERFVPROC                      glGetTexParameterfv;
PFNGLGETTEXPARAMETERIVPROC                      glGetTexParameteriv;
PFNGLGETUNIFORMFVPROC                           glGetUniformfv;
PFNGLGETUNIFORMIVPROC                           glGetUniformiv;
PFNGLGETUNIFORMLOCATIONPROC                     glGetUniformLocation;
PFNGLGETVERTEXATTRIBFVPROC                      glGetVertexAttribfv;
PFNGLGETVERTEXATTRIBIVPROC                      glGetVertexAttribiv;
PFNGLGETVERTEXATTRIBPOINTERVPROC                glGetVertexAttribPointerv;
PFNGLHINTPROC                                   glHint;
PFNGLISBUFFERPROC                               glIsBuffer;
PFNGLISENABLEDPROC                              glIsEnabled;
PFNGLISFRAMEBUFFERPROC                          glIsFramebuffer;
PFNGLISPROGRAMPROC                              glIsProgram;
PFNGLISRENDERBUFFERPROC                         glIsRenderbuffer;
PFNGLISSHADERPROC                               glIsShader;
PFNGLISTEXTUREPROC                              glIsTexture;
PFNGLLINEWIDTHPROC                              glLineWidth;
PFNGLLINKPROGRAMPROC                            glLinkProgram;
PFNGLPIXELSTOREIPROC                            glPixelStorei;
PFNGLPOLYGONOFFSETPROC                          glPolygonOffset;
PFNGLREADPIXELSPROC                             glReadPixels;
PFNGLRELEASESHADERCOMPILERPROC                  glReleaseShaderCompiler;
PFNGLRENDERBUFFERSTORAGEPROC                    glRenderbufferStorage;
PFNGLSAMPLECOVERAGEPROC                         glSampleCoverage;
PFNGLSCISSORPROC                                glScissor;
PFNGLSHADERBINARYPROC                           glShaderBinary;
PFNGLSHADERSOURCEPROC                           glShaderSource;
PFNGLSTENCILFUNCPROC                            glStencilFunc;
PFNGLSTENCILFUNCSEPARATEPROC                    glStencilFuncSeparate;
PFNGLSTENCILMASKPROC                            glStencilMask;
PFNGLSTENCILMASKSEPARATEPROC                    glStencilMaskSeparate;
PFNGLSTENCILOPPROC                              glStencilOp;
PFNGLSTENCILOPSEPARATEPROC                      glStencilOpSeparate;
PFNGLTEXIMAGE2DPROC                             glTexImage2D;
PFNGLTEXPARAMETERFPROC                          glTexParameterf;
PFNGLTEXPARAMETERFVPROC                         glTexParameterfv;
PFNGLTEXPARAMETERIPROC                          glTexParameteri;
PFNGLTEXPARAMETERIVPROC                         glTexParameteriv;
PFNGLTEXSUBIMAGE2DPROC                          glTexSubImage2D;
PFNGLUNIFORM1FPROC                              glUniform1f;
PFNGLUNIFORM1FVPROC                             glUniform1fv;
PFNGLUNIFORM1IPROC                              glUniform1i;
PFNGLUNIFORM1IVPROC                             glUniform1iv;
PFNGLUNIFORM2FPROC                              glUniform2f;
PFNGLUNIFORM2FVPROC                             glUniform2fv;
PFNGLUNIFORM2IPROC                              glUniform2i;
PFNGLUNIFORM2IVPROC                             glUniform2iv;
PFNGLUNIFORM3FPROC                              glUniform3f;
PFNGLUNIFORM3FVPROC                             glUniform3fv;
PFNGLUNIFORM3IPROC                              glUniform3i;
PFNGLUNIFORM3IVPROC                             glUniform3iv;
PFNGLUNIFORM4FPROC                              glUniform4f;
PFNGLUNIFORM4FVPROC                             glUniform4fv;
PFNGLUNIFORM4IPROC                              glUniform4i;
PFNGLUNIFORM4IVPROC                             glUniform4iv;
PFNGLUNIFORMMATRIX2FVPROC                       glUniformMatrix2fv;
PFNGLUNIFORMMATRIX3FVPROC                       glUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC                       glUniformMatrix4fv;
PFNGLUSEPROGRAMPROC                             glUseProgram;
PFNGLVALIDATEPROGRAMPROC                        glValidateProgram;
PFNGLVERTEXATTRIB1FPROC                         glVertexAttrib1f;
PFNGLVERTEXATTRIB1FVPROC                        glVertexAttrib1fv;
PFNGLVERTEXATTRIB2FPROC                         glVertexAttrib2f;
PFNGLVERTEXATTRIB2FVPROC                        glVertexAttrib2fv;
PFNGLVERTEXATTRIB3FPROC                         glVertexAttrib3f;
PFNGLVERTEXATTRIB3FVPROC                        glVertexAttrib3fv;
PFNGLVERTEXATTRIB4FPROC                         glVertexAttrib4f;
PFNGLVERTEXATTRIB4FVPROC                        glVertexAttrib4fv;
PFNGLVERTEXATTRIBPOINTERPROC                    glVertexAttribPointer;
PFNGLVIEWPORTPROC                               glViewport;

PFNGLDRAWELEMENTSBASEVERTEXEXTPROC              glDrawElementsBaseVertex;

PFNGLGENVERTEXARRAYSOESPROC                     glGenVertexArrays;
PFNGLDELETEVERTEXARRAYSOESPROC                  glDeleteVertexArrays;
PFNGLBINDVERTEXARRAYOESPROC                     glBindVertexArray;
//==============================================================================
