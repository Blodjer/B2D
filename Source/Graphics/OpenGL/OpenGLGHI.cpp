#include "B2D_pch.h"
#include "OpenGLGHI.h"
#include "OpenGLMaterial.h"
#include "OpenGLRenderTarget.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"

#include <GL/glew.h>

bool OpenGLGHI::Init()
{
    glewInit();

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
    {
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                //B2D_CORE_INFO(message);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                B2D_CORE_INFO(message);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                B2D_CORE_WARNING(message);
                break;
            case GL_DEBUG_SEVERITY_HIGH:
                B2D_CORE_ERROR(message);
                break;
            default:
                B2D_CORE_WARNING(message);
                break;
        }
    }, nullptr);

    B2D_CORE_INFO("Initialize OpenGL...");
    B2D_CORE_INFO("GL Version      {}", glGetString(GL_VERSION));
    B2D_CORE_INFO("GL Vendor       {}", glGetString(GL_VENDOR));
    B2D_CORE_INFO("GL Renderer     {}\n", glGetString(GL_RENDERER));

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.001f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void OpenGLGHI::Shutdown()
{

}

void OpenGLGHI::Clear(bool color, bool depth, bool stencil)
{
    GLbitfield clearFlags = 0;
    if (color)
    {
        glClearColor(0.7f, 0.5, 0.7f, 1);
        clearFlags |= GL_COLOR_BUFFER_BIT;
    }
    if (depth)
    {
        clearFlags |= GL_DEPTH_BUFFER_BIT;
    }
    if (stencil)
    {
        clearFlags |= GL_STENCIL_BUFFER_BIT;
    }

    glClear(clearFlags);
}

GHIRenderTarget* OpenGLGHI::CreateRenderTarget()
{
    GHITexture* ghiTexture = CreateTexture(nullptr, 1920, 1080, 3);
    OpenGLTexture* renderTexture = static_cast<OpenGLTexture*>(ghiTexture);

    GLuint fb;
    glGenFramebuffers(1, &fb);
    OpenGLRenderTarget* renderTarget = new OpenGLRenderTarget(fb, renderTexture);

    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture->GetHandle(), 0);

    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, DrawBuffers);

    B2D_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    return renderTarget;
}

GHIRenderTarget* OpenGLGHI::CreateRenderTarget(GHITexture* texture)
{
    OpenGLTexture* renderTexture = static_cast<OpenGLTexture*>(texture);

    GLuint fb;
    glGenFramebuffers(1, &fb);
    OpenGLRenderTarget* renderTarget = new OpenGLRenderTarget(fb, renderTexture);

    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture->GetHandle(), 0);

    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, DrawBuffers);

    B2D_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    return renderTarget;
}

void OpenGLGHI::DeleteRenderTarget(GHIRenderTarget*& renderTarget)
{
    OpenGLRenderTarget* rt = static_cast<OpenGLRenderTarget*>(renderTarget);
    GLuint handle = rt->GetHandle();

    glDeleteFramebuffers(1, &handle);

    GHITexture* texture = const_cast<GHITexture*>(renderTarget->GetTexture());
    FreeTexture(texture);

    delete rt;
    renderTarget = nullptr;
}

void OpenGLGHI::BindRenderTarget(GHIRenderTarget* renderTarget)
{
    OpenGLRenderTarget* rt = static_cast<OpenGLRenderTarget*>(renderTarget);

    glBindFramebuffer(GL_FRAMEBUFFER, rt->GetHandle());

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1920, 1080);
}

void OpenGLGHI::BindRenderTargetAndClear(GHIRenderTarget* renderTarget)
{
    BindRenderTarget(renderTarget);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

GHITexture* OpenGLGHI::CreateTexture(void* data, uint32 width, uint32 height, uint8 components)
{
    OpenGLTexture* texture = new OpenGLTexture();
    texture->Create(data, width, height, components);

    return texture;
}

void OpenGLGHI::BindTexture(GHITexture const* texture)
{
    OpenGLTexture const* tex = static_cast<OpenGLTexture const*>(texture);
    glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
}

void OpenGLGHI::FreeTexture(GHITexture*& texture)
{
    texture->Free();
    delete texture;
    texture = nullptr;
}

GHIShader* OpenGLGHI::CreatePixelShader(char* code)
{
    OpenGLShader* shader = new OpenGLShader();
    shader->Create(code, GL_FRAGMENT_SHADER);

    return shader;
}

void OpenGLGHI::DeleteShader(GHIShader*& shader)
{
    OpenGLShader* sh = static_cast<OpenGLShader*>(shader);
    sh->Delete();
    delete shader;
    shader = nullptr;
}

GHIShader* OpenGLGHI::CreateVertexShader(char* code)
{
    OpenGLShader* shader = new OpenGLShader();
    shader->Create(code, GL_VERTEX_SHADER);

    return shader;
}

GHIMaterial* OpenGLGHI::CreateMaterial(GHIShader* vertexShader, GHIShader* pixelShader)
{
    return new OpenGLMaterial(static_cast<OpenGLShader*>(vertexShader), static_cast<OpenGLShader*>(pixelShader));
}

void OpenGLGHI::BindMaterial(GHIMaterial* material)
{
    OpenGLMaterial* openglMaterial = static_cast<OpenGLMaterial*>(material);

    glUseProgram(openglMaterial->GetHandle());
}
