#include "B2D_pch.h"
#include "OpenGLGHI.h"
#include "OpenGLTexture.h"

#include <GL/glew.h>

OpenGLGHI::OpenGLGHI()
{

}

OpenGLGHI::~OpenGLGHI()
{

}

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
                B2D_CORE_INFO(message);
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
    glAlphaFunc(GL_GREATER, 0.4f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void OpenGLGHI::Shutdown()
{

}

void OpenGLGHI::Clear()
{
    glClearColor(0.7f, 0, 0.7f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GHITexture* OpenGLGHI::CreateTexture(void* data, uint32 width, uint32 height)
{
    OpenGLTexture* texture = new OpenGLTexture();
    texture->Create(data, width, height);

    return texture;
}

void OpenGLGHI::FreeTexture(GHITexture* texture)
{
    texture->Free();
    delete texture;
}
