//==============================================================================
// xxGraphic : OpenGL Internal Source
//
// Copyright (c) 2019-2021 TAiGA
// https://github.com/metarutaiga/xxGraphic
//==============================================================================
#include "xxGraphicInternalGL.h"

//==============================================================================
//  GLSL Shader
//==============================================================================
const char* const glDefaultShaderCode =
R"(uniform vec4 buf[12];
uniform sampler2D tex;

attribute vec3 position;
attribute vec4 color;
attribute vec2 uv;

varying vec4 varyColor;
varying vec2 varyUV;

#ifdef _VERTEX_
void main()
{
    mat4 world = mat4(buf[0], buf[1], buf[2], buf[3]);
    mat4 view = mat4(buf[4], buf[5], buf[6], buf[7]);
    mat4 projection = mat4(buf[8], buf[9], buf[10], buf[11]);
    gl_Position = projection * (view * (world * vec4(position, 1)));
    varyColor = color;
    varyUV = uv;
}
#endif

#ifdef _FRAGMENT_
void main()
{
    gl_FragColor = varyColor * texture2D(tex, varyUV);
}
#endif)";
//==============================================================================
//  Function
//==============================================================================
uint64_t    (*glCreateContext)(uint64_t instance, void* view, void** display);
void        (*glDestroyContext)(uint64_t context, void* view, void* display);
void*       (*glGetProcAddress)(const char* name);
float       (*glGetScaleContext)(uint64_t context, void* view);
void        (*glMakeCurrentContext)(uint64_t context, void* display);
void        (*glPresentContext)(uint64_t context, void* display);
//==============================================================================
//  Loader
//==============================================================================
#define GL_PROTOTYPE(type, prototype, parameter, ...) \
extern type (GL_APIENTRYP prototype ## Entry) parameter; \
extern "C" type GL_APIENTRY prototype parameter \
{ \
    return prototype ## Entry(__VA_ARGS__); \
} \
static void* GL_APIENTRY prototype ## Dummy parameter \
{ \
    return NULL; \
} \
static type GL_APIENTRY prototype ## Trunk parameter \
{ \
    prototype ## Entry = (type (GL_APIENTRYP) parameter)glGetProcAddress(#prototype); \
    if (prototype ## Entry == NULL) \
        prototype ## Entry = (type (GL_APIENTRYP) parameter)prototype ## Dummy; \
    return prototype ## Entry(__VA_ARGS__); \
} \
static type (GL_APIENTRYP prototype ## Entry) parameter = prototype ## Trunk;
//------------------------------------------------------------------------------
GL_PROTOTYPE(void, glActiveTexture, (GLenum texture), texture);
GL_PROTOTYPE(void, glAttachShader, (GLuint program, GLuint shader), program, shader);
GL_PROTOTYPE(void, glBindAttribLocation, (GLuint program, GLuint index, const GLchar *name), program, index, name);
GL_PROTOTYPE(void, glBindBuffer, (GLenum target, GLuint buffer), target, buffer);
GL_PROTOTYPE(void, glBindFramebuffer, (GLenum target, GLuint framebuffer), target, framebuffer);
GL_PROTOTYPE(void, glBindRenderbuffer, (GLenum target, GLuint renderbuffer), target, renderbuffer);
GL_PROTOTYPE(void, glBindTexture, (GLenum target, GLuint texture), target, texture);
GL_PROTOTYPE(void, glBlendColor, (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha), red, green, blue, alpha);
GL_PROTOTYPE(void, glBlendEquation, (GLenum mode), mode);
GL_PROTOTYPE(void, glBlendEquationSeparate, (GLenum modeRGB, GLenum modeAlpha), modeRGB, modeAlpha);
GL_PROTOTYPE(void, glBlendFunc, (GLenum sfactor, GLenum dfactor), sfactor, dfactor);
GL_PROTOTYPE(void, glBlendFuncSeparate, (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha), sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
GL_PROTOTYPE(void, glBufferData, (GLenum target, GLsizeiptr size, const void *data, GLenum usage), target, size, data, usage);
GL_PROTOTYPE(void, glBufferSubData, (GLenum target, GLintptr offset, GLsizeiptr size, const void *data), target, offset, size, data);
GL_PROTOTYPE(GLenum, glCheckFramebufferStatus, (GLenum target), target);
GL_PROTOTYPE(void, glClear, (GLbitfield mask), mask);
GL_PROTOTYPE(void, glClearColor, (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha), red, green, blue, alpha);
GL_PROTOTYPE(void, glClearDepthf, (GLfloat d), d);
GL_PROTOTYPE(void, glClearStencil, (GLint s), s);
GL_PROTOTYPE(void, glColorMask, (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha), red, green, blue, alpha);
GL_PROTOTYPE(void, glCompileShader, (GLuint shader), shader);
GL_PROTOTYPE(void, glCompressedTexImage2D, (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data), target, level, internalformat, width, height, border, imageSize, data);
GL_PROTOTYPE(void, glCompressedTexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data), target, level, xoffset, yoffset, width, height, format, imageSize, data);
GL_PROTOTYPE(void, glCopyTexImage2D, (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border), target, level, internalformat, x, y, width, height, border);
GL_PROTOTYPE(void, glCopyTexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height), target, level, xoffset, yoffset, x, y, width, height);
GL_PROTOTYPE(GLuint, glCreateProgram, (void));
GL_PROTOTYPE(GLuint, glCreateShader, (GLenum type), type);
GL_PROTOTYPE(void, glCullFace, (GLenum mode), mode);
GL_PROTOTYPE(void, glDeleteBuffers, (GLsizei n, const GLuint *buffers), n, buffers);
GL_PROTOTYPE(void, glDeleteFramebuffers, (GLsizei n, const GLuint *framebuffers), n, framebuffers);
GL_PROTOTYPE(void, glDeleteProgram, (GLuint program), program);
GL_PROTOTYPE(void, glDeleteRenderbuffers, (GLsizei n, const GLuint *renderbuffers), n, renderbuffers);
GL_PROTOTYPE(void, glDeleteShader, (GLuint shader), shader);
GL_PROTOTYPE(void, glDeleteTextures, (GLsizei n, const GLuint *textures), n, textures);
GL_PROTOTYPE(void, glDepthFunc, (GLenum func), func);
GL_PROTOTYPE(void, glDepthMask, (GLboolean flag), flag);
GL_PROTOTYPE(void, glDepthRangef, (GLfloat n, GLfloat f), n, f);
GL_PROTOTYPE(void, glDetachShader, (GLuint program, GLuint shader), program, shader);
GL_PROTOTYPE(void, glDisable, (GLenum cap), cap);
GL_PROTOTYPE(void, glDisableVertexAttribArray, (GLuint index), index);
GL_PROTOTYPE(void, glDrawArrays, (GLenum mode, GLint first, GLsizei count), mode, first, count);
GL_PROTOTYPE(void, glDrawElements, (GLenum mode, GLsizei count, GLenum type, const void *indices), mode, count, type, indices);
GL_PROTOTYPE(void, glEnable, (GLenum cap), cap);
GL_PROTOTYPE(void, glEnableVertexAttribArray, (GLuint index), index);
GL_PROTOTYPE(void, glFinish, (void));
GL_PROTOTYPE(void, glFlush, (void));
GL_PROTOTYPE(void, glFramebufferRenderbuffer, (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer), target, attachment, renderbuffertarget, renderbuffer);
GL_PROTOTYPE(void, glFramebufferTexture2D, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level), target, attachment, textarget, texture, level);
GL_PROTOTYPE(void, glFrontFace, (GLenum mode), mode);
GL_PROTOTYPE(void, glGenBuffers, (GLsizei n, GLuint *buffers), n, buffers);
GL_PROTOTYPE(void, glGenerateMipmap, (GLenum target), target);
GL_PROTOTYPE(void, glGenFramebuffers, (GLsizei n, GLuint *framebuffers), n, framebuffers);
GL_PROTOTYPE(void, glGenRenderbuffers, (GLsizei n, GLuint *renderbuffers), n, renderbuffers);
GL_PROTOTYPE(void, glGenTextures, (GLsizei n, GLuint *textures), n, textures);
GL_PROTOTYPE(void, glGetActiveAttrib, (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name), program, index, bufSize, length, size, type, name);
GL_PROTOTYPE(void, glGetActiveUniform, (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name), program, index, bufSize, length, size, type, name);
GL_PROTOTYPE(void, glGetAttachedShaders, (GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders), program, maxCount, count, shaders);
GL_PROTOTYPE(GLint, glGetAttribLocation, (GLuint program, const GLchar *name), program, name);
GL_PROTOTYPE(void, glGetBooleanv, (GLenum pname, GLboolean *data), pname, data);
GL_PROTOTYPE(void, glGetBufferParameteriv, (GLenum target, GLenum pname, GLint *params), target, pname, params);
GL_PROTOTYPE(GLenum, glGetError, (void));
GL_PROTOTYPE(void, glGetFloatv, (GLenum pname, GLfloat *data), pname, data);
GL_PROTOTYPE(void, glGetFramebufferAttachmentParameteriv, (GLenum target, GLenum attachment, GLenum pname, GLint *params), target, attachment, pname, params);
GL_PROTOTYPE(void, glGetIntegerv, (GLenum pname, GLint *data), pname, data);
GL_PROTOTYPE(void, glGetProgramiv, (GLuint program, GLenum pname, GLint *params), program, pname, params);
GL_PROTOTYPE(void, glGetProgramInfoLog, (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog), program, bufSize, length, infoLog);
GL_PROTOTYPE(void, glGetRenderbufferParameteriv, (GLenum target, GLenum pname, GLint *params), target, pname, params);
GL_PROTOTYPE(void, glGetShaderiv, (GLuint shader, GLenum pname, GLint *params), shader, pname, params);
GL_PROTOTYPE(void, glGetShaderInfoLog, (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog), shader, bufSize, length, infoLog);
GL_PROTOTYPE(void, glGetShaderPrecisionFormat, (GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision), shadertype, precisiontype, range, precision);
GL_PROTOTYPE(void, glGetShaderSource, (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source), shader, bufSize, length, source);
GL_PROTOTYPE(const GLubyte*, glGetString, (GLenum name), name);
GL_PROTOTYPE(void, glGetTexParameterfv, (GLenum target, GLenum pname, GLfloat *params), target, pname, params);
GL_PROTOTYPE(void, glGetTexParameteriv, (GLenum target, GLenum pname, GLint *params), target, pname, params);
GL_PROTOTYPE(void, glGetUniformfv, (GLuint program, GLint location, GLfloat *params), program, location, params);
GL_PROTOTYPE(void, glGetUniformiv, (GLuint program, GLint location, GLint *params), program, location, params);
GL_PROTOTYPE(GLint, glGetUniformLocation, (GLuint program, const GLchar *name), program, name);
GL_PROTOTYPE(void, glGetVertexAttribfv, (GLuint index, GLenum pname, GLfloat *params), index, pname, params);
GL_PROTOTYPE(void, glGetVertexAttribiv, (GLuint index, GLenum pname, GLint *params), index, pname, params);
GL_PROTOTYPE(void, glGetVertexAttribPointerv, (GLuint index, GLenum pname, void **pointer), index, pname, pointer);
GL_PROTOTYPE(void, glHint, (GLenum target, GLenum mode), target, mode);
GL_PROTOTYPE(GLboolean, glIsBuffer, (GLuint buffer), buffer);
GL_PROTOTYPE(GLboolean, glIsEnabled, (GLenum cap), cap);
GL_PROTOTYPE(GLboolean, glIsFramebuffer, (GLuint framebuffer), framebuffer);
GL_PROTOTYPE(GLboolean, glIsProgram, (GLuint program), program);
GL_PROTOTYPE(GLboolean, glIsRenderbuffer, (GLuint renderbuffer), renderbuffer);
GL_PROTOTYPE(GLboolean, glIsShader, (GLuint shader), shader);
GL_PROTOTYPE(GLboolean, glIsTexture, (GLuint texture), texture);
GL_PROTOTYPE(void, glLineWidth, (GLfloat width), width);
GL_PROTOTYPE(void, glLinkProgram, (GLuint program), program);
GL_PROTOTYPE(void, glPixelStorei, (GLenum pname, GLint param), pname, param);
GL_PROTOTYPE(void, glPolygonOffset, (GLfloat factor, GLfloat units), factor, units);
GL_PROTOTYPE(void, glReadPixels, (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels), x, y, width, height, format, type, pixels);
GL_PROTOTYPE(void, glReleaseShaderCompiler, (void));
GL_PROTOTYPE(void, glRenderbufferStorage, (GLenum target, GLenum internalformat, GLsizei width, GLsizei height), target, internalformat, width, height);
GL_PROTOTYPE(void, glSampleCoverage, (GLfloat value, GLboolean invert), value, invert);
GL_PROTOTYPE(void, glScissor, (GLint x, GLint y, GLsizei width, GLsizei height), x, y, width, height);
GL_PROTOTYPE(void, glShaderBinary, (GLsizei count, const GLuint *shaders, GLenum binaryformat, const void *binary, GLsizei length), count, shaders, binaryformat, binary, length);
GL_PROTOTYPE(void, glShaderSource, (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length), shader, count, string, length);
GL_PROTOTYPE(void, glStencilFunc, (GLenum func, GLint ref, GLuint mask), func, ref, mask);
GL_PROTOTYPE(void, glStencilFuncSeparate, (GLenum face, GLenum func, GLint ref, GLuint mask), face, func, ref, mask);
GL_PROTOTYPE(void, glStencilMask, (GLuint mask), mask);
GL_PROTOTYPE(void, glStencilMaskSeparate, (GLenum face, GLuint mask), face, mask);
GL_PROTOTYPE(void, glStencilOp, (GLenum fail, GLenum zfail, GLenum zpass), fail, zfail, zpass);
GL_PROTOTYPE(void, glStencilOpSeparate, (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass), face, sfail, dpfail, dppass);
GL_PROTOTYPE(void, glTexImage2D, (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels), target, level, internalformat, width, height, border, format, type, pixels);
GL_PROTOTYPE(void, glTexParameterf, (GLenum target, GLenum pname, GLfloat param), target, pname, param);
GL_PROTOTYPE(void, glTexParameterfv, (GLenum target, GLenum pname, const GLfloat *params), target, pname, params);
GL_PROTOTYPE(void, glTexParameteri, (GLenum target, GLenum pname, GLint param), target, pname, param);
GL_PROTOTYPE(void, glTexParameteriv, (GLenum target, GLenum pname, const GLint *params), target, pname, params);
GL_PROTOTYPE(void, glTexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels), target, level, xoffset, yoffset, width, height, format, type, pixels);
GL_PROTOTYPE(void, glUniform1f, (GLint location, GLfloat v0), location, v0);
GL_PROTOTYPE(void, glUniform1fv, (GLint location, GLsizei count, const GLfloat *value), location, count, value);
GL_PROTOTYPE(void, glUniform1i, (GLint location, GLint v0), location, v0);
GL_PROTOTYPE(void, glUniform1iv, (GLint location, GLsizei count, const GLint *value), location, count, value);
GL_PROTOTYPE(void, glUniform2f, (GLint location, GLfloat v0, GLfloat v1), location, v0, v1);
GL_PROTOTYPE(void, glUniform2fv, (GLint location, GLsizei count, const GLfloat *value), location, count, value);
GL_PROTOTYPE(void, glUniform2i, (GLint location, GLint v0, GLint v1), location, v0, v1);
GL_PROTOTYPE(void, glUniform2iv, (GLint location, GLsizei count, const GLint *value), location, count, value);
GL_PROTOTYPE(void, glUniform3f, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2), location, v0, v1, v2);
GL_PROTOTYPE(void, glUniform3fv, (GLint location, GLsizei count, const GLfloat *value), location, count, value);
GL_PROTOTYPE(void, glUniform3i, (GLint location, GLint v0, GLint v1, GLint v2), location, v0, v1, v2);
GL_PROTOTYPE(void, glUniform3iv, (GLint location, GLsizei count, const GLint *value), location, count, value);
GL_PROTOTYPE(void, glUniform4f, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3), location, v0, v1, v2, v3);
GL_PROTOTYPE(void, glUniform4fv, (GLint location, GLsizei count, const GLfloat *value), location, count, value);
GL_PROTOTYPE(void, glUniform4i, (GLint location, GLint v0, GLint v1, GLint v2, GLint v3), location, v0, v1, v2, v3);
GL_PROTOTYPE(void, glUniform4iv, (GLint location, GLsizei count, const GLint *value), location, count, value);
GL_PROTOTYPE(void, glUniformMatrix2fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), location, count, transpose, value);
GL_PROTOTYPE(void, glUniformMatrix3fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), location, count, transpose, value);
GL_PROTOTYPE(void, glUniformMatrix4fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), location, count, transpose, value);
GL_PROTOTYPE(void, glUseProgram, (GLuint program), program);
GL_PROTOTYPE(void, glValidateProgram, (GLuint program), program);
GL_PROTOTYPE(void, glVertexAttrib1f, (GLuint index, GLfloat x), index, x);
GL_PROTOTYPE(void, glVertexAttrib1fv, (GLuint index, const GLfloat *v), index, v);
GL_PROTOTYPE(void, glVertexAttrib2f, (GLuint index, GLfloat x, GLfloat y), index, x, y);
GL_PROTOTYPE(void, glVertexAttrib2fv, (GLuint index, const GLfloat *v), index, v);
GL_PROTOTYPE(void, glVertexAttrib3f, (GLuint index, GLfloat x, GLfloat y, GLfloat z), index, x, y, z);
GL_PROTOTYPE(void, glVertexAttrib3fv, (GLuint index, const GLfloat *v), index, v);
GL_PROTOTYPE(void, glVertexAttrib4f, (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w), index, x, y, z, w);
GL_PROTOTYPE(void, glVertexAttrib4fv, (GLuint index, const GLfloat *v), index, v);
GL_PROTOTYPE(void, glVertexAttribPointer, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer), index, size, type, normalized, stride, pointer);
GL_PROTOTYPE(void, glViewport, (GLint x, GLint y, GLsizei width, GLsizei height), x, y, width, height);
//------------------------------------------------------------------------------
GL_PROTOTYPE(void, glReadBuffer, (GLenum src), src);
GL_PROTOTYPE(void, glDrawRangeElements, (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices), mode, start, end, count, type, indices);
GL_PROTOTYPE(void, glTexImage3D, (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels), target, level, internalformat, width, height, depth, border, format, type, pixels);
GL_PROTOTYPE(void, glTexSubImage3D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels), target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
GL_PROTOTYPE(void, glCopyTexSubImage3D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height), target, level, xoffset, yoffset, zoffset, x, y, width, height);
GL_PROTOTYPE(void, glCompressedTexImage3D, (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data), target, level, internalformat, width, height, depth, border, imageSize, data);
GL_PROTOTYPE(void, glCompressedTexSubImage3D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data), target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
GL_PROTOTYPE(void, glGenQueries, (GLsizei n, GLuint *ids), n, ids);
GL_PROTOTYPE(void, glDeleteQueries, (GLsizei n, const GLuint *ids), n, ids);
GL_PROTOTYPE(GLboolean, glIsQuery, (GLuint id), id);
GL_PROTOTYPE(void, glBeginQuery, (GLenum target, GLuint id), target, id);
GL_PROTOTYPE(void, glEndQuery, (GLenum target), target);
GL_PROTOTYPE(void, glGetQueryiv, (GLenum target, GLenum pname, GLint *params), target, pname, params);
GL_PROTOTYPE(void, glGetQueryObjectuiv, (GLuint id, GLenum pname, GLuint *params), id, pname, params);
GL_PROTOTYPE(GLboolean, glUnmapBuffer, (GLenum target), target);
GL_PROTOTYPE(void, glGetBufferPointerv, (GLenum target, GLenum pname, void **params), target, pname, params);
GL_PROTOTYPE(void, glDrawBuffers, (GLsizei n, const GLenum *bufs), n, bufs);
GL_PROTOTYPE(void, glUniformMatrix2x3fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), location, count, transpose, value);
GL_PROTOTYPE(void, glUniformMatrix3x2fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), location, count, transpose, value);
GL_PROTOTYPE(void, glUniformMatrix2x4fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), location, count, transpose, value);
GL_PROTOTYPE(void, glUniformMatrix4x2fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), location, count, transpose, value);
GL_PROTOTYPE(void, glUniformMatrix3x4fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), location, count, transpose, value);
GL_PROTOTYPE(void, glUniformMatrix4x3fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), location, count, transpose, value);
GL_PROTOTYPE(void, glBlitFramebuffer, (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter), srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
GL_PROTOTYPE(void, glRenderbufferStorageMultisample, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height), target, samples, internalformat, width, height);
GL_PROTOTYPE(void, glFramebufferTextureLayer, (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer), target, attachment, texture, level, layer);
GL_PROTOTYPE(void*, glMapBufferRange, (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access), target, offset, length, access);
GL_PROTOTYPE(void, glFlushMappedBufferRange, (GLenum target, GLintptr offset, GLsizeiptr length), target, offset, length);
GL_PROTOTYPE(void, glBindVertexArray, (GLuint array), array);
GL_PROTOTYPE(void, glDeleteVertexArrays, (GLsizei n, const GLuint *arrays), n, arrays);
GL_PROTOTYPE(void, glGenVertexArrays, (GLsizei n, GLuint *arrays), n, arrays);
GL_PROTOTYPE(GLboolean, glIsVertexArray, (GLuint array), array);
GL_PROTOTYPE(void, glGetIntegeri_v, (GLenum target, GLuint index, GLint *data), target, index, data);
GL_PROTOTYPE(void, glBeginTransformFeedback, (GLenum primitiveMode), primitiveMode);
GL_PROTOTYPE(void, glEndTransformFeedback, (void));
GL_PROTOTYPE(void, glBindBufferRange, (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size), target, index, buffer, offset, size);
GL_PROTOTYPE(void, glBindBufferBase, (GLenum target, GLuint index, GLuint buffer), target, index, buffer);
GL_PROTOTYPE(void, glTransformFeedbackVaryings, (GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode), program, count, varyings, bufferMode);
GL_PROTOTYPE(void, glGetTransformFeedbackVarying, (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name), program, index, bufSize, length, size, type, name);
GL_PROTOTYPE(void, glVertexAttribIPointer, (GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer), index, size, type, stride, pointer);
GL_PROTOTYPE(void, glGetVertexAttribIiv, (GLuint index, GLenum pname, GLint *params), index, pname, params);
GL_PROTOTYPE(void, glGetVertexAttribIuiv, (GLuint index, GLenum pname, GLuint *params), index, pname, params);
GL_PROTOTYPE(void, glVertexAttribI4i, (GLuint index, GLint x, GLint y, GLint z, GLint w), index, x, y, z, w);
GL_PROTOTYPE(void, glVertexAttribI4ui, (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w), index, x, y, z, w);
GL_PROTOTYPE(void, glVertexAttribI4iv, (GLuint index, const GLint *v), index, v);
GL_PROTOTYPE(void, glVertexAttribI4uiv, (GLuint index, const GLuint *v), index, v);
GL_PROTOTYPE(void, glGetUniformuiv, (GLuint program, GLint location, GLuint *params), program, location, params);
GL_PROTOTYPE(GLint, glGetFragDataLocation, (GLuint program, const GLchar *name), program, name);
GL_PROTOTYPE(void, glUniform1ui, (GLint location, GLuint v0), location, v0);
GL_PROTOTYPE(void, glUniform2ui, (GLint location, GLuint v0, GLuint v1), location, v0, v1);
GL_PROTOTYPE(void, glUniform3ui, (GLint location, GLuint v0, GLuint v1, GLuint v2), location, v0, v1, v2);
GL_PROTOTYPE(void, glUniform4ui, (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3), location, v0, v1, v2, v3);
GL_PROTOTYPE(void, glUniform1uiv, (GLint location, GLsizei count, const GLuint *value), location, count, value);
GL_PROTOTYPE(void, glUniform2uiv, (GLint location, GLsizei count, const GLuint *value), location, count, value);
GL_PROTOTYPE(void, glUniform3uiv, (GLint location, GLsizei count, const GLuint *value), location, count, value);
GL_PROTOTYPE(void, glUniform4uiv, (GLint location, GLsizei count, const GLuint *value), location, count, value);
GL_PROTOTYPE(void, glClearBufferiv, (GLenum buffer, GLint drawbuffer, const GLint *value), buffer, drawbuffer, value);
GL_PROTOTYPE(void, glClearBufferuiv, (GLenum buffer, GLint drawbuffer, const GLuint *value), buffer, drawbuffer, value);
GL_PROTOTYPE(void, glClearBufferfv, (GLenum buffer, GLint drawbuffer, const GLfloat *value), buffer, drawbuffer, value);
GL_PROTOTYPE(void, glClearBufferfi, (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil), buffer, drawbuffer, depth, stencil);
GL_PROTOTYPE(const GLubyte*, glGetStringi, (GLenum name, GLuint index), name, index);
GL_PROTOTYPE(void, glCopyBufferSubData, (GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size), readTarget, writeTarget, readOffset, writeOffset, size);
GL_PROTOTYPE(void, glGetUniformIndices, (GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices), program, uniformCount, uniformNames, uniformIndices);
GL_PROTOTYPE(void, glGetActiveUniformsiv, (GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params), program, uniformCount, uniformIndices, pname, params);
GL_PROTOTYPE(GLuint, glGetUniformBlockIndex, (GLuint program, const GLchar *uniformBlockName), program, uniformBlockName);
GL_PROTOTYPE(void, glGetActiveUniformBlockiv, (GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params), program, uniformBlockIndex, pname, params);
GL_PROTOTYPE(void, glGetActiveUniformBlockName, (GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName), program, uniformBlockIndex, bufSize, length, uniformBlockName);
GL_PROTOTYPE(void, glUniformBlockBinding, (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding), program, uniformBlockIndex, uniformBlockBinding);
GL_PROTOTYPE(void, glDrawArraysInstanced, (GLenum mode, GLint first, GLsizei count, GLsizei instancecount), mode, first, count, instancecount);
GL_PROTOTYPE(void, glDrawElementsInstanced, (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount), mode, count, type, indices, instancecount);
GL_PROTOTYPE(GLsync, glFenceSync, (GLenum condition, GLbitfield flags), condition, flags);
GL_PROTOTYPE(GLboolean, glIsSync, (GLsync sync), sync);
GL_PROTOTYPE(void, glDeleteSync, (GLsync sync), sync);
GL_PROTOTYPE(GLenum, glClientWaitSync, (GLsync sync, GLbitfield flags, GLuint64 timeout), sync, flags, timeout);
GL_PROTOTYPE(void, glWaitSync, (GLsync sync, GLbitfield flags, GLuint64 timeout), sync, flags, timeout);
GL_PROTOTYPE(void, glGetInteger64v, (GLenum pname, GLint64 *data), pname, data);
GL_PROTOTYPE(void, glGetSynciv, (GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values), sync, pname, bufSize, length, values);
GL_PROTOTYPE(void, glGetInteger64i_v, (GLenum target, GLuint index, GLint64 *data), target, index, data);
GL_PROTOTYPE(void, glGetBufferParameteri64v, (GLenum target, GLenum pname, GLint64 *params), target, pname, params);
GL_PROTOTYPE(void, glGenSamplers, (GLsizei count, GLuint *samplers), count, samplers);
GL_PROTOTYPE(void, glDeleteSamplers, (GLsizei count, const GLuint *samplers), count, samplers);
GL_PROTOTYPE(GLboolean, glIsSampler, (GLuint sampler), sampler);
GL_PROTOTYPE(void, glBindSampler, (GLuint unit, GLuint sampler), unit, sampler);
GL_PROTOTYPE(void, glSamplerParameteri, (GLuint sampler, GLenum pname, GLint param), sampler, pname, param);
GL_PROTOTYPE(void, glSamplerParameteriv, (GLuint sampler, GLenum pname, const GLint *param), sampler, pname, param);
GL_PROTOTYPE(void, glSamplerParameterf, (GLuint sampler, GLenum pname, GLfloat param), sampler, pname, param);
GL_PROTOTYPE(void, glSamplerParameterfv, (GLuint sampler, GLenum pname, const GLfloat *param), sampler, pname, param);
GL_PROTOTYPE(void, glGetSamplerParameteriv, (GLuint sampler, GLenum pname, GLint *params), sampler, pname, params);
GL_PROTOTYPE(void, glGetSamplerParameterfv, (GLuint sampler, GLenum pname, GLfloat *params), sampler, pname, params);
GL_PROTOTYPE(void, glVertexAttribDivisor, (GLuint index, GLuint divisor), index, divisor);
GL_PROTOTYPE(void, glBindTransformFeedback, (GLenum target, GLuint id), target, id);
GL_PROTOTYPE(void, glDeleteTransformFeedbacks, (GLsizei n, const GLuint *ids), n, ids);
GL_PROTOTYPE(void, glGenTransformFeedbacks, (GLsizei n, GLuint *ids), n, ids);
GL_PROTOTYPE(GLboolean, glIsTransformFeedback, (GLuint id), id);
GL_PROTOTYPE(void, glPauseTransformFeedback, (void));
GL_PROTOTYPE(void, glResumeTransformFeedback, (void));
GL_PROTOTYPE(void, glGetProgramBinary, (GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary), program, bufSize, length, binaryFormat, binary);
GL_PROTOTYPE(void, glProgramBinary, (GLuint program, GLenum binaryFormat, const void *binary, GLsizei length), program, binaryFormat, binary, length);
GL_PROTOTYPE(void, glProgramParameteri, (GLuint program, GLenum pname, GLint value), program, pname, value);
GL_PROTOTYPE(void, glInvalidateFramebuffer, (GLenum target, GLsizei numAttachments, const GLenum *attachments), target, numAttachments, attachments);
GL_PROTOTYPE(void, glInvalidateSubFramebuffer, (GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height), target, numAttachments, attachments, x, y, width, height);
GL_PROTOTYPE(void, glTexStorage2D, (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height), target, levels, internalformat, width, height);
GL_PROTOTYPE(void, glTexStorage3D, (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth), target, levels, internalformat, width, height, depth);
GL_PROTOTYPE(void, glGetInternalformativ, (GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params), target, internalformat, pname, bufSize, params);
//------------------------------------------------------------------------------
GL_PROTOTYPE(void, glDispatchCompute, (GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z), num_groups_x, num_groups_y, num_groups_z);
GL_PROTOTYPE(void, glDispatchComputeIndirect, (GLintptr indirect), indirect);
GL_PROTOTYPE(void, glDrawArraysIndirect, (GLenum mode, const void *indirect), mode, indirect);
GL_PROTOTYPE(void, glDrawElementsIndirect, (GLenum mode, GLenum type, const void *indirect), mode, type, indirect);
GL_PROTOTYPE(void, glFramebufferParameteri, (GLenum target, GLenum pname, GLint param), target, pname, param);
GL_PROTOTYPE(void, glGetFramebufferParameteriv, (GLenum target, GLenum pname, GLint *params), target, pname, params);
GL_PROTOTYPE(void, glGetProgramInterfaceiv, (GLuint program, GLenum programInterface, GLenum pname, GLint *params), program, programInterface, pname, params);
GL_PROTOTYPE(GLuint, glGetProgramResourceIndex, (GLuint program, GLenum programInterface, const GLchar *name), program, programInterface, name);
GL_PROTOTYPE(void, glGetProgramResourceName, (GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name), program, programInterface, index, bufSize, length, name);
GL_PROTOTYPE(void, glGetProgramResourceiv, (GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params), program, programInterface, index, propCount, props, bufSize, length, params);
GL_PROTOTYPE(GLint, glGetProgramResourceLocation, (GLuint program, GLenum programInterface, const GLchar *name), program, programInterface, name);
GL_PROTOTYPE(void, glUseProgramStages, (GLuint pipeline, GLbitfield stages, GLuint program), pipeline, stages, program);
GL_PROTOTYPE(void, glActiveShaderProgram, (GLuint pipeline, GLuint program), pipeline, program);
GL_PROTOTYPE(GLuint, glCreateShaderProgramv, (GLenum type, GLsizei count, const GLchar *const*strings), type, count, strings);
GL_PROTOTYPE(void, glBindProgramPipeline, (GLuint pipeline), pipeline);
GL_PROTOTYPE(void, glDeleteProgramPipelines, (GLsizei n, const GLuint *pipelines), n, pipelines);
GL_PROTOTYPE(void, glGenProgramPipelines, (GLsizei n, GLuint *pipelines), n, pipelines);
GL_PROTOTYPE(GLboolean, glIsProgramPipeline, (GLuint pipeline), pipeline);
GL_PROTOTYPE(void, glGetProgramPipelineiv, (GLuint pipeline, GLenum pname, GLint *params), pipeline, pname, params);
GL_PROTOTYPE(void, glProgramUniform1i, (GLuint program, GLint location, GLint v0), program, location, v0);
GL_PROTOTYPE(void, glProgramUniform2i, (GLuint program, GLint location, GLint v0, GLint v1), program, location, v0, v1);
GL_PROTOTYPE(void, glProgramUniform3i, (GLuint program, GLint location, GLint v0, GLint v1, GLint v2), program, location, v0, v1, v2);
GL_PROTOTYPE(void, glProgramUniform4i, (GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3), program, location, v0, v1, v2, v3);
GL_PROTOTYPE(void, glProgramUniform1ui, (GLuint program, GLint location, GLuint v0), program, location, v0);
GL_PROTOTYPE(void, glProgramUniform2ui, (GLuint program, GLint location, GLuint v0, GLuint v1), program, location, v0, v1);
GL_PROTOTYPE(void, glProgramUniform3ui, (GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2), program, location, v0, v1, v2);
GL_PROTOTYPE(void, glProgramUniform4ui, (GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3), program, location, v0, v1, v2, v3);
GL_PROTOTYPE(void, glProgramUniform1f, (GLuint program, GLint location, GLfloat v0), program, location, v0);
GL_PROTOTYPE(void, glProgramUniform2f, (GLuint program, GLint location, GLfloat v0, GLfloat v1), program, location, v0, v1);
GL_PROTOTYPE(void, glProgramUniform3f, (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2), program, location, v0, v1, v2);
GL_PROTOTYPE(void, glProgramUniform4f, (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3), program, location, v0, v1, v2, v3);
GL_PROTOTYPE(void, glProgramUniform1iv, (GLuint program, GLint location, GLsizei count, const GLint *value), program, location, count, value);
GL_PROTOTYPE(void, glProgramUniform2iv, (GLuint program, GLint location, GLsizei count, const GLint *value), program, location, count, value);
GL_PROTOTYPE(void, glProgramUniform3iv, (GLuint program, GLint location, GLsizei count, const GLint *value), program, location, count, value);
GL_PROTOTYPE(void, glProgramUniform4iv, (GLuint program, GLint location, GLsizei count, const GLint *value), program, location, count, value);
GL_PROTOTYPE(void, glProgramUniform1uiv, (GLuint program, GLint location, GLsizei count, const GLuint *value), program, location, count, value);
GL_PROTOTYPE(void, glProgramUniform2uiv, (GLuint program, GLint location, GLsizei count, const GLuint *value), program, location, count, value);
GL_PROTOTYPE(void, glProgramUniform3uiv, (GLuint program, GLint location, GLsizei count, const GLuint *value), program, location, count, value);
GL_PROTOTYPE(void, glProgramUniform4uiv, (GLuint program, GLint location, GLsizei count, const GLuint *value), program, location, count, value);
GL_PROTOTYPE(void, glProgramUniform1fv, (GLuint program, GLint location, GLsizei count, const GLfloat *value), program, location, count, value);
GL_PROTOTYPE(void, glProgramUniform2fv, (GLuint program, GLint location, GLsizei count, const GLfloat *value), program, location, count, value);
GL_PROTOTYPE(void, glProgramUniform3fv, (GLuint program, GLint location, GLsizei count, const GLfloat *value), program, location, count, value);
GL_PROTOTYPE(void, glProgramUniform4fv, (GLuint program, GLint location, GLsizei count, const GLfloat *value), program, location, count, value);
GL_PROTOTYPE(void, glProgramUniformMatrix2fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), program, location, count, transpose, value);
GL_PROTOTYPE(void, glProgramUniformMatrix3fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), program, location, count, transpose, value);
GL_PROTOTYPE(void, glProgramUniformMatrix4fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), program, location, count, transpose, value);
GL_PROTOTYPE(void, glProgramUniformMatrix2x3fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), program, location, count, transpose, value);
GL_PROTOTYPE(void, glProgramUniformMatrix3x2fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), program, location, count, transpose, value);
GL_PROTOTYPE(void, glProgramUniformMatrix2x4fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), program, location, count, transpose, value);
GL_PROTOTYPE(void, glProgramUniformMatrix4x2fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), program, location, count, transpose, value);
GL_PROTOTYPE(void, glProgramUniformMatrix3x4fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), program, location, count, transpose, value);
GL_PROTOTYPE(void, glProgramUniformMatrix4x3fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), program, location, count, transpose, value);
GL_PROTOTYPE(void, glValidateProgramPipeline, (GLuint pipeline), pipeline);
GL_PROTOTYPE(void, glGetProgramPipelineInfoLog, (GLuint pipeline, GLsizei bufSize, GLsizei *length, GLchar *infoLog), pipeline, bufSize, length, infoLog);
GL_PROTOTYPE(void, glBindImageTexture, (GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format), unit, texture, level, layered, layer, access, format);
GL_PROTOTYPE(void, glGetBooleani_v, (GLenum target, GLuint index, GLboolean *data), target, index, data);
GL_PROTOTYPE(void, glMemoryBarrier, (GLbitfield barriers), barriers);
GL_PROTOTYPE(void, glMemoryBarrierByRegion, (GLbitfield barriers), barriers);
GL_PROTOTYPE(void, glTexStorage2DMultisample, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations), target, samples, internalformat, width, height, fixedsamplelocations);
GL_PROTOTYPE(void, glGetMultisamplefv, (GLenum pname, GLuint index, GLfloat *val), pname, index, val);
GL_PROTOTYPE(void, glSampleMaski, (GLuint maskNumber, GLbitfield mask), maskNumber, mask);
GL_PROTOTYPE(void, glGetTexLevelParameteriv, (GLenum target, GLint level, GLenum pname, GLint *params), target, level, pname, params);
GL_PROTOTYPE(void, glGetTexLevelParameterfv, (GLenum target, GLint level, GLenum pname, GLfloat *params), target, level, pname, params);
GL_PROTOTYPE(void, glBindVertexBuffer, (GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride), bindingindex, buffer, offset, stride);
GL_PROTOTYPE(void, glVertexAttribFormat, (GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset), attribindex, size, type, normalized, relativeoffset);
GL_PROTOTYPE(void, glVertexAttribIFormat, (GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset), attribindex, size, type, relativeoffset);
GL_PROTOTYPE(void, glVertexAttribBinding, (GLuint attribindex, GLuint bindingindex), attribindex, bindingindex);
GL_PROTOTYPE(void, glVertexBindingDivisor, (GLuint bindingindex, GLuint divisor), bindingindex, divisor);
//------------------------------------------------------------------------------
GL_PROTOTYPE(void, glBlendBarrier, (void));
GL_PROTOTYPE(void, glCopyImageSubData, (GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth), srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, srcWidth, srcHeight, srcDepth);
GL_PROTOTYPE(void, glDebugMessageControl, (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled), source, type, severity, count, ids, enabled);
GL_PROTOTYPE(void, glDebugMessageInsert, (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf), source, type, id, severity, length, buf);
GL_PROTOTYPE(void, glDebugMessageCallback, (GLDEBUGPROC callback, const void *userParam), callback, userParam);
GL_PROTOTYPE(GLuint, glGetDebugMessageLog, (GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog), count, bufSize, sources, types, ids, severities, lengths, messageLog);
GL_PROTOTYPE(void, glPushDebugGroup, (GLenum source, GLuint id, GLsizei length, const GLchar *message), source, id, length, message);
GL_PROTOTYPE(void, glPopDebugGroup, (void));
GL_PROTOTYPE(void, glObjectLabel, (GLenum identifier, GLuint name, GLsizei length, const GLchar *label), identifier, name, length, label);
GL_PROTOTYPE(void, glGetObjectLabel, (GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label), identifier, name, bufSize, length, label);
GL_PROTOTYPE(void, glObjectPtrLabel, (const void *ptr, GLsizei length, const GLchar *label), ptr, length, label);
GL_PROTOTYPE(void, glGetObjectPtrLabel, (const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label), ptr, bufSize, length, label);
GL_PROTOTYPE(void, glGetPointerv, (GLenum pname, void **params), pname, params);
GL_PROTOTYPE(void, glEnablei, (GLenum target, GLuint index), target, index);
GL_PROTOTYPE(void, glDisablei, (GLenum target, GLuint index), target, index);
GL_PROTOTYPE(void, glBlendEquationi, (GLuint buf, GLenum mode), buf, mode);
GL_PROTOTYPE(void, glBlendEquationSeparatei, (GLuint buf, GLenum modeRGB, GLenum modeAlpha), buf, modeRGB, modeAlpha);
GL_PROTOTYPE(void, glBlendFunci, (GLuint buf, GLenum src, GLenum dst), buf, src, dst);
GL_PROTOTYPE(void, glBlendFuncSeparatei, (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha), buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
GL_PROTOTYPE(void, glColorMaski, (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a), index, r, g, b, a);
GL_PROTOTYPE(GLboolean, glIsEnabledi, (GLenum target, GLuint index), target, index);
GL_PROTOTYPE(void, glDrawElementsBaseVertex, (GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex), mode, count, type, indices, basevertex);
GL_PROTOTYPE(void, glDrawRangeElementsBaseVertex, (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex), mode, start, end, count, type, indices, basevertex);
GL_PROTOTYPE(void, glDrawElementsInstancedBaseVertex, (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex), mode, count, type, indices, instancecount, basevertex);
GL_PROTOTYPE(void, glFramebufferTexture, (GLenum target, GLenum attachment, GLuint texture, GLint level), target, attachment, texture, level);
GL_PROTOTYPE(void, glPrimitiveBoundingBox, (GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat minW, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLfloat maxW), minX, minY, minZ, minW, maxX, maxY, maxZ, maxW);
GL_PROTOTYPE(GLenum, glGetGraphicsResetStatus, (void));
GL_PROTOTYPE(void, glReadnPixels, (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void *data), x, y, width, height, format, type, bufSize, data);
GL_PROTOTYPE(void, glGetnUniformfv, (GLuint program, GLint location, GLsizei bufSize, GLfloat *params), program, location, bufSize, params);
GL_PROTOTYPE(void, glGetnUniformiv, (GLuint program, GLint location, GLsizei bufSize, GLint *params), program, location, bufSize, params);
GL_PROTOTYPE(void, glGetnUniformuiv, (GLuint program, GLint location, GLsizei bufSize, GLuint *params), program, location, bufSize, params);
GL_PROTOTYPE(void, glMinSampleShading, (GLfloat value), value);
GL_PROTOTYPE(void, glPatchParameteri, (GLenum pname, GLint value), pname, value);
GL_PROTOTYPE(void, glTexParameterIiv, (GLenum target, GLenum pname, const GLint *params), target, pname, params);
GL_PROTOTYPE(void, glTexParameterIuiv, (GLenum target, GLenum pname, const GLuint *params), target, pname, params);
GL_PROTOTYPE(void, glGetTexParameterIiv, (GLenum target, GLenum pname, GLint *params), target, pname, params);
GL_PROTOTYPE(void, glGetTexParameterIuiv, (GLenum target, GLenum pname, GLuint *params), target, pname, params);
GL_PROTOTYPE(void, glSamplerParameterIiv, (GLuint sampler, GLenum pname, const GLint *params), sampler, pname, params);
GL_PROTOTYPE(void, glSamplerParameterIuiv, (GLuint sampler, GLenum pname, const GLuint *params), sampler, pname, params);
GL_PROTOTYPE(void, glGetSamplerParameterIiv, (GLuint sampler, GLenum pname, GLint *params), sampler, pname, params);
GL_PROTOTYPE(void, glGetSamplerParameterIuiv, (GLuint sampler, GLenum pname, GLuint *params), sampler, pname, params);
GL_PROTOTYPE(void, glTexBuffer, (GLenum target, GLenum internalformat, GLuint buffer), target, internalformat, buffer);
GL_PROTOTYPE(void, glTexBufferRange, (GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size), target, internalformat, buffer, offset, size);
GL_PROTOTYPE(void, glTexStorage3DMultisample, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations), target, samples, internalformat, width, height, depth, fixedsamplelocations);
//==============================================================================
//  Windows GL Loader
//==============================================================================
#if defined(xxWINDOWS)
#define _GDI32_
#include <windows.h>
static HMODULE opengl32 = nullptr;
#define WGL_PROTOTYPE(type, prototype, parameter, ...) \
extern type (GL_APIENTRYP prototype ## Entry) parameter; \
extern "C" type GL_APIENTRY prototype parameter \
{ \
    return prototype ## Entry(__VA_ARGS__); \
} \
static void* GL_APIENTRY prototype ## Dummy parameter \
{ \
    return NULL; \
} \
static type GL_APIENTRY prototype ## Trunk parameter \
{ \
    if (opengl32 == NULL) \
        opengl32 = LoadLibraryA("opengl32.dll"); \
    prototype ## Entry = (type (GL_APIENTRYP) parameter)GetProcAddress(opengl32, #prototype); \
    if (prototype ## Entry == NULL) \
        prototype ## Entry = (type (GL_APIENTRYP) parameter)prototype ## Dummy; \
    return prototype ## Entry(__VA_ARGS__); \
} \
static type (GL_APIENTRYP prototype ## Entry) parameter = prototype ## Trunk;
//------------------------------------------------------------------------------
WGL_PROTOTYPE(HGLRC, wglCreateContext, (HDC hDc), hDc);
WGL_PROTOTYPE(BOOL, wglDeleteContext, (HGLRC oldContext), oldContext);
WGL_PROTOTYPE(HGLRC, wglGetCurrentContext, (VOID));
WGL_PROTOTYPE(HDC, wglGetCurrentDC, (VOID));
WGL_PROTOTYPE(PROC, wglGetProcAddress, (LPCSTR lpszProc), lpszProc);
WGL_PROTOTYPE(BOOL, wglMakeCurrent, (HDC hDc, HGLRC newContext), hDc, newContext);
WGL_PROTOTYPE(BOOL, wglShareLists, (HGLRC hrcSrvShare, HGLRC hrcSrvSource), hrcSrvShare, hrcSrvSource);
//------------------------------------------------------------------------------
#endif
//==============================================================================
