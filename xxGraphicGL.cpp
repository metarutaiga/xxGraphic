//==============================================================================
// xxGraphic : OpenGL Function Source
//
// Copyright (c) 2019 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "internal/xxGraphicInternal.h"
#include "xxGraphicGL.h"

//==============================================================================
//  Initialize
//==============================================================================
bool xxGraphicCreateGLES2(void* (GL_APIENTRYP getSymbol)(const char* name, bool* failed))
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
bool xxGraphicCreateGLES3(void* (GL_APIENTRYP getSymbol)(const char* name, bool* failed))
{
    bool failed = xxGraphicCreateGLES2(getSymbol) == false;

#define symbol(var) (void*&)var = getSymbol(#var, &failed);
    symbol(glReadBuffer);
    symbol(glDrawRangeElements);
    symbol(glTexImage3D);
    symbol(glTexSubImage3D);
    symbol(glCopyTexSubImage3D);
    symbol(glCompressedTexImage3D);
    symbol(glCompressedTexSubImage3D);
    symbol(glGenQueries);
    symbol(glDeleteQueries);
    symbol(glIsQuery);
    symbol(glBeginQuery);
    symbol(glEndQuery);
    symbol(glGetQueryiv);
    symbol(glGetQueryObjectuiv);
    symbol(glUnmapBuffer);
    symbol(glGetBufferPointerv);
    symbol(glDrawBuffers);
    symbol(glUniformMatrix2x3fv);
    symbol(glUniformMatrix3x2fv);
    symbol(glUniformMatrix2x4fv);
    symbol(glUniformMatrix4x2fv);
    symbol(glUniformMatrix3x4fv);
    symbol(glUniformMatrix4x3fv);
    symbol(glBlitFramebuffer);
    symbol(glRenderbufferStorageMultisample);
    symbol(glFramebufferTextureLayer);
    symbol(glMapBufferRange);
    symbol(glFlushMappedBufferRange);
    symbol(glBindVertexArray);
    symbol(glDeleteVertexArrays);
    symbol(glGenVertexArrays);
    symbol(glIsVertexArray);
    symbol(glGetIntegeri_v);
    symbol(glBeginTransformFeedback);
    symbol(glEndTransformFeedback);
    symbol(glBindBufferRange);
    symbol(glBindBufferBase);
    symbol(glTransformFeedbackVaryings);
    symbol(glGetTransformFeedbackVarying);
    symbol(glVertexAttribIPointer);
    symbol(glGetVertexAttribIiv);
    symbol(glGetVertexAttribIuiv);
    symbol(glVertexAttribI4i);
    symbol(glVertexAttribI4ui);
    symbol(glVertexAttribI4iv);
    symbol(glVertexAttribI4uiv);
    symbol(glGetUniformuiv);
    symbol(glGetFragDataLocation);
    symbol(glUniform1ui);
    symbol(glUniform2ui);
    symbol(glUniform3ui);
    symbol(glUniform4ui);
    symbol(glUniform1uiv);
    symbol(glUniform2uiv);
    symbol(glUniform3uiv);
    symbol(glUniform4uiv);
    symbol(glClearBufferiv);
    symbol(glClearBufferuiv);
    symbol(glClearBufferfv);
    symbol(glClearBufferfi);
    symbol(glGetStringi);
    symbol(glCopyBufferSubData);
    symbol(glGetUniformIndices);
    symbol(glGetActiveUniformsiv);
    symbol(glGetUniformBlockIndex);
    symbol(glGetActiveUniformBlockiv);
    symbol(glGetActiveUniformBlockName);
    symbol(glUniformBlockBinding);
    symbol(glDrawArraysInstanced);
    symbol(glDrawElementsInstanced);
    symbol(glFenceSync);
    symbol(glIsSync);
    symbol(glDeleteSync);
    symbol(glClientWaitSync);
    symbol(glWaitSync);
    symbol(glGetInteger64v);
    symbol(glGetSynciv);
    symbol(glGetInteger64i_v);
    symbol(glGetBufferParameteri64v);
    symbol(glGenSamplers);
    symbol(glDeleteSamplers);
    symbol(glIsSampler);
    symbol(glBindSampler);
    symbol(glSamplerParameteri);
    symbol(glSamplerParameteriv);
    symbol(glSamplerParameterf);
    symbol(glSamplerParameterfv);
    symbol(glGetSamplerParameteriv);
    symbol(glGetSamplerParameterfv);
    symbol(glVertexAttribDivisor);
    symbol(glBindTransformFeedback);
    symbol(glDeleteTransformFeedbacks);
    symbol(glGenTransformFeedbacks);
    symbol(glIsTransformFeedback);
    symbol(glPauseTransformFeedback);
    symbol(glResumeTransformFeedback);
    symbol(glGetProgramBinary);
    symbol(glProgramBinary);
    symbol(glProgramParameteri);
    symbol(glInvalidateFramebuffer);
    symbol(glInvalidateSubFramebuffer);
    symbol(glTexStorage2D);
    symbol(glTexStorage3D);
    symbol(glGetInternalformativ);
#undef symbol

    return (failed == false);
}
//------------------------------------------------------------------------------
bool xxGraphicCreateGLES31(void* (GL_APIENTRYP getSymbol)(const char* name, bool* failed))
{
    bool failed = xxGraphicCreateGLES3(getSymbol) == false;

#define symbol(var) (void*&)var = getSymbol(#var, &failed);
    symbol(glDispatchCompute);
    symbol(glDispatchComputeIndirect);
    symbol(glDrawArraysIndirect);
    symbol(glDrawElementsIndirect);
    symbol(glFramebufferParameteri);
    symbol(glGetFramebufferParameteriv);
    symbol(glGetProgramInterfaceiv);
    symbol(glGetProgramResourceIndex);
    symbol(glGetProgramResourceName);
    symbol(glGetProgramResourceiv);
    symbol(glGetProgramResourceLocation);
    symbol(glUseProgramStages);
    symbol(glActiveShaderProgram);
    symbol(glCreateShaderProgramv);
    symbol(glBindProgramPipeline);
    symbol(glDeleteProgramPipelines);
    symbol(glGenProgramPipelines);
    symbol(glIsProgramPipeline);
    symbol(glGetProgramPipelineiv);
    symbol(glProgramUniform1i);
    symbol(glProgramUniform2i);
    symbol(glProgramUniform3i);
    symbol(glProgramUniform4i);
    symbol(glProgramUniform1ui);
    symbol(glProgramUniform2ui);
    symbol(glProgramUniform3ui);
    symbol(glProgramUniform4ui);
    symbol(glProgramUniform1f);
    symbol(glProgramUniform2f);
    symbol(glProgramUniform3f);
    symbol(glProgramUniform4f);
    symbol(glProgramUniform1iv);
    symbol(glProgramUniform2iv);
    symbol(glProgramUniform3iv);
    symbol(glProgramUniform4iv);
    symbol(glProgramUniform1uiv);
    symbol(glProgramUniform2uiv);
    symbol(glProgramUniform3uiv);
    symbol(glProgramUniform4uiv);
    symbol(glProgramUniform1fv);
    symbol(glProgramUniform2fv);
    symbol(glProgramUniform3fv);
    symbol(glProgramUniform4fv);
    symbol(glProgramUniformMatrix2fv);
    symbol(glProgramUniformMatrix3fv);
    symbol(glProgramUniformMatrix4fv);
    symbol(glProgramUniformMatrix2x3fv);
    symbol(glProgramUniformMatrix3x2fv);
    symbol(glProgramUniformMatrix2x4fv);
    symbol(glProgramUniformMatrix4x2fv);
    symbol(glProgramUniformMatrix3x4fv);
    symbol(glProgramUniformMatrix4x3fv);
    symbol(glValidateProgramPipeline);
    symbol(glGetProgramPipelineInfoLog);
    symbol(glBindImageTexture);
    symbol(glGetBooleani_v);
    symbol(glMemoryBarrier);
    symbol(glMemoryBarrierByRegion);
    symbol(glTexStorage2DMultisample);
    symbol(glGetMultisamplefv);
    symbol(glSampleMaski);
    symbol(glGetTexLevelParameteriv);
    symbol(glGetTexLevelParameterfv);
    symbol(glBindVertexBuffer);
    symbol(glVertexAttribFormat);
    symbol(glVertexAttribIFormat);
    symbol(glVertexAttribBinding);
    symbol(glVertexBindingDivisor);
#undef symbol

    return (failed == false);
}
//------------------------------------------------------------------------------
bool xxGraphicCreateGLES32(void* (GL_APIENTRYP getSymbol)(const char* name, bool* failed))
{
    bool failed = xxGraphicCreateGLES31(getSymbol) == false;

#define symbol(var) (void*&)var = getSymbol(#var, &failed);
    symbol(glBlendBarrier);
    symbol(glCopyImageSubData);
    symbol(glDebugMessageControl);
    symbol(glDebugMessageInsert);
    symbol(glDebugMessageCallback);
    symbol(glGetDebugMessageLog);
    symbol(glPushDebugGroup);
    symbol(glPopDebugGroup);
    symbol(glObjectLabel);
    symbol(glGetObjectLabel);
    symbol(glObjectPtrLabel);
    symbol(glGetObjectPtrLabel);
    symbol(glGetPointerv);
    symbol(glEnablei);
    symbol(glDisablei);
    symbol(glBlendEquationi);
    symbol(glBlendEquationSeparatei);
    symbol(glBlendFunci);
    symbol(glBlendFuncSeparatei);
    symbol(glColorMaski);
    symbol(glIsEnabledi);
    symbol(glDrawElementsBaseVertex);
    symbol(glDrawRangeElementsBaseVertex);
    symbol(glDrawElementsInstancedBaseVertex);
    symbol(glFramebufferTexture);
    symbol(glPrimitiveBoundingBox);
    symbol(glGetGraphicsResetStatus);
    symbol(glReadnPixels);
    symbol(glGetnUniformfv);
    symbol(glGetnUniformiv);
    symbol(glGetnUniformuiv);
    symbol(glMinSampleShading);
    symbol(glPatchParameteri);
    symbol(glTexParameterIiv);
    symbol(glTexParameterIuiv);
    symbol(glGetTexParameterIiv);
    symbol(glGetTexParameterIuiv);
    symbol(glSamplerParameterIiv);
    symbol(glSamplerParameterIuiv);
    symbol(glGetSamplerParameterIiv);
    symbol(glGetSamplerParameterIuiv);
    symbol(glTexBuffer);
    symbol(glTexBufferRange);
    symbol(glTexStorage3DMultisample);
#undef symbol

    return (failed == false);
}
//------------------------------------------------------------------------------
static void* GL_APIENTRY getNULLSymbol(const char* name, bool* failed)
{
    return nullptr;
}
//------------------------------------------------------------------------------
void xxGraphicDestroyGL()
{
    xxGraphicCreateGLES2(getNULLSymbol);
    xxGraphicCreateGLES3(getNULLSymbol);
    xxGraphicCreateGLES31(getNULLSymbol);
}
//==============================================================================
//  GLSL Shader
//==============================================================================
const char* const vertexShaderCode = 
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
const char* const fragmentShaderCode =
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
//------------------------------------------------------------------------------
PFNGLREADBUFFERPROC                             glReadBuffer;
PFNGLDRAWRANGEELEMENTSPROC                      glDrawRangeElements;
PFNGLTEXIMAGE3DPROC                             glTexImage3D;
PFNGLTEXSUBIMAGE3DPROC                          glTexSubImage3D;
PFNGLCOPYTEXSUBIMAGE3DPROC                      glCopyTexSubImage3D;
PFNGLCOMPRESSEDTEXIMAGE3DPROC                   glCompressedTexImage3D;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC                glCompressedTexSubImage3D;
PFNGLGENQUERIESPROC                             glGenQueries;
PFNGLDELETEQUERIESPROC                          glDeleteQueries;
PFNGLISQUERYPROC                                glIsQuery;
PFNGLBEGINQUERYPROC                             glBeginQuery;
PFNGLENDQUERYPROC                               glEndQuery;
PFNGLGETQUERYIVPROC                             glGetQueryiv;
PFNGLGETQUERYOBJECTUIVPROC                      glGetQueryObjectuiv;
PFNGLUNMAPBUFFERPROC                            glUnmapBuffer;
PFNGLGETBUFFERPOINTERVPROC                      glGetBufferPointerv;
PFNGLDRAWBUFFERSPROC                            glDrawBuffers;
PFNGLUNIFORMMATRIX2X3FVPROC                     glUniformMatrix2x3fv;
PFNGLUNIFORMMATRIX3X2FVPROC                     glUniformMatrix3x2fv;
PFNGLUNIFORMMATRIX2X4FVPROC                     glUniformMatrix2x4fv;
PFNGLUNIFORMMATRIX4X2FVPROC                     glUniformMatrix4x2fv;
PFNGLUNIFORMMATRIX3X4FVPROC                     glUniformMatrix3x4fv;
PFNGLUNIFORMMATRIX4X3FVPROC                     glUniformMatrix4x3fv;
PFNGLBLITFRAMEBUFFERPROC                        glBlitFramebuffer;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC         glRenderbufferStorageMultisample;
PFNGLFRAMEBUFFERTEXTURELAYERPROC                glFramebufferTextureLayer;
PFNGLMAPBUFFERRANGEPROC                         glMapBufferRange;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC                 glFlushMappedBufferRange;
PFNGLBINDVERTEXARRAYPROC                        glBindVertexArray;
PFNGLDELETEVERTEXARRAYSPROC                     glDeleteVertexArrays;
PFNGLGENVERTEXARRAYSPROC                        glGenVertexArrays;
PFNGLISVERTEXARRAYPROC                          glIsVertexArray;
PFNGLGETINTEGERI_VPROC                          glGetIntegeri_v;
PFNGLBEGINTRANSFORMFEEDBACKPROC                 glBeginTransformFeedback;
PFNGLENDTRANSFORMFEEDBACKPROC                   glEndTransformFeedback;
PFNGLBINDBUFFERRANGEPROC                        glBindBufferRange;
PFNGLBINDBUFFERBASEPROC                         glBindBufferBase;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC              glTransformFeedbackVaryings;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC            glGetTransformFeedbackVarying;
PFNGLVERTEXATTRIBIPOINTERPROC                   glVertexAttribIPointer;
PFNGLGETVERTEXATTRIBIIVPROC                     glGetVertexAttribIiv;
PFNGLGETVERTEXATTRIBIUIVPROC                    glGetVertexAttribIuiv;
PFNGLVERTEXATTRIBI4IPROC                        glVertexAttribI4i;
PFNGLVERTEXATTRIBI4UIPROC                       glVertexAttribI4ui;
PFNGLVERTEXATTRIBI4IVPROC                       glVertexAttribI4iv;
PFNGLVERTEXATTRIBI4UIVPROC                      glVertexAttribI4uiv;
PFNGLGETUNIFORMUIVPROC                          glGetUniformuiv;
PFNGLGETFRAGDATALOCATIONPROC                    glGetFragDataLocation;
PFNGLUNIFORM1UIPROC                             glUniform1ui;
PFNGLUNIFORM2UIPROC                             glUniform2ui;
PFNGLUNIFORM3UIPROC                             glUniform3ui;
PFNGLUNIFORM4UIPROC                             glUniform4ui;
PFNGLUNIFORM1UIVPROC                            glUniform1uiv;
PFNGLUNIFORM2UIVPROC                            glUniform2uiv;
PFNGLUNIFORM3UIVPROC                            glUniform3uiv;
PFNGLUNIFORM4UIVPROC                            glUniform4uiv;
PFNGLCLEARBUFFERIVPROC                          glClearBufferiv;
PFNGLCLEARBUFFERUIVPROC                         glClearBufferuiv;
PFNGLCLEARBUFFERFVPROC                          glClearBufferfv;
PFNGLCLEARBUFFERFIPROC                          glClearBufferfi;
PFNGLGETSTRINGIPROC                             glGetStringi;
PFNGLCOPYBUFFERSUBDATAPROC                      glCopyBufferSubData;
PFNGLGETUNIFORMINDICESPROC                      glGetUniformIndices;
PFNGLGETACTIVEUNIFORMSIVPROC                    glGetActiveUniformsiv;
PFNGLGETUNIFORMBLOCKINDEXPROC                   glGetUniformBlockIndex;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC                glGetActiveUniformBlockiv;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC              glGetActiveUniformBlockName;
PFNGLUNIFORMBLOCKBINDINGPROC                    glUniformBlockBinding;
PFNGLDRAWARRAYSINSTANCEDPROC                    glDrawArraysInstanced;
PFNGLDRAWELEMENTSINSTANCEDPROC                  glDrawElementsInstanced;
PFNGLFENCESYNCPROC                              glFenceSync;
PFNGLISSYNCPROC                                 glIsSync;
PFNGLDELETESYNCPROC                             glDeleteSync;
PFNGLCLIENTWAITSYNCPROC                         glClientWaitSync;
PFNGLWAITSYNCPROC                               glWaitSync;
PFNGLGETINTEGER64VPROC                          glGetInteger64v;
PFNGLGETSYNCIVPROC                              glGetSynciv;
PFNGLGETINTEGER64I_VPROC                        glGetInteger64i_v;
PFNGLGETBUFFERPARAMETERI64VPROC                 glGetBufferParameteri64v;
PFNGLGENSAMPLERSPROC                            glGenSamplers;
PFNGLDELETESAMPLERSPROC                         glDeleteSamplers;
PFNGLISSAMPLERPROC                              glIsSampler;
PFNGLBINDSAMPLERPROC                            glBindSampler;
PFNGLSAMPLERPARAMETERIPROC                      glSamplerParameteri;
PFNGLSAMPLERPARAMETERIVPROC                     glSamplerParameteriv;
PFNGLSAMPLERPARAMETERFPROC                      glSamplerParameterf;
PFNGLSAMPLERPARAMETERFVPROC                     glSamplerParameterfv;
PFNGLGETSAMPLERPARAMETERIVPROC                  glGetSamplerParameteriv;
PFNGLGETSAMPLERPARAMETERFVPROC                  glGetSamplerParameterfv;
PFNGLVERTEXATTRIBDIVISORPROC                    glVertexAttribDivisor;
PFNGLBINDTRANSFORMFEEDBACKPROC                  glBindTransformFeedback;
PFNGLDELETETRANSFORMFEEDBACKSPROC               glDeleteTransformFeedbacks;
PFNGLGENTRANSFORMFEEDBACKSPROC                  glGenTransformFeedbacks;
PFNGLISTRANSFORMFEEDBACKPROC                    glIsTransformFeedback;
PFNGLPAUSETRANSFORMFEEDBACKPROC                 glPauseTransformFeedback;
PFNGLRESUMETRANSFORMFEEDBACKPROC                glResumeTransformFeedback;
PFNGLGETPROGRAMBINARYPROC                       glGetProgramBinary;
PFNGLPROGRAMBINARYPROC                          glProgramBinary;
PFNGLPROGRAMPARAMETERIPROC                      glProgramParameteri;
PFNGLINVALIDATEFRAMEBUFFERPROC                  glInvalidateFramebuffer;
PFNGLINVALIDATESUBFRAMEBUFFERPROC               glInvalidateSubFramebuffer;
PFNGLTEXSTORAGE2DPROC                           glTexStorage2D;
PFNGLTEXSTORAGE3DPROC                           glTexStorage3D;
PFNGLGETINTERNALFORMATIVPROC                    glGetInternalformativ;
//------------------------------------------------------------------------------
PFNGLDISPATCHCOMPUTEPROC                        glDispatchCompute;
PFNGLDISPATCHCOMPUTEINDIRECTPROC                glDispatchComputeIndirect;
PFNGLDRAWARRAYSINDIRECTPROC                     glDrawArraysIndirect;
PFNGLDRAWELEMENTSINDIRECTPROC                   glDrawElementsIndirect;
PFNGLFRAMEBUFFERPARAMETERIPROC                  glFramebufferParameteri;
PFNGLGETFRAMEBUFFERPARAMETERIVPROC              glGetFramebufferParameteriv;
PFNGLGETPROGRAMINTERFACEIVPROC                  glGetProgramInterfaceiv;
PFNGLGETPROGRAMRESOURCEINDEXPROC                glGetProgramResourceIndex;
PFNGLGETPROGRAMRESOURCENAMEPROC                 glGetProgramResourceName;
PFNGLGETPROGRAMRESOURCEIVPROC                   glGetProgramResourceiv;
PFNGLGETPROGRAMRESOURCELOCATIONPROC             glGetProgramResourceLocation;
PFNGLUSEPROGRAMSTAGESPROC                       glUseProgramStages;
PFNGLACTIVESHADERPROGRAMPROC                    glActiveShaderProgram;
PFNGLCREATESHADERPROGRAMVPROC                   glCreateShaderProgramv;
PFNGLBINDPROGRAMPIPELINEPROC                    glBindProgramPipeline;
PFNGLDELETEPROGRAMPIPELINESPROC                 glDeleteProgramPipelines;
PFNGLGENPROGRAMPIPELINESPROC                    glGenProgramPipelines;
PFNGLISPROGRAMPIPELINEPROC                      glIsProgramPipeline;
PFNGLGETPROGRAMPIPELINEIVPROC                   glGetProgramPipelineiv;
PFNGLPROGRAMUNIFORM1IPROC                       glProgramUniform1i;
PFNGLPROGRAMUNIFORM2IPROC                       glProgramUniform2i;
PFNGLPROGRAMUNIFORM3IPROC                       glProgramUniform3i;
PFNGLPROGRAMUNIFORM4IPROC                       glProgramUniform4i;
PFNGLPROGRAMUNIFORM1UIPROC                      glProgramUniform1ui;
PFNGLPROGRAMUNIFORM2UIPROC                      glProgramUniform2ui;
PFNGLPROGRAMUNIFORM3UIPROC                      glProgramUniform3ui;
PFNGLPROGRAMUNIFORM4UIPROC                      glProgramUniform4ui;
PFNGLPROGRAMUNIFORM1FPROC                       glProgramUniform1f;
PFNGLPROGRAMUNIFORM2FPROC                       glProgramUniform2f;
PFNGLPROGRAMUNIFORM3FPROC                       glProgramUniform3f;
PFNGLPROGRAMUNIFORM4FPROC                       glProgramUniform4f;
PFNGLPROGRAMUNIFORM1IVPROC                      glProgramUniform1iv;
PFNGLPROGRAMUNIFORM2IVPROC                      glProgramUniform2iv;
PFNGLPROGRAMUNIFORM3IVPROC                      glProgramUniform3iv;
PFNGLPROGRAMUNIFORM4IVPROC                      glProgramUniform4iv;
PFNGLPROGRAMUNIFORM1UIVPROC                     glProgramUniform1uiv;
PFNGLPROGRAMUNIFORM2UIVPROC                     glProgramUniform2uiv;
PFNGLPROGRAMUNIFORM3UIVPROC                     glProgramUniform3uiv;
PFNGLPROGRAMUNIFORM4UIVPROC                     glProgramUniform4uiv;
PFNGLPROGRAMUNIFORM1FVPROC                      glProgramUniform1fv;
PFNGLPROGRAMUNIFORM2FVPROC                      glProgramUniform2fv;
PFNGLPROGRAMUNIFORM3FVPROC                      glProgramUniform3fv;
PFNGLPROGRAMUNIFORM4FVPROC                      glProgramUniform4fv;
PFNGLPROGRAMUNIFORMMATRIX2FVPROC                glProgramUniformMatrix2fv;
PFNGLPROGRAMUNIFORMMATRIX3FVPROC                glProgramUniformMatrix3fv;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC                glProgramUniformMatrix4fv;
PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC              glProgramUniformMatrix2x3fv;
PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC              glProgramUniformMatrix3x2fv;
PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC              glProgramUniformMatrix2x4fv;
PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC              glProgramUniformMatrix4x2fv;
PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC              glProgramUniformMatrix3x4fv;
PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC              glProgramUniformMatrix4x3fv;
PFNGLVALIDATEPROGRAMPIPELINEPROC                glValidateProgramPipeline;
PFNGLGETPROGRAMPIPELINEINFOLOGPROC              glGetProgramPipelineInfoLog;
PFNGLBINDIMAGETEXTUREPROC                       glBindImageTexture;
PFNGLGETBOOLEANI_VPROC                          glGetBooleani_v;
PFNGLMEMORYBARRIERPROC                          glMemoryBarrier;
PFNGLMEMORYBARRIERBYREGIONPROC                  glMemoryBarrierByRegion;
PFNGLTEXSTORAGE2DMULTISAMPLEPROC                glTexStorage2DMultisample;
PFNGLGETMULTISAMPLEFVPROC                       glGetMultisamplefv;
PFNGLSAMPLEMASKIPROC                            glSampleMaski;
PFNGLGETTEXLEVELPARAMETERIVPROC                 glGetTexLevelParameteriv;
PFNGLGETTEXLEVELPARAMETERFVPROC                 glGetTexLevelParameterfv;
PFNGLBINDVERTEXBUFFERPROC                       glBindVertexBuffer;
PFNGLVERTEXATTRIBFORMATPROC                     glVertexAttribFormat;
PFNGLVERTEXATTRIBIFORMATPROC                    glVertexAttribIFormat;
PFNGLVERTEXATTRIBBINDINGPROC                    glVertexAttribBinding;
PFNGLVERTEXBINDINGDIVISORPROC                   glVertexBindingDivisor;
//------------------------------------------------------------------------------
PFNGLBLENDBARRIERPROC                           glBlendBarrier;
PFNGLCOPYIMAGESUBDATAPROC                       glCopyImageSubData;
PFNGLDEBUGMESSAGECONTROLPROC                    glDebugMessageControl;
PFNGLDEBUGMESSAGEINSERTPROC                     glDebugMessageInsert;
PFNGLDEBUGMESSAGECALLBACKPROC                   glDebugMessageCallback;
PFNGLGETDEBUGMESSAGELOGPROC                     glGetDebugMessageLog;
PFNGLPUSHDEBUGGROUPPROC                         glPushDebugGroup;
PFNGLPOPDEBUGGROUPPROC                          glPopDebugGroup;
PFNGLOBJECTLABELPROC                            glObjectLabel;
PFNGLGETOBJECTLABELPROC                         glGetObjectLabel;
PFNGLOBJECTPTRLABELPROC                         glObjectPtrLabel;
PFNGLGETOBJECTPTRLABELPROC                      glGetObjectPtrLabel;
PFNGLGETPOINTERVPROC                            glGetPointerv;
PFNGLENABLEIPROC                                glEnablei;
PFNGLDISABLEIPROC                               glDisablei;
PFNGLBLENDEQUATIONIPROC                         glBlendEquationi;
PFNGLBLENDEQUATIONSEPARATEIPROC                 glBlendEquationSeparatei;
PFNGLBLENDFUNCIPROC                             glBlendFunci;
PFNGLBLENDFUNCSEPARATEIPROC                     glBlendFuncSeparatei;
PFNGLCOLORMASKIPROC                             glColorMaski;
PFNGLISENABLEDIPROC                             glIsEnabledi;
PFNGLDRAWELEMENTSBASEVERTEXPROC                 glDrawElementsBaseVertex;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC            glDrawRangeElementsBaseVertex;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC        glDrawElementsInstancedBaseVertex;
PFNGLFRAMEBUFFERTEXTUREPROC                     glFramebufferTexture;
PFNGLPRIMITIVEBOUNDINGBOXPROC                   glPrimitiveBoundingBox;
PFNGLGETGRAPHICSRESETSTATUSPROC                 glGetGraphicsResetStatus;
PFNGLREADNPIXELSPROC                            glReadnPixels;
PFNGLGETNUNIFORMFVPROC                          glGetnUniformfv;
PFNGLGETNUNIFORMIVPROC                          glGetnUniformiv;
PFNGLGETNUNIFORMUIVPROC                         glGetnUniformuiv;
PFNGLMINSAMPLESHADINGPROC                       glMinSampleShading;
PFNGLPATCHPARAMETERIPROC                        glPatchParameteri;
PFNGLTEXPARAMETERIIVPROC                        glTexParameterIiv;
PFNGLTEXPARAMETERIUIVPROC                       glTexParameterIuiv;
PFNGLGETTEXPARAMETERIIVPROC                     glGetTexParameterIiv;
PFNGLGETTEXPARAMETERIUIVPROC                    glGetTexParameterIuiv;
PFNGLSAMPLERPARAMETERIIVPROC                    glSamplerParameterIiv;
PFNGLSAMPLERPARAMETERIUIVPROC                   glSamplerParameterIuiv;
PFNGLGETSAMPLERPARAMETERIIVPROC                 glGetSamplerParameterIiv;
PFNGLGETSAMPLERPARAMETERIUIVPROC                glGetSamplerParameterIuiv;
PFNGLTEXBUFFERPROC                              glTexBuffer;
PFNGLTEXBUFFERRANGEPROC                         glTexBufferRange;
PFNGLTEXSTORAGE3DMULTISAMPLEPROC                glTexStorage3DMultisample;
//==============================================================================
