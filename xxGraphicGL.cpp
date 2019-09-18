//==============================================================================
// xxGraphic : OpenGL Function Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphicGL.h"

//==============================================================================
//  Initialize
//==============================================================================
bool xxGraphicCreateGL(void* (*getSymbol)(const char* name, bool* failed))
{
    bool failed = false;

#define symbol(var) (void*&)var = getSymbol(#var, &failed);
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
#undef symbol

    return (failed == false);
}
//------------------------------------------------------------------------------
static void* getNULLSymbol(const char* name, bool* failed)
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
uint64_t                                        (*glCreateContext)(uint64_t instance, void* view, void** display);
void                                            (*glDestroyContext)(uint64_t context, void* view, void* display);
void                                            (*glMakeCurrentContext)(uint64_t context, void* display);
void                                            (*glPresentContext)(uint64_t context, void* display);
void                                            (*glGetViewSize)(void* view, unsigned int* width, unsigned int* height);
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
//==============================================================================
