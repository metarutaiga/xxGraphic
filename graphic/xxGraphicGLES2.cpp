#include "xxGraphicInternal.h"
#include "xxGraphicGL.h"
#include "xxGraphicGLES2.h"

#include "gl/gl2.h"
#define GL_CONSTANT_BUFFER  0

//==============================================================================
//  Instance
//==============================================================================
uint64_t xxCreateInstanceGLES2()
{
    uint64_t instance = xxGraphicCreateWGL();
    if (instance == 0)
        return 0;

    xxRegisterFunction(GLES2);

    return instance;
}
//------------------------------------------------------------------------------
void xxDestroyInstanceGLES2(uint64_t instance)
{
    xxGraphicDestroyWGL(instance, nullptr, nullptr);

    xxUnregisterFunction();
}
//==============================================================================
//  Device
//==============================================================================
uint64_t xxCreateDeviceGLES2(uint64_t instance)
{
    return instance;
}
//------------------------------------------------------------------------------
void xxDestroyDeviceGLES2(uint64_t device)
{

}
//------------------------------------------------------------------------------
void xxResetDeviceGLES2(uint64_t device)
{

}
//------------------------------------------------------------------------------
bool xxTestDeviceGLES2(uint64_t device)
{
    return true;
}
//------------------------------------------------------------------------------
const char* xxGetDeviceStringGLES2(uint64_t device)
{
    return "OpenGL ES 2.0";
}
//==============================================================================
//  Framebuffer
//==============================================================================
//==============================================================================
//  Swapchain
//==============================================================================
struct SWAPCHAINGL
{
    uint64_t    context;
    void*       view;
    void*       display;
    int         width;
    int         height;
    GLuint      vao;
};
//------------------------------------------------------------------------------
uint64_t xxCreateSwapchainGLES2(uint64_t device, void* view, unsigned int width, unsigned int height)
{
    SWAPCHAINGL* glSwapchain = new SWAPCHAINGL;
    if (glSwapchain == nullptr)
        return 0;

    void* display = nullptr;
    uint64_t context = xxglCreateContext(device, view, &display);
    if (context == 0)
    {
        delete glSwapchain;
        return 0;
    }

    HWND hWnd = (HWND)view;
    if (width == 0 || height == 0)
    {
        RECT rect = {};
        if (GetClientRect(hWnd, &rect) == TRUE)
        {
            width = rect.right - rect.left;
            height = rect.bottom - rect.top;
        }
    }

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glSwapchain->context = context;
    glSwapchain->view = view;
    glSwapchain->display = display;
    glSwapchain->width = width;
    glSwapchain->height = height;
    glSwapchain->vao = vao;

    return reinterpret_cast<uint64_t>(glSwapchain);
}
//------------------------------------------------------------------------------
void xxDestroySwapchainGLES2(uint64_t swapchain)
{
    SWAPCHAINGL* glSwapchain = reinterpret_cast<SWAPCHAINGL*>(swapchain);
    if (glSwapchain == nullptr)
        return;

    xxglMakeCurrentContext(glSwapchain->context, glSwapchain->display);
    if (glSwapchain->vao)
    {
        glDeleteVertexArrays(1, &glSwapchain->vao);
    }
    xxglDestroyContext(glSwapchain->context, glSwapchain->view, glSwapchain->display);
    delete glSwapchain;
}
//------------------------------------------------------------------------------
void xxPresentSwapchainGLES2(uint64_t swapchain)
{
    SWAPCHAINGL* glSwapchain = reinterpret_cast<SWAPCHAINGL*>(swapchain);
    if (glSwapchain == nullptr)
        return;

    xxglPresentContext(glSwapchain->context, glSwapchain->display);
}
//------------------------------------------------------------------------------
uint64_t xxGetCommandBufferGLES2(uint64_t device, uint64_t swapchain)
{
    SWAPCHAINGL* glSwapchain = reinterpret_cast<SWAPCHAINGL*>(swapchain);
    if (glSwapchain == nullptr)
        return 0;

    xxglMakeCurrentContext(glSwapchain->context, glSwapchain->display);

    return device;
}
//------------------------------------------------------------------------------
uint64_t xxGetFramebufferGLES2(uint64_t device, uint64_t swapchain)
{
    return 0;
}
//==============================================================================
//  Command Buffer
//==============================================================================
bool xxBeginCommandBufferGLES2(uint64_t commandBuffer)
{
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);

    return true;
}
//------------------------------------------------------------------------------
void xxEndCommandBufferGLES2(uint64_t commandBuffer)
{

}
//------------------------------------------------------------------------------
void xxSubmitCommandBufferGLES2(uint64_t commandBuffer)
{
}
//==============================================================================
//  Render Pass
//==============================================================================
struct RENDERPASSGL
{
    float   color[4];
    float   depth;
    uint8_t stencil;
};
//------------------------------------------------------------------------------
uint64_t xxCreateRenderPassGLES2(uint64_t device, float r, float g, float b, float a, float depth, unsigned char stencil)
{
    RENDERPASSGL* glRenderPass = new RENDERPASSGL;

    glRenderPass->color[0] = r;
    glRenderPass->color[1] = g;
    glRenderPass->color[2] = b;
    glRenderPass->color[3] = a;
    glRenderPass->depth = depth;
    glRenderPass->stencil = stencil;

    return reinterpret_cast<uint64_t>(glRenderPass);
}
//------------------------------------------------------------------------------
void xxDestroyRenderPassGLES2(uint64_t renderPass)
{
    RENDERPASSGL* glRenderPass = reinterpret_cast<RENDERPASSGL*>(renderPass);

    delete glRenderPass;
}
//------------------------------------------------------------------------------
bool xxBeginRenderPassGLES2(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass)
{
    RENDERPASSGL* glRenderPass = reinterpret_cast<RENDERPASSGL*>(renderPass);
    if (glRenderPass == nullptr)
        return false;

    glClearColor(glRenderPass->color[0], glRenderPass->color[1], glRenderPass->color[2], glRenderPass->color[3]);
    glClearDepthf(glRenderPass->depth);
    glClearStencil(glRenderPass->stencil);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    return true;
}
//------------------------------------------------------------------------------
void xxEndRenderPassGLES2(uint64_t commandBuffer, uint64_t framebuffer, uint64_t renderPass)
{

}
//==============================================================================
//  Buffer
//==============================================================================
struct BUFFERGL
{
    GLenum      type;
    GLuint      buffer;
    void*       memory;
    GLsizei     size;
};
//------------------------------------------------------------------------------
uint64_t xxCreateConstantBufferGLES2(uint64_t device, unsigned int size)
{
    BUFFERGL* glBuffer = new BUFFERGL;
    if (glBuffer == nullptr)
        return 0;

    glBuffer->type = GL_CONSTANT_BUFFER;
    glBuffer->buffer = 0;
    glBuffer->memory = xxAlloc(char, size);
    glBuffer->size = size;

    return reinterpret_cast<uint64_t>(glBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateIndexBufferGLES2(uint64_t device, unsigned int size)
{
    BUFFERGL* glBuffer = new BUFFERGL;
    if (glBuffer == nullptr)
        return 0;

    GLuint buffer = 0;
    glGenBuffers(1, &buffer);

    glBuffer->type = GL_ELEMENT_ARRAY_BUFFER;
    glBuffer->buffer = buffer;
    glBuffer->memory = xxAlloc(char, size);
    glBuffer->size = size;

    return reinterpret_cast<uint64_t>(glBuffer);
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexBufferGLES2(uint64_t device, unsigned int size)
{
    BUFFERGL* glBuffer = new BUFFERGL;
    if (glBuffer == nullptr)
        return 0;

    GLuint buffer = 0;
    glGenBuffers(1, &buffer);

    glBuffer->type = GL_ARRAY_BUFFER;
    glBuffer->buffer = buffer;
    glBuffer->memory = xxAlloc(char, size);
    glBuffer->size = size;

    return reinterpret_cast<uint64_t>(glBuffer);
}
//------------------------------------------------------------------------------
void xxDestroyBufferGLES2(uint64_t device, uint64_t buffer)
{
    BUFFERGL* glBuffer = reinterpret_cast<BUFFERGL*>(buffer);
    if (glBuffer == nullptr)
        return;

    if (glBuffer->buffer)
        glDeleteBuffers(1, &glBuffer->buffer);
    xxFree(glBuffer->memory);
    delete glBuffer;
}
//------------------------------------------------------------------------------
void* xxMapBufferGLES2(uint64_t device, uint64_t buffer)
{
    BUFFERGL* glBuffer = reinterpret_cast<BUFFERGL*>(buffer);
    if (glBuffer == nullptr)
        return nullptr;

    return glBuffer->memory;
}
//------------------------------------------------------------------------------
void xxUnmapBufferGLES2(uint64_t device, uint64_t buffer)
{
    BUFFERGL* glBuffer = reinterpret_cast<BUFFERGL*>(buffer);
    if (glBuffer == nullptr)
        return;
    if (glBuffer->type == GL_CONSTANT_BUFFER)
        return;

    glBindBuffer(glBuffer->type, glBuffer->buffer);
    glBufferData(glBuffer->type, glBuffer->size, glBuffer->memory, GL_STREAM_DRAW);
}
//==============================================================================
//  Texture
//==============================================================================
struct TEXTUREGL
{
    GLenum          type;
    GLuint          texture;
    void*           memory;
    unsigned int    width;
    unsigned int    height;
    unsigned int    depth;
    unsigned int    mipmap;
    unsigned int    array;
};
//------------------------------------------------------------------------------
uint64_t xxCreateTextureGLES2(uint64_t device, int format, unsigned int width, unsigned int height, unsigned int depth, unsigned int mipmap, unsigned int array)
{
    if (width == 0 || height == 0 || depth == 0 || mipmap == 0 || array == 0)
        return 0;

    TEXTUREGL* glTexture = new TEXTUREGL;
    if (glTexture == nullptr)
        return 0;

    GLuint texture = 0;
    glGenTextures(1, &texture);
    if (glGetError() != 0)
    {
        delete glTexture;
        return 0;
    }

    glTexture->type = 0;
    glTexture->texture = texture;
    glTexture->memory = nullptr;
    glTexture->width = width;
    glTexture->height = height;
    glTexture->depth = depth;
    glTexture->mipmap = mipmap;
    glTexture->array = array;

    return reinterpret_cast<uint64_t>(glTexture);
}
//------------------------------------------------------------------------------
void xxDestroyTextureGLES2(uint64_t texture)
{
    TEXTUREGL* glTexture = reinterpret_cast<TEXTUREGL*>(texture);
    if (glTexture == nullptr)
        return;

    glDeleteTextures(1, &glTexture->texture);
    xxFree(glTexture->memory);
    delete glTexture;
}
//------------------------------------------------------------------------------
void* xxMapTextureGLES2(uint64_t device, uint64_t texture, unsigned int& stride, unsigned int level, unsigned int array, unsigned int mipmap)
{
    TEXTUREGL* glTexture = reinterpret_cast<TEXTUREGL*>(texture);
    if (glTexture == nullptr)
        return nullptr;

    unsigned int width = glTexture->width >> level;
    unsigned int height = glTexture->height >> level;
    unsigned int depth = glTexture->depth >> level;
    if (width == 0)
        width = 1;
    if (height == 0)
        height = 1;
    if (depth == 0)
        depth = 1;
    glTexture->memory = xxRealloc(char, width * height * depth * 4, glTexture->memory);

    stride = width * 4;

    return glTexture->memory;
}
//------------------------------------------------------------------------------
void xxUnmapTextureGLES2(uint64_t device, uint64_t texture, unsigned int level, unsigned int array, unsigned int mipmap)
{
    TEXTUREGL* glTexture = reinterpret_cast<TEXTUREGL*>(texture);
    if (glTexture == nullptr)
        return;

    if (glTexture->depth == 1 && glTexture->array == 1)
    {
        glTexture->type = GL_TEXTURE_2D;
        glBindTexture(GL_TEXTURE_2D, glTexture->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, glTexture->width, glTexture->height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, glTexture->memory);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (glTexture->width == glTexture->height && glTexture->depth == 1 && glTexture->array == 6)
    {
        glTexture->type = GL_TEXTURE_CUBE_MAP;
        glBindTexture(GL_TEXTURE_CUBE_MAP, glTexture->texture);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + array, level, GL_RGBA, glTexture->width, glTexture->height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, glTexture->memory);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    if (glTexture->mipmap == level + 1)
    {
        xxFree(glTexture->memory);
        glTexture->memory = nullptr;
    }
}
//==============================================================================
//  Sampler
//==============================================================================
uint64_t xxCreateSamplerGLES2(uint64_t device, bool clampU, bool clampV, bool clampW, bool linearMag, bool linearMin, bool linearMip, int anisotropy)
{
    return 0;
}
//------------------------------------------------------------------------------
void xxDestroySamplerGLES2(uint64_t sampler)
{

}
//==============================================================================
//  Vertex Attribute
//==============================================================================
struct VERTEXATTRIBUTEGL
{
    struct Attribute
    {
        GLuint index;
        GLint size;
        GLenum type;
        GLboolean normalized;
        GLsizei stride;
        const GLvoid* pointer;
        const char* name;
    } attributes[16];
    int count;
};
//------------------------------------------------------------------------------
uint64_t xxCreateVertexAttributeGLES2(uint64_t device, int count, ...)
{
    VERTEXATTRIBUTEGL* glVertexAttribute = new VERTEXATTRIBUTEGL;
    if (glVertexAttribute == nullptr)
        return 0;

    VERTEXATTRIBUTEGL::Attribute* attributes = glVertexAttribute->attributes;
    int stride = 0;

    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; ++i)
    {
        int stream = va_arg(args, int);
        int offset = va_arg(args, int);
        int element = va_arg(args, int);
        int size = va_arg(args, int);

        stride += size;

        attributes[i].index = i;
        attributes[i].size = element;
        attributes[i].type = GL_FLOAT;
        attributes[i].normalized = GL_FALSE;
        attributes[i].stride = 0;
        attributes[i].pointer = (char*)nullptr + offset;

        if (offset == 0 && element == 3 && size == sizeof(float) * 3)
        {
            attributes[i].name = "position";
        }
        if (offset != 0 && element == 3 && size == sizeof(float) * 3)
        {
            attributes[i].name = "normal";
        }
        if (offset != 0 && element == 4 && size == sizeof(char) * 4)
        {
            attributes[i].size = GL_BGRA_EXT;
            attributes[i].type = GL_UNSIGNED_BYTE;
            attributes[i].normalized = GL_TRUE;
            attributes[i].name = "color";
        }
        if (offset != 0 && element == 2 && size == sizeof(float) * 2)
        {
            attributes[i].name = "uv";
        }
    }
    va_end(args);

    for (int i = 0; i < count; ++i)
    {
        attributes[i].stride = stride;
    }

    glVertexAttribute->count = count;

    return reinterpret_cast<uint64_t>(glVertexAttribute);
}
//------------------------------------------------------------------------------
void xxDestroyVertexAttributeGLES2(uint64_t vertexAttribute)
{
    VERTEXATTRIBUTEGL* glVertexAttribute = new VERTEXATTRIBUTEGL;

    delete glVertexAttribute;
}
//==============================================================================
//  Shader
//==============================================================================
static void checkShader(GLuint glShader, const char* shader)
{
    GLint status = 0;
    glGetShaderiv(glShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        xxLog("xxGraphicGLES2", "failed to compile %s", shader);

        GLint length = 0;
        glGetShaderiv(glShader, GL_INFO_LOG_LENGTH, &length);
        char* log = xxAlloc(char, length + 1);
        if (log)
        {
            glGetShaderInfoLog(glShader, length, nullptr, log);
            log[length] = '\0';
            xxLog("xxGraphicGLES2", "%s", log);
            xxFree(log);
        }
    }
}
//------------------------------------------------------------------------------
uint64_t xxCreateVertexShaderGLES2(uint64_t device, const char* shader, uint64_t vertexAttribute)
{
    if (strcmp(shader, "default") == 0)
    {
        const char* sources[] = { "#version 100", "\n", vertexShaderCode };

        GLuint glShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(glShader, xxCountOf(sources), sources, nullptr);
        glCompileShader(glShader);
        checkShader(glShader, shader);

        return static_cast<uint64_t>(glShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateFragmentShaderGLES2(uint64_t device, const char* shader)
{
    if (strcmp(shader, "default") == 0)
    {
        const char* sources[] = { "#version 100", "\n", "precision mediump float;", "\n", fragmentShaderCode };

        GLuint glShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(glShader, xxCountOf(sources), sources, nullptr);
        glCompileShader(glShader);
        checkShader(glShader, shader);

        return static_cast<uint64_t>(glShader);
    }

    return 0;
}
//------------------------------------------------------------------------------
void xxDestroyShaderGLES2(uint64_t device, uint64_t shader)
{
    GLuint glShader = static_cast<GLuint>(shader);
    if (glShader == 0)
        return;

    glDeleteShader(glShader);
}
//==============================================================================
//  Pipeline
//==============================================================================
struct PIPELINEGL : public VERTEXATTRIBUTEGL
{
    GLuint  program;
    GLint   texture;
};
//------------------------------------------------------------------------------
static void checkProgram(GLuint glProgram)
{
    GLint status = 0;
    glGetProgramiv(glProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        xxLog("xxGraphicGLES2", "failed to link %u", glProgram);

        GLint length = 0;
        glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &length);
        char* log = xxAlloc(char, length + 1);
        if (log)
        {
            glGetProgramInfoLog(glProgram, length, nullptr, log);
            log[length] = '\0';
            xxLog("xxGraphicGLES2", "%s", log);
            xxFree(log);
        }
    }
}
//------------------------------------------------------------------------------
uint64_t xxCreateBlendStateGLES2(uint64_t device, bool blending)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateDepthStencilStateGLES2(uint64_t device, bool depthTest, bool depthWrite)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreateRasterizerStateGLES2(uint64_t device, bool cull, bool scissor)
{
    return 0;
}
//------------------------------------------------------------------------------
uint64_t xxCreatePipelineGLES2(uint64_t device, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t vertexShader, uint64_t fragmentShader)
{
    VERTEXATTRIBUTEGL* glVertexAttribute = reinterpret_cast<VERTEXATTRIBUTEGL*>(vertexAttribute);
    if (glVertexAttribute == nullptr)
        return 0;
    PIPELINEGL* glPipeline = new PIPELINEGL;
    if (glPipeline == nullptr)
        return 0;

    GLuint glVertexShader = static_cast<GLuint>(vertexShader);
    GLuint glFragmentShader = static_cast<GLuint>(fragmentShader);
    GLuint glProgram = glCreateProgram();
    glAttachShader(glProgram, glVertexShader);
    glAttachShader(glProgram, glFragmentShader);
    glLinkProgram(glProgram);
    checkProgram(glProgram);

    VERTEXATTRIBUTEGL::Attribute* sources = glVertexAttribute->attributes;
    VERTEXATTRIBUTEGL::Attribute* targets = glPipeline->attributes;
    for (int i = 0; i < glVertexAttribute->count; ++i)
    {
        VERTEXATTRIBUTEGL::Attribute& source = sources[i];
        VERTEXATTRIBUTEGL::Attribute& target = targets[i];
        target = source;
        target.index = glGetAttribLocation(glProgram, target.name);
    }

    glPipeline->count = glVertexAttribute->count;
    glPipeline->program = glProgram;
    glPipeline->texture = glGetUniformLocation(glProgram, "tex");

    return reinterpret_cast<uint64_t>(glPipeline);
}
//------------------------------------------------------------------------------
void xxDestroyBlendStateGLES2(uint64_t blendState)
{

}
//------------------------------------------------------------------------------
void xxDestroyDepthStencilStateGLES2(uint64_t depthStencilState)
{

}
//------------------------------------------------------------------------------
void xxDestroyRasterizerStateGLES2(uint64_t rasterizerState)
{

}
//------------------------------------------------------------------------------
void xxDestroyPipelineGLES2(uint64_t pipeline)
{
    PIPELINEGL* glPipeline = reinterpret_cast<PIPELINEGL*>(pipeline);
    if (glPipeline == nullptr)
        return;

    glDeleteProgram(glPipeline->program);
    delete glPipeline;
}
//==============================================================================
//  Command
//==============================================================================
void xxSetViewportGLES2(uint64_t commandBuffer, int x, int y, int width, int height, float minZ, float maxZ)
{
    glViewport(x, y, width, height);
}
//------------------------------------------------------------------------------
void xxSetScissorGLES2(uint64_t commandBuffer, int x, int y, int width, int height)
{
    glScissor(x, y, width, height);
}
//------------------------------------------------------------------------------
void xxSetPipelineGLES2(uint64_t commandBuffer, uint64_t pipeline)
{
    PIPELINEGL* glPipeline = reinterpret_cast<PIPELINEGL*>(pipeline);
    VERTEXATTRIBUTEGL::Attribute* attributes = glPipeline->attributes;

    for (int i = 0; i < glPipeline->count; ++i)
    {
        VERTEXATTRIBUTEGL::Attribute& attribute = attributes[i];
        glEnableVertexAttribArray(attribute.index);
        glVertexAttribPointer(attribute.index, attribute.size, attribute.type, attribute.normalized, attribute.stride, attribute.pointer);
    }

    glUseProgram(glPipeline->program);
    glUniform1i(glPipeline->texture, 0);
}
//------------------------------------------------------------------------------
void xxSetIndexBufferGLES2(uint64_t commandBuffer, uint64_t buffer)
{
    BUFFERGL* glBuffer = reinterpret_cast<BUFFERGL*>(buffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glBuffer->buffer);
}
//------------------------------------------------------------------------------
void xxSetVertexBuffersGLES2(uint64_t commandBuffer, int count, const uint64_t* buffers, uint64_t vertexAttribute)
{
    BUFFERGL* glBuffer = reinterpret_cast<BUFFERGL*>(buffers[0]);

    glBindBuffer(GL_ARRAY_BUFFER, glBuffer->buffer);
}
//------------------------------------------------------------------------------
void xxSetVertexTexturesGLES2(uint64_t commandBuffer, int count, const uint64_t* textures)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentTexturesGLES2(uint64_t commandBuffer, int count, const uint64_t* textures)
{
    for (int i = 0; i < count; ++i)
    {
        TEXTUREGL* glTexture = reinterpret_cast<TEXTUREGL*>(textures[i]);
        if (glTexture == nullptr)
            continue;
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(glTexture->type, glTexture->texture);
    }
}
//------------------------------------------------------------------------------
void xxSetVertexSamplersGLES2(uint64_t commandBuffer, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetFragmentSamplersGLES2(uint64_t commandBuffer, int count, const uint64_t* samplers)
{

}
//------------------------------------------------------------------------------
void xxSetVertexConstantBufferGLES2(uint64_t commandBuffer, uint64_t buffer, unsigned int size)
{
    BUFFERGL* glBuffer = reinterpret_cast<BUFFERGL*>(buffer);

    glUniform4fv(0, size / sizeof(float) / 4, (GLfloat*)glBuffer->memory);
}
//------------------------------------------------------------------------------
void xxSetFragmentConstantBufferGLES2(uint64_t commandBuffer, uint64_t buffer, unsigned int size)
{

}
//------------------------------------------------------------------------------
void xxDrawIndexedGLES2(uint64_t commandBuffer, int indexCount, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    //glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (char*)nullptr + firstIndex * sizeof(int));
    glDrawElementsBaseVertex(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (char*)nullptr + firstIndex * sizeof(int), vertexOffset);
}
//==============================================================================
//  Fixed-Function
//==============================================================================
void xxSetTransformGLES2(uint64_t commandBuffer, const float* world, const float* view, const float* projection)
{

}
//==============================================================================
