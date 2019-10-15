//==============================================================================
// xxGraphic : OpenGL Function Header
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#pragma once

#include "xxSystem.h"

#define GL_GLES_PROTOTYPES 0
#include "gl/gl32.h"

#ifndef GL_APPLE_texture_format_BGRA8888
#define GL_APPLE_texture_format_BGRA8888 1
#define GL_BGRA_EXT                       0x80E1
#define GL_BGRA8_EXT                      0x93A1
#endif /* GL_APPLE_texture_format_BGRA8888 */

//==============================================================================
//  Initialize
//==============================================================================
bool xxGraphicCreateGL(void* (GL_APIENTRYP getSymbol)(const char* name, bool* failed));
void xxGraphicDestroyGL();
//==============================================================================
//  GLSL Shader
//==============================================================================
xxEXTERN const char* const                              vertexShaderCode;
xxEXTERN const char* const                              fragmentShaderCode;
//==============================================================================
//  Function
//==============================================================================
xxEXTERN uint64_t                                       (*glCreateContext)(uint64_t device, void* view, void** display);
xxEXTERN void                                           (*glDestroyContext)(uint64_t context, void* view, void* display);
xxEXTERN void                                           (*glMakeCurrentContext)(uint64_t context, void* display);
xxEXTERN void                                           (*glPresentContext)(uint64_t context, void* display);
//------------------------------------------------------------------------------
xxEXTERN PFNGLACTIVETEXTUREPROC                         glActiveTexture;
xxEXTERN PFNGLATTACHSHADERPROC                          glAttachShader;
xxEXTERN PFNGLBINDATTRIBLOCATIONPROC                    glBindAttribLocation;
xxEXTERN PFNGLBINDBUFFERPROC                            glBindBuffer;
xxEXTERN PFNGLBINDFRAMEBUFFERPROC                       glBindFramebuffer;
xxEXTERN PFNGLBINDRENDERBUFFERPROC                      glBindRenderbuffer;
xxEXTERN PFNGLBINDTEXTUREPROC                           glBindTexture;
xxEXTERN PFNGLBLENDCOLORPROC                            glBlendColor;
xxEXTERN PFNGLBLENDEQUATIONPROC                         glBlendEquation;
xxEXTERN PFNGLBLENDEQUATIONSEPARATEPROC                 glBlendEquationSeparate;
xxEXTERN PFNGLBLENDFUNCPROC                             glBlendFunc;
xxEXTERN PFNGLBLENDFUNCSEPARATEPROC                     glBlendFuncSeparate;
xxEXTERN PFNGLBUFFERDATAPROC                            glBufferData;
xxEXTERN PFNGLBUFFERSUBDATAPROC                         glBufferSubData;
xxEXTERN PFNGLCHECKFRAMEBUFFERSTATUSPROC                glCheckFramebufferStatus;
xxEXTERN PFNGLCLEARPROC                                 glClear;
xxEXTERN PFNGLCLEARCOLORPROC                            glClearColor;
xxEXTERN PFNGLCLEARDEPTHFPROC                           glClearDepthf;
xxEXTERN PFNGLCLEARSTENCILPROC                          glClearStencil;
xxEXTERN PFNGLCOLORMASKPROC                             glColorMask;
xxEXTERN PFNGLCOMPILESHADERPROC                         glCompileShader;
xxEXTERN PFNGLCOMPRESSEDTEXIMAGE2DPROC                  glCompressedTexImage2D;
xxEXTERN PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC               glCompressedTexSubImage2D;
xxEXTERN PFNGLCOPYTEXIMAGE2DPROC                        glCopyTexImage2D;
xxEXTERN PFNGLCOPYTEXSUBIMAGE2DPROC                     glCopyTexSubImage2D;
xxEXTERN PFNGLCREATEPROGRAMPROC                         glCreateProgram;
xxEXTERN PFNGLCREATESHADERPROC                          glCreateShader;
xxEXTERN PFNGLCULLFACEPROC                              glCullFace;
xxEXTERN PFNGLDELETEBUFFERSPROC                         glDeleteBuffers;
xxEXTERN PFNGLDELETEFRAMEBUFFERSPROC                    glDeleteFramebuffers;
xxEXTERN PFNGLDELETEPROGRAMPROC                         glDeleteProgram;
xxEXTERN PFNGLDELETERENDERBUFFERSPROC                   glDeleteRenderbuffers;
xxEXTERN PFNGLDELETESHADERPROC                          glDeleteShader;
xxEXTERN PFNGLDELETETEXTURESPROC                        glDeleteTextures;
xxEXTERN PFNGLDEPTHFUNCPROC                             glDepthFunc;
xxEXTERN PFNGLDEPTHMASKPROC                             glDepthMask;
xxEXTERN PFNGLDEPTHRANGEFPROC                           glDepthRangef;
xxEXTERN PFNGLDETACHSHADERPROC                          glDetachShader;
xxEXTERN PFNGLDISABLEPROC                               glDisable;
xxEXTERN PFNGLDISABLEVERTEXATTRIBARRAYPROC              glDisableVertexAttribArray;
xxEXTERN PFNGLDRAWARRAYSPROC                            glDrawArrays;
xxEXTERN PFNGLDRAWELEMENTSPROC                          glDrawElements;
xxEXTERN PFNGLENABLEPROC                                glEnable;
xxEXTERN PFNGLENABLEVERTEXATTRIBARRAYPROC               glEnableVertexAttribArray;
xxEXTERN PFNGLFINISHPROC                                glFinish;
xxEXTERN PFNGLFLUSHPROC                                 glFlush;
xxEXTERN PFNGLFRAMEBUFFERRENDERBUFFERPROC               glFramebufferRenderbuffer;
xxEXTERN PFNGLFRAMEBUFFERTEXTURE2DPROC                  glFramebufferTexture2D;
xxEXTERN PFNGLFRONTFACEPROC                             glFrontFace;
xxEXTERN PFNGLGENBUFFERSPROC                            glGenBuffers;
xxEXTERN PFNGLGENERATEMIPMAPPROC                        glGenerateMipmap;
xxEXTERN PFNGLGENFRAMEBUFFERSPROC                       glGenFramebuffers;
xxEXTERN PFNGLGENRENDERBUFFERSPROC                      glGenRenderbuffers;
xxEXTERN PFNGLGENTEXTURESPROC                           glGenTextures;
xxEXTERN PFNGLGETACTIVEATTRIBPROC                       glGetActiveAttrib;
xxEXTERN PFNGLGETACTIVEUNIFORMPROC                      glGetActiveUniform;
xxEXTERN PFNGLGETATTACHEDSHADERSPROC                    glGetAttachedShaders;
xxEXTERN PFNGLGETATTRIBLOCATIONPROC                     glGetAttribLocation;
xxEXTERN PFNGLGETBOOLEANVPROC                           glGetBooleanv;
xxEXTERN PFNGLGETBUFFERPARAMETERIVPROC                  glGetBufferParameteriv;
xxEXTERN PFNGLGETERRORPROC                              glGetError;
xxEXTERN PFNGLGETFLOATVPROC                             glGetFloatv;
xxEXTERN PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC   glGetFramebufferAttachmentParameteriv;
xxEXTERN PFNGLGETINTEGERVPROC                           glGetIntegerv;
xxEXTERN PFNGLGETPROGRAMIVPROC                          glGetProgramiv;
xxEXTERN PFNGLGETPROGRAMINFOLOGPROC                     glGetProgramInfoLog;
xxEXTERN PFNGLGETRENDERBUFFERPARAMETERIVPROC            glGetRenderbufferParameteriv;
xxEXTERN PFNGLGETSHADERIVPROC                           glGetShaderiv;
xxEXTERN PFNGLGETSHADERINFOLOGPROC                      glGetShaderInfoLog;
xxEXTERN PFNGLGETSHADERPRECISIONFORMATPROC              glGetShaderPrecisionFormat;
xxEXTERN PFNGLGETSHADERSOURCEPROC                       glGetShaderSource;
xxEXTERN PFNGLGETSTRINGPROC                             glGetString;
xxEXTERN PFNGLGETTEXPARAMETERFVPROC                     glGetTexParameterfv;
xxEXTERN PFNGLGETTEXPARAMETERIVPROC                     glGetTexParameteriv;
xxEXTERN PFNGLGETUNIFORMFVPROC                          glGetUniformfv;
xxEXTERN PFNGLGETUNIFORMIVPROC                          glGetUniformiv;
xxEXTERN PFNGLGETUNIFORMLOCATIONPROC                    glGetUniformLocation;
xxEXTERN PFNGLGETVERTEXATTRIBFVPROC                     glGetVertexAttribfv;
xxEXTERN PFNGLGETVERTEXATTRIBIVPROC                     glGetVertexAttribiv;
xxEXTERN PFNGLGETVERTEXATTRIBPOINTERVPROC               glGetVertexAttribPointerv;
xxEXTERN PFNGLHINTPROC                                  glHint;
xxEXTERN PFNGLISBUFFERPROC                              glIsBuffer;
xxEXTERN PFNGLISENABLEDPROC                             glIsEnabled;
xxEXTERN PFNGLISFRAMEBUFFERPROC                         glIsFramebuffer;
xxEXTERN PFNGLISPROGRAMPROC                             glIsProgram;
xxEXTERN PFNGLISRENDERBUFFERPROC                        glIsRenderbuffer;
xxEXTERN PFNGLISSHADERPROC                              glIsShader;
xxEXTERN PFNGLISTEXTUREPROC                             glIsTexture;
xxEXTERN PFNGLLINEWIDTHPROC                             glLineWidth;
xxEXTERN PFNGLLINKPROGRAMPROC                           glLinkProgram;
xxEXTERN PFNGLPIXELSTOREIPROC                           glPixelStorei;
xxEXTERN PFNGLPOLYGONOFFSETPROC                         glPolygonOffset;
xxEXTERN PFNGLREADPIXELSPROC                            glReadPixels;
xxEXTERN PFNGLRELEASESHADERCOMPILERPROC                 glReleaseShaderCompiler;
xxEXTERN PFNGLRENDERBUFFERSTORAGEPROC                   glRenderbufferStorage;
xxEXTERN PFNGLSAMPLECOVERAGEPROC                        glSampleCoverage;
xxEXTERN PFNGLSCISSORPROC                               glScissor;
xxEXTERN PFNGLSHADERBINARYPROC                          glShaderBinary;
xxEXTERN PFNGLSHADERSOURCEPROC                          glShaderSource;
xxEXTERN PFNGLSTENCILFUNCPROC                           glStencilFunc;
xxEXTERN PFNGLSTENCILFUNCSEPARATEPROC                   glStencilFuncSeparate;
xxEXTERN PFNGLSTENCILMASKPROC                           glStencilMask;
xxEXTERN PFNGLSTENCILMASKSEPARATEPROC                   glStencilMaskSeparate;
xxEXTERN PFNGLSTENCILOPPROC                             glStencilOp;
xxEXTERN PFNGLSTENCILOPSEPARATEPROC                     glStencilOpSeparate;
xxEXTERN PFNGLTEXIMAGE2DPROC                            glTexImage2D;
xxEXTERN PFNGLTEXPARAMETERFPROC                         glTexParameterf;
xxEXTERN PFNGLTEXPARAMETERFVPROC                        glTexParameterfv;
xxEXTERN PFNGLTEXPARAMETERIPROC                         glTexParameteri;
xxEXTERN PFNGLTEXPARAMETERIVPROC                        glTexParameteriv;
xxEXTERN PFNGLTEXSUBIMAGE2DPROC                         glTexSubImage2D;
xxEXTERN PFNGLUNIFORM1FPROC                             glUniform1f;
xxEXTERN PFNGLUNIFORM1FVPROC                            glUniform1fv;
xxEXTERN PFNGLUNIFORM1IPROC                             glUniform1i;
xxEXTERN PFNGLUNIFORM1IVPROC                            glUniform1iv;
xxEXTERN PFNGLUNIFORM2FPROC                             glUniform2f;
xxEXTERN PFNGLUNIFORM2FVPROC                            glUniform2fv;
xxEXTERN PFNGLUNIFORM2IPROC                             glUniform2i;
xxEXTERN PFNGLUNIFORM2IVPROC                            glUniform2iv;
xxEXTERN PFNGLUNIFORM3FPROC                             glUniform3f;
xxEXTERN PFNGLUNIFORM3FVPROC                            glUniform3fv;
xxEXTERN PFNGLUNIFORM3IPROC                             glUniform3i;
xxEXTERN PFNGLUNIFORM3IVPROC                            glUniform3iv;
xxEXTERN PFNGLUNIFORM4FPROC                             glUniform4f;
xxEXTERN PFNGLUNIFORM4FVPROC                            glUniform4fv;
xxEXTERN PFNGLUNIFORM4IPROC                             glUniform4i;
xxEXTERN PFNGLUNIFORM4IVPROC                            glUniform4iv;
xxEXTERN PFNGLUNIFORMMATRIX2FVPROC                      glUniformMatrix2fv;
xxEXTERN PFNGLUNIFORMMATRIX3FVPROC                      glUniformMatrix3fv;
xxEXTERN PFNGLUNIFORMMATRIX4FVPROC                      glUniformMatrix4fv;
xxEXTERN PFNGLUSEPROGRAMPROC                            glUseProgram;
xxEXTERN PFNGLVALIDATEPROGRAMPROC                       glValidateProgram;
xxEXTERN PFNGLVERTEXATTRIB1FPROC                        glVertexAttrib1f;
xxEXTERN PFNGLVERTEXATTRIB1FVPROC                       glVertexAttrib1fv;
xxEXTERN PFNGLVERTEXATTRIB2FPROC                        glVertexAttrib2f;
xxEXTERN PFNGLVERTEXATTRIB2FVPROC                       glVertexAttrib2fv;
xxEXTERN PFNGLVERTEXATTRIB3FPROC                        glVertexAttrib3f;
xxEXTERN PFNGLVERTEXATTRIB3FVPROC                       glVertexAttrib3fv;
xxEXTERN PFNGLVERTEXATTRIB4FPROC                        glVertexAttrib4f;
xxEXTERN PFNGLVERTEXATTRIB4FVPROC                       glVertexAttrib4fv;
xxEXTERN PFNGLVERTEXATTRIBPOINTERPROC                   glVertexAttribPointer;
xxEXTERN PFNGLVIEWPORTPROC                              glViewport;
