//==============================================================================
// xxGraphic : OpenGL Function Header
//
// Copyright (c) 2019-2020 TAiGA
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

#ifndef GL_ARB_texture_rectangle
#define GL_ARB_texture_rectangle 1
#define GL_TEXTURE_RECTANGLE_ARB          0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE_ARB  0x84F6
#define GL_PROXY_TEXTURE_RECTANGLE_ARB    0x84F7
#define GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB 0x84F8
#endif /* GL_EXT_texture_rectangle */

#ifndef GL_UNSIGNED_INT_8_8_8_8_REV
#define GL_UNSIGNED_INT_8_8_8_8_REV       0x8367
#endif /* GL_UNSIGNED_INT_8_8_8_8_REV */

#ifndef GLEXPORT
#   if defined(_MSC_VER)
#       define GLEXPORT extern
#   else
#       define GLEXPORT extern __attribute__((visibility("hidden")))
#   endif
#endif

//==============================================================================
//  Initialize
//==============================================================================
bool xxGraphicCreateGLES2(void* (GL_APIENTRYP getSymbol)(const char* name, bool* failed));
bool xxGraphicCreateGLES3(void* (GL_APIENTRYP getSymbol)(const char* name, bool* failed));
bool xxGraphicCreateGLES31(void* (GL_APIENTRYP getSymbol)(const char* name, bool* failed));
bool xxGraphicCreateGLES32(void* (GL_APIENTRYP getSymbol)(const char* name, bool* failed));
void xxGraphicDestroyGL();
//==============================================================================
//  GLSL Shader
//==============================================================================
GLEXPORT const char* const                              glDefaultShaderCode;
//==============================================================================
//  Function
//==============================================================================
GLEXPORT uint64_t                                       (*glCreateContext)(uint64_t device, void* view, void** display);
GLEXPORT void                                           (*glDestroyContext)(uint64_t context, void* view, void* display);
GLEXPORT float                                          (*glGetScaleContext)(uint64_t context, void* view);
GLEXPORT void                                           (*glMakeCurrentContext)(uint64_t context, void* display);
GLEXPORT void                                           (*glPresentContext)(uint64_t context, void* display);
//------------------------------------------------------------------------------
#if GL_ES_VERSION_2_0
GLEXPORT PFNGLACTIVETEXTUREPROC                         glActiveTexture;
GLEXPORT PFNGLATTACHSHADERPROC                          glAttachShader;
GLEXPORT PFNGLBINDATTRIBLOCATIONPROC                    glBindAttribLocation;
GLEXPORT PFNGLBINDBUFFERPROC                            glBindBuffer;
GLEXPORT PFNGLBINDFRAMEBUFFERPROC                       glBindFramebuffer;
GLEXPORT PFNGLBINDRENDERBUFFERPROC                      glBindRenderbuffer;
GLEXPORT PFNGLBINDTEXTUREPROC                           glBindTexture;
GLEXPORT PFNGLBLENDCOLORPROC                            glBlendColor;
GLEXPORT PFNGLBLENDEQUATIONPROC                         glBlendEquation;
GLEXPORT PFNGLBLENDEQUATIONSEPARATEPROC                 glBlendEquationSeparate;
GLEXPORT PFNGLBLENDFUNCPROC                             glBlendFunc;
GLEXPORT PFNGLBLENDFUNCSEPARATEPROC                     glBlendFuncSeparate;
GLEXPORT PFNGLBUFFERDATAPROC                            glBufferData;
GLEXPORT PFNGLBUFFERSUBDATAPROC                         glBufferSubData;
GLEXPORT PFNGLCHECKFRAMEBUFFERSTATUSPROC                glCheckFramebufferStatus;
GLEXPORT PFNGLCLEARPROC                                 glClear;
GLEXPORT PFNGLCLEARCOLORPROC                            glClearColor;
GLEXPORT PFNGLCLEARDEPTHFPROC                           glClearDepthf;
GLEXPORT PFNGLCLEARSTENCILPROC                          glClearStencil;
GLEXPORT PFNGLCOLORMASKPROC                             glColorMask;
GLEXPORT PFNGLCOMPILESHADERPROC                         glCompileShader;
GLEXPORT PFNGLCOMPRESSEDTEXIMAGE2DPROC                  glCompressedTexImage2D;
GLEXPORT PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC               glCompressedTexSubImage2D;
GLEXPORT PFNGLCOPYTEXIMAGE2DPROC                        glCopyTexImage2D;
GLEXPORT PFNGLCOPYTEXSUBIMAGE2DPROC                     glCopyTexSubImage2D;
GLEXPORT PFNGLCREATEPROGRAMPROC                         glCreateProgram;
GLEXPORT PFNGLCREATESHADERPROC                          glCreateShader;
GLEXPORT PFNGLCULLFACEPROC                              glCullFace;
GLEXPORT PFNGLDELETEBUFFERSPROC                         glDeleteBuffers;
GLEXPORT PFNGLDELETEFRAMEBUFFERSPROC                    glDeleteFramebuffers;
GLEXPORT PFNGLDELETEPROGRAMPROC                         glDeleteProgram;
GLEXPORT PFNGLDELETERENDERBUFFERSPROC                   glDeleteRenderbuffers;
GLEXPORT PFNGLDELETESHADERPROC                          glDeleteShader;
GLEXPORT PFNGLDELETETEXTURESPROC                        glDeleteTextures;
GLEXPORT PFNGLDEPTHFUNCPROC                             glDepthFunc;
GLEXPORT PFNGLDEPTHMASKPROC                             glDepthMask;
GLEXPORT PFNGLDEPTHRANGEFPROC                           glDepthRangef;
GLEXPORT PFNGLDETACHSHADERPROC                          glDetachShader;
GLEXPORT PFNGLDISABLEPROC                               glDisable;
GLEXPORT PFNGLDISABLEVERTEXATTRIBARRAYPROC              glDisableVertexAttribArray;
GLEXPORT PFNGLDRAWARRAYSPROC                            glDrawArrays;
GLEXPORT PFNGLDRAWELEMENTSPROC                          glDrawElements;
GLEXPORT PFNGLENABLEPROC                                glEnable;
GLEXPORT PFNGLENABLEVERTEXATTRIBARRAYPROC               glEnableVertexAttribArray;
GLEXPORT PFNGLFINISHPROC                                glFinish;
GLEXPORT PFNGLFLUSHPROC                                 glFlush;
GLEXPORT PFNGLFRAMEBUFFERRENDERBUFFERPROC               glFramebufferRenderbuffer;
GLEXPORT PFNGLFRAMEBUFFERTEXTURE2DPROC                  glFramebufferTexture2D;
GLEXPORT PFNGLFRONTFACEPROC                             glFrontFace;
GLEXPORT PFNGLGENBUFFERSPROC                            glGenBuffers;
GLEXPORT PFNGLGENERATEMIPMAPPROC                        glGenerateMipmap;
GLEXPORT PFNGLGENFRAMEBUFFERSPROC                       glGenFramebuffers;
GLEXPORT PFNGLGENRENDERBUFFERSPROC                      glGenRenderbuffers;
GLEXPORT PFNGLGENTEXTURESPROC                           glGenTextures;
GLEXPORT PFNGLGETACTIVEATTRIBPROC                       glGetActiveAttrib;
GLEXPORT PFNGLGETACTIVEUNIFORMPROC                      glGetActiveUniform;
GLEXPORT PFNGLGETATTACHEDSHADERSPROC                    glGetAttachedShaders;
GLEXPORT PFNGLGETATTRIBLOCATIONPROC                     glGetAttribLocation;
GLEXPORT PFNGLGETBOOLEANVPROC                           glGetBooleanv;
GLEXPORT PFNGLGETBUFFERPARAMETERIVPROC                  glGetBufferParameteriv;
GLEXPORT PFNGLGETERRORPROC                              glGetError;
GLEXPORT PFNGLGETFLOATVPROC                             glGetFloatv;
GLEXPORT PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC   glGetFramebufferAttachmentParameteriv;
GLEXPORT PFNGLGETINTEGERVPROC                           glGetIntegerv;
GLEXPORT PFNGLGETPROGRAMIVPROC                          glGetProgramiv;
GLEXPORT PFNGLGETPROGRAMINFOLOGPROC                     glGetProgramInfoLog;
GLEXPORT PFNGLGETRENDERBUFFERPARAMETERIVPROC            glGetRenderbufferParameteriv;
GLEXPORT PFNGLGETSHADERIVPROC                           glGetShaderiv;
GLEXPORT PFNGLGETSHADERINFOLOGPROC                      glGetShaderInfoLog;
GLEXPORT PFNGLGETSHADERPRECISIONFORMATPROC              glGetShaderPrecisionFormat;
GLEXPORT PFNGLGETSHADERSOURCEPROC                       glGetShaderSource;
GLEXPORT PFNGLGETSTRINGPROC                             glGetString;
GLEXPORT PFNGLGETTEXPARAMETERFVPROC                     glGetTexParameterfv;
GLEXPORT PFNGLGETTEXPARAMETERIVPROC                     glGetTexParameteriv;
GLEXPORT PFNGLGETUNIFORMFVPROC                          glGetUniformfv;
GLEXPORT PFNGLGETUNIFORMIVPROC                          glGetUniformiv;
GLEXPORT PFNGLGETUNIFORMLOCATIONPROC                    glGetUniformLocation;
GLEXPORT PFNGLGETVERTEXATTRIBFVPROC                     glGetVertexAttribfv;
GLEXPORT PFNGLGETVERTEXATTRIBIVPROC                     glGetVertexAttribiv;
GLEXPORT PFNGLGETVERTEXATTRIBPOINTERVPROC               glGetVertexAttribPointerv;
GLEXPORT PFNGLHINTPROC                                  glHint;
GLEXPORT PFNGLISBUFFERPROC                              glIsBuffer;
GLEXPORT PFNGLISENABLEDPROC                             glIsEnabled;
GLEXPORT PFNGLISFRAMEBUFFERPROC                         glIsFramebuffer;
GLEXPORT PFNGLISPROGRAMPROC                             glIsProgram;
GLEXPORT PFNGLISRENDERBUFFERPROC                        glIsRenderbuffer;
GLEXPORT PFNGLISSHADERPROC                              glIsShader;
GLEXPORT PFNGLISTEXTUREPROC                             glIsTexture;
GLEXPORT PFNGLLINEWIDTHPROC                             glLineWidth;
GLEXPORT PFNGLLINKPROGRAMPROC                           glLinkProgram;
GLEXPORT PFNGLPIXELSTOREIPROC                           glPixelStorei;
GLEXPORT PFNGLPOLYGONOFFSETPROC                         glPolygonOffset;
GLEXPORT PFNGLREADPIXELSPROC                            glReadPixels;
GLEXPORT PFNGLRELEASESHADERCOMPILERPROC                 glReleaseShaderCompiler;
GLEXPORT PFNGLRENDERBUFFERSTORAGEPROC                   glRenderbufferStorage;
GLEXPORT PFNGLSAMPLECOVERAGEPROC                        glSampleCoverage;
GLEXPORT PFNGLSCISSORPROC                               glScissor;
GLEXPORT PFNGLSHADERBINARYPROC                          glShaderBinary;
GLEXPORT PFNGLSHADERSOURCEPROC                          glShaderSource;
GLEXPORT PFNGLSTENCILFUNCPROC                           glStencilFunc;
GLEXPORT PFNGLSTENCILFUNCSEPARATEPROC                   glStencilFuncSeparate;
GLEXPORT PFNGLSTENCILMASKPROC                           glStencilMask;
GLEXPORT PFNGLSTENCILMASKSEPARATEPROC                   glStencilMaskSeparate;
GLEXPORT PFNGLSTENCILOPPROC                             glStencilOp;
GLEXPORT PFNGLSTENCILOPSEPARATEPROC                     glStencilOpSeparate;
GLEXPORT PFNGLTEXIMAGE2DPROC                            glTexImage2D;
GLEXPORT PFNGLTEXPARAMETERFPROC                         glTexParameterf;
GLEXPORT PFNGLTEXPARAMETERFVPROC                        glTexParameterfv;
GLEXPORT PFNGLTEXPARAMETERIPROC                         glTexParameteri;
GLEXPORT PFNGLTEXPARAMETERIVPROC                        glTexParameteriv;
GLEXPORT PFNGLTEXSUBIMAGE2DPROC                         glTexSubImage2D;
GLEXPORT PFNGLUNIFORM1FPROC                             glUniform1f;
GLEXPORT PFNGLUNIFORM1FVPROC                            glUniform1fv;
GLEXPORT PFNGLUNIFORM1IPROC                             glUniform1i;
GLEXPORT PFNGLUNIFORM1IVPROC                            glUniform1iv;
GLEXPORT PFNGLUNIFORM2FPROC                             glUniform2f;
GLEXPORT PFNGLUNIFORM2FVPROC                            glUniform2fv;
GLEXPORT PFNGLUNIFORM2IPROC                             glUniform2i;
GLEXPORT PFNGLUNIFORM2IVPROC                            glUniform2iv;
GLEXPORT PFNGLUNIFORM3FPROC                             glUniform3f;
GLEXPORT PFNGLUNIFORM3FVPROC                            glUniform3fv;
GLEXPORT PFNGLUNIFORM3IPROC                             glUniform3i;
GLEXPORT PFNGLUNIFORM3IVPROC                            glUniform3iv;
GLEXPORT PFNGLUNIFORM4FPROC                             glUniform4f;
GLEXPORT PFNGLUNIFORM4FVPROC                            glUniform4fv;
GLEXPORT PFNGLUNIFORM4IPROC                             glUniform4i;
GLEXPORT PFNGLUNIFORM4IVPROC                            glUniform4iv;
GLEXPORT PFNGLUNIFORMMATRIX2FVPROC                      glUniformMatrix2fv;
GLEXPORT PFNGLUNIFORMMATRIX3FVPROC                      glUniformMatrix3fv;
GLEXPORT PFNGLUNIFORMMATRIX4FVPROC                      glUniformMatrix4fv;
GLEXPORT PFNGLUSEPROGRAMPROC                            glUseProgram;
GLEXPORT PFNGLVALIDATEPROGRAMPROC                       glValidateProgram;
GLEXPORT PFNGLVERTEXATTRIB1FPROC                        glVertexAttrib1f;
GLEXPORT PFNGLVERTEXATTRIB1FVPROC                       glVertexAttrib1fv;
GLEXPORT PFNGLVERTEXATTRIB2FPROC                        glVertexAttrib2f;
GLEXPORT PFNGLVERTEXATTRIB2FVPROC                       glVertexAttrib2fv;
GLEXPORT PFNGLVERTEXATTRIB3FPROC                        glVertexAttrib3f;
GLEXPORT PFNGLVERTEXATTRIB3FVPROC                       glVertexAttrib3fv;
GLEXPORT PFNGLVERTEXATTRIB4FPROC                        glVertexAttrib4f;
GLEXPORT PFNGLVERTEXATTRIB4FVPROC                       glVertexAttrib4fv;
GLEXPORT PFNGLVERTEXATTRIBPOINTERPROC                   glVertexAttribPointer;
GLEXPORT PFNGLVIEWPORTPROC                              glViewport;
#endif
//------------------------------------------------------------------------------
#if GL_ES_VERSION_3_0
GLEXPORT PFNGLREADBUFFERPROC                            glReadBuffer;
GLEXPORT PFNGLDRAWRANGEELEMENTSPROC                     glDrawRangeElements;
GLEXPORT PFNGLTEXIMAGE3DPROC                            glTexImage3D;
GLEXPORT PFNGLTEXSUBIMAGE3DPROC                         glTexSubImage3D;
GLEXPORT PFNGLCOPYTEXSUBIMAGE3DPROC                     glCopyTexSubImage3D;
GLEXPORT PFNGLCOMPRESSEDTEXIMAGE3DPROC                  glCompressedTexImage3D;
GLEXPORT PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC               glCompressedTexSubImage3D;
GLEXPORT PFNGLGENQUERIESPROC                            glGenQueries;
GLEXPORT PFNGLDELETEQUERIESPROC                         glDeleteQueries;
GLEXPORT PFNGLISQUERYPROC                               glIsQuery;
GLEXPORT PFNGLBEGINQUERYPROC                            glBeginQuery;
GLEXPORT PFNGLENDQUERYPROC                              glEndQuery;
GLEXPORT PFNGLGETQUERYIVPROC                            glGetQueryiv;
GLEXPORT PFNGLGETQUERYOBJECTUIVPROC                     glGetQueryObjectuiv;
GLEXPORT PFNGLUNMAPBUFFERPROC                           glUnmapBuffer;
GLEXPORT PFNGLGETBUFFERPOINTERVPROC                     glGetBufferPointerv;
GLEXPORT PFNGLDRAWBUFFERSPROC                           glDrawBuffers;
GLEXPORT PFNGLUNIFORMMATRIX2X3FVPROC                    glUniformMatrix2x3fv;
GLEXPORT PFNGLUNIFORMMATRIX3X2FVPROC                    glUniformMatrix3x2fv;
GLEXPORT PFNGLUNIFORMMATRIX2X4FVPROC                    glUniformMatrix2x4fv;
GLEXPORT PFNGLUNIFORMMATRIX4X2FVPROC                    glUniformMatrix4x2fv;
GLEXPORT PFNGLUNIFORMMATRIX3X4FVPROC                    glUniformMatrix3x4fv;
GLEXPORT PFNGLUNIFORMMATRIX4X3FVPROC                    glUniformMatrix4x3fv;
GLEXPORT PFNGLBLITFRAMEBUFFERPROC                       glBlitFramebuffer;
GLEXPORT PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC        glRenderbufferStorageMultisample;
GLEXPORT PFNGLFRAMEBUFFERTEXTURELAYERPROC               glFramebufferTextureLayer;
GLEXPORT PFNGLMAPBUFFERRANGEPROC                        glMapBufferRange;
GLEXPORT PFNGLFLUSHMAPPEDBUFFERRANGEPROC                glFlushMappedBufferRange;
GLEXPORT PFNGLBINDVERTEXARRAYPROC                       glBindVertexArray;
GLEXPORT PFNGLDELETEVERTEXARRAYSPROC                    glDeleteVertexArrays;
GLEXPORT PFNGLGENVERTEXARRAYSPROC                       glGenVertexArrays;
GLEXPORT PFNGLISVERTEXARRAYPROC                         glIsVertexArray;
GLEXPORT PFNGLGETINTEGERI_VPROC                         glGetIntegeri_v;
GLEXPORT PFNGLBEGINTRANSFORMFEEDBACKPROC                glBeginTransformFeedback;
GLEXPORT PFNGLENDTRANSFORMFEEDBACKPROC                  glEndTransformFeedback;
GLEXPORT PFNGLBINDBUFFERRANGEPROC                       glBindBufferRange;
GLEXPORT PFNGLBINDBUFFERBASEPROC                        glBindBufferBase;
GLEXPORT PFNGLTRANSFORMFEEDBACKVARYINGSPROC             glTransformFeedbackVaryings;
GLEXPORT PFNGLGETTRANSFORMFEEDBACKVARYINGPROC           glGetTransformFeedbackVarying;
GLEXPORT PFNGLVERTEXATTRIBIPOINTERPROC                  glVertexAttribIPointer;
GLEXPORT PFNGLGETVERTEXATTRIBIIVPROC                    glGetVertexAttribIiv;
GLEXPORT PFNGLGETVERTEXATTRIBIUIVPROC                   glGetVertexAttribIuiv;
GLEXPORT PFNGLVERTEXATTRIBI4IPROC                       glVertexAttribI4i;
GLEXPORT PFNGLVERTEXATTRIBI4UIPROC                      glVertexAttribI4ui;
GLEXPORT PFNGLVERTEXATTRIBI4IVPROC                      glVertexAttribI4iv;
GLEXPORT PFNGLVERTEXATTRIBI4UIVPROC                     glVertexAttribI4uiv;
GLEXPORT PFNGLGETUNIFORMUIVPROC                         glGetUniformuiv;
GLEXPORT PFNGLGETFRAGDATALOCATIONPROC                   glGetFragDataLocation;
GLEXPORT PFNGLUNIFORM1UIPROC                            glUniform1ui;
GLEXPORT PFNGLUNIFORM2UIPROC                            glUniform2ui;
GLEXPORT PFNGLUNIFORM3UIPROC                            glUniform3ui;
GLEXPORT PFNGLUNIFORM4UIPROC                            glUniform4ui;
GLEXPORT PFNGLUNIFORM1UIVPROC                           glUniform1uiv;
GLEXPORT PFNGLUNIFORM2UIVPROC                           glUniform2uiv;
GLEXPORT PFNGLUNIFORM3UIVPROC                           glUniform3uiv;
GLEXPORT PFNGLUNIFORM4UIVPROC                           glUniform4uiv;
GLEXPORT PFNGLCLEARBUFFERIVPROC                         glClearBufferiv;
GLEXPORT PFNGLCLEARBUFFERUIVPROC                        glClearBufferuiv;
GLEXPORT PFNGLCLEARBUFFERFVPROC                         glClearBufferfv;
GLEXPORT PFNGLCLEARBUFFERFIPROC                         glClearBufferfi;
GLEXPORT PFNGLGETSTRINGIPROC                            glGetStringi;
GLEXPORT PFNGLCOPYBUFFERSUBDATAPROC                     glCopyBufferSubData;
GLEXPORT PFNGLGETUNIFORMINDICESPROC                     glGetUniformIndices;
GLEXPORT PFNGLGETACTIVEUNIFORMSIVPROC                   glGetActiveUniformsiv;
GLEXPORT PFNGLGETUNIFORMBLOCKINDEXPROC                  glGetUniformBlockIndex;
GLEXPORT PFNGLGETACTIVEUNIFORMBLOCKIVPROC               glGetActiveUniformBlockiv;
GLEXPORT PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC             glGetActiveUniformBlockName;
GLEXPORT PFNGLUNIFORMBLOCKBINDINGPROC                   glUniformBlockBinding;
GLEXPORT PFNGLDRAWARRAYSINSTANCEDPROC                   glDrawArraysInstanced;
GLEXPORT PFNGLDRAWELEMENTSINSTANCEDPROC                 glDrawElementsInstanced;
GLEXPORT PFNGLFENCESYNCPROC                             glFenceSync;
GLEXPORT PFNGLISSYNCPROC                                glIsSync;
GLEXPORT PFNGLDELETESYNCPROC                            glDeleteSync;
GLEXPORT PFNGLCLIENTWAITSYNCPROC                        glClientWaitSync;
GLEXPORT PFNGLWAITSYNCPROC                              glWaitSync;
GLEXPORT PFNGLGETINTEGER64VPROC                         glGetInteger64v;
GLEXPORT PFNGLGETSYNCIVPROC                             glGetSynciv;
GLEXPORT PFNGLGETINTEGER64I_VPROC                       glGetInteger64i_v;
GLEXPORT PFNGLGETBUFFERPARAMETERI64VPROC                glGetBufferParameteri64v;
GLEXPORT PFNGLGENSAMPLERSPROC                           glGenSamplers;
GLEXPORT PFNGLDELETESAMPLERSPROC                        glDeleteSamplers;
GLEXPORT PFNGLISSAMPLERPROC                             glIsSampler;
GLEXPORT PFNGLBINDSAMPLERPROC                           glBindSampler;
GLEXPORT PFNGLSAMPLERPARAMETERIPROC                     glSamplerParameteri;
GLEXPORT PFNGLSAMPLERPARAMETERIVPROC                    glSamplerParameteriv;
GLEXPORT PFNGLSAMPLERPARAMETERFPROC                     glSamplerParameterf;
GLEXPORT PFNGLSAMPLERPARAMETERFVPROC                    glSamplerParameterfv;
GLEXPORT PFNGLGETSAMPLERPARAMETERIVPROC                 glGetSamplerParameteriv;
GLEXPORT PFNGLGETSAMPLERPARAMETERFVPROC                 glGetSamplerParameterfv;
GLEXPORT PFNGLVERTEXATTRIBDIVISORPROC                   glVertexAttribDivisor;
GLEXPORT PFNGLBINDTRANSFORMFEEDBACKPROC                 glBindTransformFeedback;
GLEXPORT PFNGLDELETETRANSFORMFEEDBACKSPROC              glDeleteTransformFeedbacks;
GLEXPORT PFNGLGENTRANSFORMFEEDBACKSPROC                 glGenTransformFeedbacks;
GLEXPORT PFNGLISTRANSFORMFEEDBACKPROC                   glIsTransformFeedback;
GLEXPORT PFNGLPAUSETRANSFORMFEEDBACKPROC                glPauseTransformFeedback;
GLEXPORT PFNGLRESUMETRANSFORMFEEDBACKPROC               glResumeTransformFeedback;
GLEXPORT PFNGLGETPROGRAMBINARYPROC                      glGetProgramBinary;
GLEXPORT PFNGLPROGRAMBINARYPROC                         glProgramBinary;
GLEXPORT PFNGLPROGRAMPARAMETERIPROC                     glProgramParameteri;
GLEXPORT PFNGLINVALIDATEFRAMEBUFFERPROC                 glInvalidateFramebuffer;
GLEXPORT PFNGLINVALIDATESUBFRAMEBUFFERPROC              glInvalidateSubFramebuffer;
GLEXPORT PFNGLTEXSTORAGE2DPROC                          glTexStorage2D;
GLEXPORT PFNGLTEXSTORAGE3DPROC                          glTexStorage3D;
GLEXPORT PFNGLGETINTERNALFORMATIVPROC                   glGetInternalformativ;
#endif
//------------------------------------------------------------------------------
#if GL_ES_VERSION_3_1
GLEXPORT PFNGLDISPATCHCOMPUTEPROC                       glDispatchCompute;
GLEXPORT PFNGLDISPATCHCOMPUTEINDIRECTPROC               glDispatchComputeIndirect;
GLEXPORT PFNGLDRAWARRAYSINDIRECTPROC                    glDrawArraysIndirect;
GLEXPORT PFNGLDRAWELEMENTSINDIRECTPROC                  glDrawElementsIndirect;
GLEXPORT PFNGLFRAMEBUFFERPARAMETERIPROC                 glFramebufferParameteri;
GLEXPORT PFNGLGETFRAMEBUFFERPARAMETERIVPROC             glGetFramebufferParameteriv;
GLEXPORT PFNGLGETPROGRAMINTERFACEIVPROC                 glGetProgramInterfaceiv;
GLEXPORT PFNGLGETPROGRAMRESOURCEINDEXPROC               glGetProgramResourceIndex;
GLEXPORT PFNGLGETPROGRAMRESOURCENAMEPROC                glGetProgramResourceName;
GLEXPORT PFNGLGETPROGRAMRESOURCEIVPROC                  glGetProgramResourceiv;
GLEXPORT PFNGLGETPROGRAMRESOURCELOCATIONPROC            glGetProgramResourceLocation;
GLEXPORT PFNGLUSEPROGRAMSTAGESPROC                      glUseProgramStages;
GLEXPORT PFNGLACTIVESHADERPROGRAMPROC                   glActiveShaderProgram;
GLEXPORT PFNGLCREATESHADERPROGRAMVPROC                  glCreateShaderProgramv;
GLEXPORT PFNGLBINDPROGRAMPIPELINEPROC                   glBindProgramPipeline;
GLEXPORT PFNGLDELETEPROGRAMPIPELINESPROC                glDeleteProgramPipelines;
GLEXPORT PFNGLGENPROGRAMPIPELINESPROC                   glGenProgramPipelines;
GLEXPORT PFNGLISPROGRAMPIPELINEPROC                     glIsProgramPipeline;
GLEXPORT PFNGLGETPROGRAMPIPELINEIVPROC                  glGetProgramPipelineiv;
GLEXPORT PFNGLPROGRAMUNIFORM1IPROC                      glProgramUniform1i;
GLEXPORT PFNGLPROGRAMUNIFORM2IPROC                      glProgramUniform2i;
GLEXPORT PFNGLPROGRAMUNIFORM3IPROC                      glProgramUniform3i;
GLEXPORT PFNGLPROGRAMUNIFORM4IPROC                      glProgramUniform4i;
GLEXPORT PFNGLPROGRAMUNIFORM1UIPROC                     glProgramUniform1ui;
GLEXPORT PFNGLPROGRAMUNIFORM2UIPROC                     glProgramUniform2ui;
GLEXPORT PFNGLPROGRAMUNIFORM3UIPROC                     glProgramUniform3ui;
GLEXPORT PFNGLPROGRAMUNIFORM4UIPROC                     glProgramUniform4ui;
GLEXPORT PFNGLPROGRAMUNIFORM1FPROC                      glProgramUniform1f;
GLEXPORT PFNGLPROGRAMUNIFORM2FPROC                      glProgramUniform2f;
GLEXPORT PFNGLPROGRAMUNIFORM3FPROC                      glProgramUniform3f;
GLEXPORT PFNGLPROGRAMUNIFORM4FPROC                      glProgramUniform4f;
GLEXPORT PFNGLPROGRAMUNIFORM1IVPROC                     glProgramUniform1iv;
GLEXPORT PFNGLPROGRAMUNIFORM2IVPROC                     glProgramUniform2iv;
GLEXPORT PFNGLPROGRAMUNIFORM3IVPROC                     glProgramUniform3iv;
GLEXPORT PFNGLPROGRAMUNIFORM4IVPROC                     glProgramUniform4iv;
GLEXPORT PFNGLPROGRAMUNIFORM1UIVPROC                    glProgramUniform1uiv;
GLEXPORT PFNGLPROGRAMUNIFORM2UIVPROC                    glProgramUniform2uiv;
GLEXPORT PFNGLPROGRAMUNIFORM3UIVPROC                    glProgramUniform3uiv;
GLEXPORT PFNGLPROGRAMUNIFORM4UIVPROC                    glProgramUniform4uiv;
GLEXPORT PFNGLPROGRAMUNIFORM1FVPROC                     glProgramUniform1fv;
GLEXPORT PFNGLPROGRAMUNIFORM2FVPROC                     glProgramUniform2fv;
GLEXPORT PFNGLPROGRAMUNIFORM3FVPROC                     glProgramUniform3fv;
GLEXPORT PFNGLPROGRAMUNIFORM4FVPROC                     glProgramUniform4fv;
GLEXPORT PFNGLPROGRAMUNIFORMMATRIX2FVPROC               glProgramUniformMatrix2fv;
GLEXPORT PFNGLPROGRAMUNIFORMMATRIX3FVPROC               glProgramUniformMatrix3fv;
GLEXPORT PFNGLPROGRAMUNIFORMMATRIX4FVPROC               glProgramUniformMatrix4fv;
GLEXPORT PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC             glProgramUniformMatrix2x3fv;
GLEXPORT PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC             glProgramUniformMatrix3x2fv;
GLEXPORT PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC             glProgramUniformMatrix2x4fv;
GLEXPORT PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC             glProgramUniformMatrix4x2fv;
GLEXPORT PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC             glProgramUniformMatrix3x4fv;
GLEXPORT PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC             glProgramUniformMatrix4x3fv;
GLEXPORT PFNGLVALIDATEPROGRAMPIPELINEPROC               glValidateProgramPipeline;
GLEXPORT PFNGLGETPROGRAMPIPELINEINFOLOGPROC             glGetProgramPipelineInfoLog;
GLEXPORT PFNGLBINDIMAGETEXTUREPROC                      glBindImageTexture;
GLEXPORT PFNGLGETBOOLEANI_VPROC                         glGetBooleani_v;
GLEXPORT PFNGLMEMORYBARRIERPROC                         glMemoryBarrier;
GLEXPORT PFNGLMEMORYBARRIERBYREGIONPROC                 glMemoryBarrierByRegion;
GLEXPORT PFNGLTEXSTORAGE2DMULTISAMPLEPROC               glTexStorage2DMultisample;
GLEXPORT PFNGLGETMULTISAMPLEFVPROC                      glGetMultisamplefv;
GLEXPORT PFNGLSAMPLEMASKIPROC                           glSampleMaski;
GLEXPORT PFNGLGETTEXLEVELPARAMETERIVPROC                glGetTexLevelParameteriv;
GLEXPORT PFNGLGETTEXLEVELPARAMETERFVPROC                glGetTexLevelParameterfv;
GLEXPORT PFNGLBINDVERTEXBUFFERPROC                      glBindVertexBuffer;
GLEXPORT PFNGLVERTEXATTRIBFORMATPROC                    glVertexAttribFormat;
GLEXPORT PFNGLVERTEXATTRIBIFORMATPROC                   glVertexAttribIFormat;
GLEXPORT PFNGLVERTEXATTRIBBINDINGPROC                   glVertexAttribBinding;
GLEXPORT PFNGLVERTEXBINDINGDIVISORPROC                  glVertexBindingDivisor;
#endif
//------------------------------------------------------------------------------
#if GL_ES_VERSION_3_2
GLEXPORT PFNGLBLENDBARRIERPROC                          glBlendBarrier;
GLEXPORT PFNGLCOPYIMAGESUBDATAPROC                      glCopyImageSubData;
GLEXPORT PFNGLDEBUGMESSAGECONTROLPROC                   glDebugMessageControl;
GLEXPORT PFNGLDEBUGMESSAGEINSERTPROC                    glDebugMessageInsert;
GLEXPORT PFNGLDEBUGMESSAGECALLBACKPROC                  glDebugMessageCallback;
GLEXPORT PFNGLGETDEBUGMESSAGELOGPROC                    glGetDebugMessageLog;
GLEXPORT PFNGLPUSHDEBUGGROUPPROC                        glPushDebugGroup;
GLEXPORT PFNGLPOPDEBUGGROUPPROC                         glPopDebugGroup;
GLEXPORT PFNGLOBJECTLABELPROC                           glObjectLabel;
GLEXPORT PFNGLGETOBJECTLABELPROC                        glGetObjectLabel;
GLEXPORT PFNGLOBJECTPTRLABELPROC                        glObjectPtrLabel;
GLEXPORT PFNGLGETOBJECTPTRLABELPROC                     glGetObjectPtrLabel;
GLEXPORT PFNGLGETPOINTERVPROC                           glGetPointerv;
GLEXPORT PFNGLENABLEIPROC                               glEnablei;
GLEXPORT PFNGLDISABLEIPROC                              glDisablei;
GLEXPORT PFNGLBLENDEQUATIONIPROC                        glBlendEquationi;
GLEXPORT PFNGLBLENDEQUATIONSEPARATEIPROC                glBlendEquationSeparatei;
GLEXPORT PFNGLBLENDFUNCIPROC                            glBlendFunci;
GLEXPORT PFNGLBLENDFUNCSEPARATEIPROC                    glBlendFuncSeparatei;
GLEXPORT PFNGLCOLORMASKIPROC                            glColorMaski;
GLEXPORT PFNGLISENABLEDIPROC                            glIsEnabledi;
GLEXPORT PFNGLDRAWELEMENTSBASEVERTEXPROC                glDrawElementsBaseVertex;
GLEXPORT PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC           glDrawRangeElementsBaseVertex;
GLEXPORT PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC       glDrawElementsInstancedBaseVertex;
GLEXPORT PFNGLFRAMEBUFFERTEXTUREPROC                    glFramebufferTexture;
GLEXPORT PFNGLPRIMITIVEBOUNDINGBOXPROC                  glPrimitiveBoundingBox;
GLEXPORT PFNGLGETGRAPHICSRESETSTATUSPROC                glGetGraphicsResetStatus;
GLEXPORT PFNGLREADNPIXELSPROC                           glReadnPixels;
GLEXPORT PFNGLGETNUNIFORMFVPROC                         glGetnUniformfv;
GLEXPORT PFNGLGETNUNIFORMIVPROC                         glGetnUniformiv;
GLEXPORT PFNGLGETNUNIFORMUIVPROC                        glGetnUniformuiv;
GLEXPORT PFNGLMINSAMPLESHADINGPROC                      glMinSampleShading;
GLEXPORT PFNGLPATCHPARAMETERIPROC                       glPatchParameteri;
GLEXPORT PFNGLTEXPARAMETERIIVPROC                       glTexParameterIiv;
GLEXPORT PFNGLTEXPARAMETERIUIVPROC                      glTexParameterIuiv;
GLEXPORT PFNGLGETTEXPARAMETERIIVPROC                    glGetTexParameterIiv;
GLEXPORT PFNGLGETTEXPARAMETERIUIVPROC                   glGetTexParameterIuiv;
GLEXPORT PFNGLSAMPLERPARAMETERIIVPROC                   glSamplerParameterIiv;
GLEXPORT PFNGLSAMPLERPARAMETERIUIVPROC                  glSamplerParameterIuiv;
GLEXPORT PFNGLGETSAMPLERPARAMETERIIVPROC                glGetSamplerParameterIiv;
GLEXPORT PFNGLGETSAMPLERPARAMETERIUIVPROC               glGetSamplerParameterIuiv;
GLEXPORT PFNGLTEXBUFFERPROC                             glTexBuffer;
GLEXPORT PFNGLTEXBUFFERRANGEPROC                        glTexBufferRange;
GLEXPORT PFNGLTEXSTORAGE3DMULTISAMPLEPROC               glTexStorage3DMultisample;
#endif
//------------------------------------------------------------------------------
