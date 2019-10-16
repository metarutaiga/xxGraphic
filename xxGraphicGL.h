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
bool xxGraphicCreateGLES2(void* (GL_APIENTRYP getSymbol)(const char* name, bool* failed));
bool xxGraphicCreateGLES3(void* (GL_APIENTRYP getSymbol)(const char* name, bool* failed));
bool xxGraphicCreateGLES31(void* (GL_APIENTRYP getSymbol)(const char* name, bool* failed));
bool xxGraphicCreateGLES32(void* (GL_APIENTRYP getSymbol)(const char* name, bool* failed));
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
#if GL_ES_VERSION_2_0
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
#endif
//------------------------------------------------------------------------------
#if GL_ES_VERSION_3_0
xxEXTERN PFNGLREADBUFFERPROC                            glReadBuffer;
xxEXTERN PFNGLDRAWRANGEELEMENTSPROC                     glDrawRangeElements;
xxEXTERN PFNGLTEXIMAGE3DPROC                            glTexImage3D;
xxEXTERN PFNGLTEXSUBIMAGE3DPROC                         glTexSubImage3D;
xxEXTERN PFNGLCOPYTEXSUBIMAGE3DPROC                     glCopyTexSubImage3D;
xxEXTERN PFNGLCOMPRESSEDTEXIMAGE3DPROC                  glCompressedTexImage3D;
xxEXTERN PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC               glCompressedTexSubImage3D;
xxEXTERN PFNGLGENQUERIESPROC                            glGenQueries;
xxEXTERN PFNGLDELETEQUERIESPROC                         glDeleteQueries;
xxEXTERN PFNGLISQUERYPROC                               glIsQuery;
xxEXTERN PFNGLBEGINQUERYPROC                            glBeginQuery;
xxEXTERN PFNGLENDQUERYPROC                              glEndQuery;
xxEXTERN PFNGLGETQUERYIVPROC                            glGetQueryiv;
xxEXTERN PFNGLGETQUERYOBJECTUIVPROC                     glGetQueryObjectuiv;
xxEXTERN PFNGLUNMAPBUFFERPROC                           glUnmapBuffer;
xxEXTERN PFNGLGETBUFFERPOINTERVPROC                     glGetBufferPointerv;
xxEXTERN PFNGLDRAWBUFFERSPROC                           glDrawBuffers;
xxEXTERN PFNGLUNIFORMMATRIX2X3FVPROC                    glUniformMatrix2x3fv;
xxEXTERN PFNGLUNIFORMMATRIX3X2FVPROC                    glUniformMatrix3x2fv;
xxEXTERN PFNGLUNIFORMMATRIX2X4FVPROC                    glUniformMatrix2x4fv;
xxEXTERN PFNGLUNIFORMMATRIX4X2FVPROC                    glUniformMatrix4x2fv;
xxEXTERN PFNGLUNIFORMMATRIX3X4FVPROC                    glUniformMatrix3x4fv;
xxEXTERN PFNGLUNIFORMMATRIX4X3FVPROC                    glUniformMatrix4x3fv;
xxEXTERN PFNGLBLITFRAMEBUFFERPROC                       glBlitFramebuffer;
xxEXTERN PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC        glRenderbufferStorageMultisample;
xxEXTERN PFNGLFRAMEBUFFERTEXTURELAYERPROC               glFramebufferTextureLayer;
xxEXTERN PFNGLMAPBUFFERRANGEPROC                        glMapBufferRange;
xxEXTERN PFNGLFLUSHMAPPEDBUFFERRANGEPROC                glFlushMappedBufferRange;
xxEXTERN PFNGLBINDVERTEXARRAYPROC                       glBindVertexArray;
xxEXTERN PFNGLDELETEVERTEXARRAYSPROC                    glDeleteVertexArrays;
xxEXTERN PFNGLGENVERTEXARRAYSPROC                       glGenVertexArrays;
xxEXTERN PFNGLISVERTEXARRAYPROC                         glIsVertexArray;
xxEXTERN PFNGLGETINTEGERI_VPROC                         glGetIntegeri_v;
xxEXTERN PFNGLBEGINTRANSFORMFEEDBACKPROC                glBeginTransformFeedback;
xxEXTERN PFNGLENDTRANSFORMFEEDBACKPROC                  glEndTransformFeedback;
xxEXTERN PFNGLBINDBUFFERRANGEPROC                       glBindBufferRange;
xxEXTERN PFNGLBINDBUFFERBASEPROC                        glBindBufferBase;
xxEXTERN PFNGLTRANSFORMFEEDBACKVARYINGSPROC             glTransformFeedbackVaryings;
xxEXTERN PFNGLGETTRANSFORMFEEDBACKVARYINGPROC           glGetTransformFeedbackVarying;
xxEXTERN PFNGLVERTEXATTRIBIPOINTERPROC                  glVertexAttribIPointer;
xxEXTERN PFNGLGETVERTEXATTRIBIIVPROC                    glGetVertexAttribIiv;
xxEXTERN PFNGLGETVERTEXATTRIBIUIVPROC                   glGetVertexAttribIuiv;
xxEXTERN PFNGLVERTEXATTRIBI4IPROC                       glVertexAttribI4i;
xxEXTERN PFNGLVERTEXATTRIBI4UIPROC                      glVertexAttribI4ui;
xxEXTERN PFNGLVERTEXATTRIBI4IVPROC                      glVertexAttribI4iv;
xxEXTERN PFNGLVERTEXATTRIBI4UIVPROC                     glVertexAttribI4uiv;
xxEXTERN PFNGLGETUNIFORMUIVPROC                         glGetUniformuiv;
xxEXTERN PFNGLGETFRAGDATALOCATIONPROC                   glGetFragDataLocation;
xxEXTERN PFNGLUNIFORM1UIPROC                            glUniform1ui;
xxEXTERN PFNGLUNIFORM2UIPROC                            glUniform2ui;
xxEXTERN PFNGLUNIFORM3UIPROC                            glUniform3ui;
xxEXTERN PFNGLUNIFORM4UIPROC                            glUniform4ui;
xxEXTERN PFNGLUNIFORM1UIVPROC                           glUniform1uiv;
xxEXTERN PFNGLUNIFORM2UIVPROC                           glUniform2uiv;
xxEXTERN PFNGLUNIFORM3UIVPROC                           glUniform3uiv;
xxEXTERN PFNGLUNIFORM4UIVPROC                           glUniform4uiv;
xxEXTERN PFNGLCLEARBUFFERIVPROC                         glClearBufferiv;
xxEXTERN PFNGLCLEARBUFFERUIVPROC                        glClearBufferuiv;
xxEXTERN PFNGLCLEARBUFFERFVPROC                         glClearBufferfv;
xxEXTERN PFNGLCLEARBUFFERFIPROC                         glClearBufferfi;
xxEXTERN PFNGLGETSTRINGIPROC                            glGetStringi;
xxEXTERN PFNGLCOPYBUFFERSUBDATAPROC                     glCopyBufferSubData;
xxEXTERN PFNGLGETUNIFORMINDICESPROC                     glGetUniformIndices;
xxEXTERN PFNGLGETACTIVEUNIFORMSIVPROC                   glGetActiveUniformsiv;
xxEXTERN PFNGLGETUNIFORMBLOCKINDEXPROC                  glGetUniformBlockIndex;
xxEXTERN PFNGLGETACTIVEUNIFORMBLOCKIVPROC               glGetActiveUniformBlockiv;
xxEXTERN PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC             glGetActiveUniformBlockName;
xxEXTERN PFNGLUNIFORMBLOCKBINDINGPROC                   glUniformBlockBinding;
xxEXTERN PFNGLDRAWARRAYSINSTANCEDPROC                   glDrawArraysInstanced;
xxEXTERN PFNGLDRAWELEMENTSINSTANCEDPROC                 glDrawElementsInstanced;
xxEXTERN PFNGLFENCESYNCPROC                             glFenceSync;
xxEXTERN PFNGLISSYNCPROC                                glIsSync;
xxEXTERN PFNGLDELETESYNCPROC                            glDeleteSync;
xxEXTERN PFNGLCLIENTWAITSYNCPROC                        glClientWaitSync;
xxEXTERN PFNGLWAITSYNCPROC                              glWaitSync;
xxEXTERN PFNGLGETINTEGER64VPROC                         glGetInteger64v;
xxEXTERN PFNGLGETSYNCIVPROC                             glGetSynciv;
xxEXTERN PFNGLGETINTEGER64I_VPROC                       glGetInteger64i_v;
xxEXTERN PFNGLGETBUFFERPARAMETERI64VPROC                glGetBufferParameteri64v;
xxEXTERN PFNGLGENSAMPLERSPROC                           glGenSamplers;
xxEXTERN PFNGLDELETESAMPLERSPROC                        glDeleteSamplers;
xxEXTERN PFNGLISSAMPLERPROC                             glIsSampler;
xxEXTERN PFNGLBINDSAMPLERPROC                           glBindSampler;
xxEXTERN PFNGLSAMPLERPARAMETERIPROC                     glSamplerParameteri;
xxEXTERN PFNGLSAMPLERPARAMETERIVPROC                    glSamplerParameteriv;
xxEXTERN PFNGLSAMPLERPARAMETERFPROC                     glSamplerParameterf;
xxEXTERN PFNGLSAMPLERPARAMETERFVPROC                    glSamplerParameterfv;
xxEXTERN PFNGLGETSAMPLERPARAMETERIVPROC                 glGetSamplerParameteriv;
xxEXTERN PFNGLGETSAMPLERPARAMETERFVPROC                 glGetSamplerParameterfv;
xxEXTERN PFNGLVERTEXATTRIBDIVISORPROC                   glVertexAttribDivisor;
xxEXTERN PFNGLBINDTRANSFORMFEEDBACKPROC                 glBindTransformFeedback;
xxEXTERN PFNGLDELETETRANSFORMFEEDBACKSPROC              glDeleteTransformFeedbacks;
xxEXTERN PFNGLGENTRANSFORMFEEDBACKSPROC                 glGenTransformFeedbacks;
xxEXTERN PFNGLISTRANSFORMFEEDBACKPROC                   glIsTransformFeedback;
xxEXTERN PFNGLPAUSETRANSFORMFEEDBACKPROC                glPauseTransformFeedback;
xxEXTERN PFNGLRESUMETRANSFORMFEEDBACKPROC               glResumeTransformFeedback;
xxEXTERN PFNGLGETPROGRAMBINARYPROC                      glGetProgramBinary;
xxEXTERN PFNGLPROGRAMBINARYPROC                         glProgramBinary;
xxEXTERN PFNGLPROGRAMPARAMETERIPROC                     glProgramParameteri;
xxEXTERN PFNGLINVALIDATEFRAMEBUFFERPROC                 glInvalidateFramebuffer;
xxEXTERN PFNGLINVALIDATESUBFRAMEBUFFERPROC              glInvalidateSubFramebuffer;
xxEXTERN PFNGLTEXSTORAGE2DPROC                          glTexStorage2D;
xxEXTERN PFNGLTEXSTORAGE3DPROC                          glTexStorage3D;
xxEXTERN PFNGLGETINTERNALFORMATIVPROC                   glGetInternalformativ;
#endif
//------------------------------------------------------------------------------
#if GL_ES_VERSION_3_1
xxEXTERN PFNGLDISPATCHCOMPUTEPROC                       glDispatchCompute;
xxEXTERN PFNGLDISPATCHCOMPUTEINDIRECTPROC               glDispatchComputeIndirect;
xxEXTERN PFNGLDRAWARRAYSINDIRECTPROC                    glDrawArraysIndirect;
xxEXTERN PFNGLDRAWELEMENTSINDIRECTPROC                  glDrawElementsIndirect;
xxEXTERN PFNGLFRAMEBUFFERPARAMETERIPROC                 glFramebufferParameteri;
xxEXTERN PFNGLGETFRAMEBUFFERPARAMETERIVPROC             glGetFramebufferParameteriv;
xxEXTERN PFNGLGETPROGRAMINTERFACEIVPROC                 glGetProgramInterfaceiv;
xxEXTERN PFNGLGETPROGRAMRESOURCEINDEXPROC               glGetProgramResourceIndex;
xxEXTERN PFNGLGETPROGRAMRESOURCENAMEPROC                glGetProgramResourceName;
xxEXTERN PFNGLGETPROGRAMRESOURCEIVPROC                  glGetProgramResourceiv;
xxEXTERN PFNGLGETPROGRAMRESOURCELOCATIONPROC            glGetProgramResourceLocation;
xxEXTERN PFNGLUSEPROGRAMSTAGESPROC                      glUseProgramStages;
xxEXTERN PFNGLACTIVESHADERPROGRAMPROC                   glActiveShaderProgram;
xxEXTERN PFNGLCREATESHADERPROGRAMVPROC                  glCreateShaderProgramv;
xxEXTERN PFNGLBINDPROGRAMPIPELINEPROC                   glBindProgramPipeline;
xxEXTERN PFNGLDELETEPROGRAMPIPELINESPROC                glDeleteProgramPipelines;
xxEXTERN PFNGLGENPROGRAMPIPELINESPROC                   glGenProgramPipelines;
xxEXTERN PFNGLISPROGRAMPIPELINEPROC                     glIsProgramPipeline;
xxEXTERN PFNGLGETPROGRAMPIPELINEIVPROC                  glGetProgramPipelineiv;
xxEXTERN PFNGLPROGRAMUNIFORM1IPROC                      glProgramUniform1i;
xxEXTERN PFNGLPROGRAMUNIFORM2IPROC                      glProgramUniform2i;
xxEXTERN PFNGLPROGRAMUNIFORM3IPROC                      glProgramUniform3i;
xxEXTERN PFNGLPROGRAMUNIFORM4IPROC                      glProgramUniform4i;
xxEXTERN PFNGLPROGRAMUNIFORM1UIPROC                     glProgramUniform1ui;
xxEXTERN PFNGLPROGRAMUNIFORM2UIPROC                     glProgramUniform2ui;
xxEXTERN PFNGLPROGRAMUNIFORM3UIPROC                     glProgramUniform3ui;
xxEXTERN PFNGLPROGRAMUNIFORM4UIPROC                     glProgramUniform4ui;
xxEXTERN PFNGLPROGRAMUNIFORM1FPROC                      glProgramUniform1f;
xxEXTERN PFNGLPROGRAMUNIFORM2FPROC                      glProgramUniform2f;
xxEXTERN PFNGLPROGRAMUNIFORM3FPROC                      glProgramUniform3f;
xxEXTERN PFNGLPROGRAMUNIFORM4FPROC                      glProgramUniform4f;
xxEXTERN PFNGLPROGRAMUNIFORM1IVPROC                     glProgramUniform1iv;
xxEXTERN PFNGLPROGRAMUNIFORM2IVPROC                     glProgramUniform2iv;
xxEXTERN PFNGLPROGRAMUNIFORM3IVPROC                     glProgramUniform3iv;
xxEXTERN PFNGLPROGRAMUNIFORM4IVPROC                     glProgramUniform4iv;
xxEXTERN PFNGLPROGRAMUNIFORM1UIVPROC                    glProgramUniform1uiv;
xxEXTERN PFNGLPROGRAMUNIFORM2UIVPROC                    glProgramUniform2uiv;
xxEXTERN PFNGLPROGRAMUNIFORM3UIVPROC                    glProgramUniform3uiv;
xxEXTERN PFNGLPROGRAMUNIFORM4UIVPROC                    glProgramUniform4uiv;
xxEXTERN PFNGLPROGRAMUNIFORM1FVPROC                     glProgramUniform1fv;
xxEXTERN PFNGLPROGRAMUNIFORM2FVPROC                     glProgramUniform2fv;
xxEXTERN PFNGLPROGRAMUNIFORM3FVPROC                     glProgramUniform3fv;
xxEXTERN PFNGLPROGRAMUNIFORM4FVPROC                     glProgramUniform4fv;
xxEXTERN PFNGLPROGRAMUNIFORMMATRIX2FVPROC               glProgramUniformMatrix2fv;
xxEXTERN PFNGLPROGRAMUNIFORMMATRIX3FVPROC               glProgramUniformMatrix3fv;
xxEXTERN PFNGLPROGRAMUNIFORMMATRIX4FVPROC               glProgramUniformMatrix4fv;
xxEXTERN PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC             glProgramUniformMatrix2x3fv;
xxEXTERN PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC             glProgramUniformMatrix3x2fv;
xxEXTERN PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC             glProgramUniformMatrix2x4fv;
xxEXTERN PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC             glProgramUniformMatrix4x2fv;
xxEXTERN PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC             glProgramUniformMatrix3x4fv;
xxEXTERN PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC             glProgramUniformMatrix4x3fv;
xxEXTERN PFNGLVALIDATEPROGRAMPIPELINEPROC               glValidateProgramPipeline;
xxEXTERN PFNGLGETPROGRAMPIPELINEINFOLOGPROC             glGetProgramPipelineInfoLog;
xxEXTERN PFNGLBINDIMAGETEXTUREPROC                      glBindImageTexture;
xxEXTERN PFNGLGETBOOLEANI_VPROC                         glGetBooleani_v;
xxEXTERN PFNGLMEMORYBARRIERPROC                         glMemoryBarrier;
xxEXTERN PFNGLMEMORYBARRIERBYREGIONPROC                 glMemoryBarrierByRegion;
xxEXTERN PFNGLTEXSTORAGE2DMULTISAMPLEPROC               glTexStorage2DMultisample;
xxEXTERN PFNGLGETMULTISAMPLEFVPROC                      glGetMultisamplefv;
xxEXTERN PFNGLSAMPLEMASKIPROC                           glSampleMaski;
xxEXTERN PFNGLGETTEXLEVELPARAMETERIVPROC                glGetTexLevelParameteriv;
xxEXTERN PFNGLGETTEXLEVELPARAMETERFVPROC                glGetTexLevelParameterfv;
xxEXTERN PFNGLBINDVERTEXBUFFERPROC                      glBindVertexBuffer;
xxEXTERN PFNGLVERTEXATTRIBFORMATPROC                    glVertexAttribFormat;
xxEXTERN PFNGLVERTEXATTRIBIFORMATPROC                   glVertexAttribIFormat;
xxEXTERN PFNGLVERTEXATTRIBBINDINGPROC                   glVertexAttribBinding;
xxEXTERN PFNGLVERTEXBINDINGDIVISORPROC                  glVertexBindingDivisor;
#endif
//------------------------------------------------------------------------------
#if GL_ES_VERSION_3_2
xxEXTERN PFNGLBLENDBARRIERPROC                          glBlendBarrier;
xxEXTERN PFNGLCOPYIMAGESUBDATAPROC                      glCopyImageSubData;
xxEXTERN PFNGLDEBUGMESSAGECONTROLPROC                   glDebugMessageControl;
xxEXTERN PFNGLDEBUGMESSAGEINSERTPROC                    glDebugMessageInsert;
xxEXTERN PFNGLDEBUGMESSAGECALLBACKPROC                  glDebugMessageCallback;
xxEXTERN PFNGLGETDEBUGMESSAGELOGPROC                    glGetDebugMessageLog;
xxEXTERN PFNGLPUSHDEBUGGROUPPROC                        glPushDebugGroup;
xxEXTERN PFNGLPOPDEBUGGROUPPROC                         glPopDebugGroup;
xxEXTERN PFNGLOBJECTLABELPROC                           glObjectLabel;
xxEXTERN PFNGLGETOBJECTLABELPROC                        glGetObjectLabel;
xxEXTERN PFNGLOBJECTPTRLABELPROC                        glObjectPtrLabel;
xxEXTERN PFNGLGETOBJECTPTRLABELPROC                     glGetObjectPtrLabel;
xxEXTERN PFNGLGETPOINTERVPROC                           glGetPointerv;
xxEXTERN PFNGLENABLEIPROC                               glEnablei;
xxEXTERN PFNGLDISABLEIPROC                              glDisablei;
xxEXTERN PFNGLBLENDEQUATIONIPROC                        glBlendEquationi;
xxEXTERN PFNGLBLENDEQUATIONSEPARATEIPROC                glBlendEquationSeparatei;
xxEXTERN PFNGLBLENDFUNCIPROC                            glBlendFunci;
xxEXTERN PFNGLBLENDFUNCSEPARATEIPROC                    glBlendFuncSeparatei;
xxEXTERN PFNGLCOLORMASKIPROC                            glColorMaski;
xxEXTERN PFNGLISENABLEDIPROC                            glIsEnabledi;
xxEXTERN PFNGLDRAWELEMENTSBASEVERTEXPROC                glDrawElementsBaseVertex;
xxEXTERN PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC           glDrawRangeElementsBaseVertex;
xxEXTERN PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC       glDrawElementsInstancedBaseVertex;
xxEXTERN PFNGLFRAMEBUFFERTEXTUREPROC                    glFramebufferTexture;
xxEXTERN PFNGLPRIMITIVEBOUNDINGBOXPROC                  glPrimitiveBoundingBox;
xxEXTERN PFNGLGETGRAPHICSRESETSTATUSPROC                glGetGraphicsResetStatus;
xxEXTERN PFNGLREADNPIXELSPROC                           glReadnPixels;
xxEXTERN PFNGLGETNUNIFORMFVPROC                         glGetnUniformfv;
xxEXTERN PFNGLGETNUNIFORMIVPROC                         glGetnUniformiv;
xxEXTERN PFNGLGETNUNIFORMUIVPROC                        glGetnUniformuiv;
xxEXTERN PFNGLMINSAMPLESHADINGPROC                      glMinSampleShading;
xxEXTERN PFNGLPATCHPARAMETERIPROC                       glPatchParameteri;
xxEXTERN PFNGLTEXPARAMETERIIVPROC                       glTexParameterIiv;
xxEXTERN PFNGLTEXPARAMETERIUIVPROC                      glTexParameterIuiv;
xxEXTERN PFNGLGETTEXPARAMETERIIVPROC                    glGetTexParameterIiv;
xxEXTERN PFNGLGETTEXPARAMETERIUIVPROC                   glGetTexParameterIuiv;
xxEXTERN PFNGLSAMPLERPARAMETERIIVPROC                   glSamplerParameterIiv;
xxEXTERN PFNGLSAMPLERPARAMETERIUIVPROC                  glSamplerParameterIuiv;
xxEXTERN PFNGLGETSAMPLERPARAMETERIIVPROC                glGetSamplerParameterIiv;
xxEXTERN PFNGLGETSAMPLERPARAMETERIUIVPROC               glGetSamplerParameterIuiv;
xxEXTERN PFNGLTEXBUFFERPROC                             glTexBuffer;
xxEXTERN PFNGLTEXBUFFERRANGEPROC                        glTexBufferRange;
xxEXTERN PFNGLTEXSTORAGE3DMULTISAMPLEPROC               glTexStorage3DMultisample;
#endif
//------------------------------------------------------------------------------
