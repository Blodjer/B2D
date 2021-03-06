#include "B2D_pch.h"
#include "OpenGLGHI.h"
#include "OpenGLMaterial.h"
#include "OpenGLRenderTarget.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"

#include <imgui/imgui.h>
#include <Editor/imgui_impl_opengl3.h>

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
                //B2D_LOG_INFO(message);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                B2D_LOG_INFO(message);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                B2D_LOG_WARNING(message);
                break;
            case GL_DEBUG_SEVERITY_HIGH:
                B2D_LOG_ERROR(message);
                break;
            default:
                B2D_LOG_WARNING(message);
                break;
        }
    }, nullptr);

    B2D_LOG_INFO("Initialize OpenGL...");
    B2D_LOG_INFO("GL Version      {}", glGetString(GL_VERSION));
    B2D_LOG_INFO("GL Vendor       {}", glGetString(GL_VENDOR));
    B2D_LOG_INFO("GL Renderer     {}\n", glGetString(GL_RENDERER));

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
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
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
    return CreateRenderTarget(ghiTexture);
}

GHIRenderTarget* OpenGLGHI::CreateRenderTarget(GHITexture* texture)
{
    OpenGLTexture* renderTexture = static_cast<OpenGLTexture*>(texture);

    GLuint depthrenderbuffer;
    glGenTextures(1, &depthrenderbuffer);
    glBindTexture(GL_TEXTURE_2D, depthrenderbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, renderTexture->GetWidth(), renderTexture->GetHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    OpenGLTexture* depthTexture = new OpenGLTexture(depthrenderbuffer, renderTexture->GetWidth(), renderTexture->GetHeight());

    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint fb;
    glGenFramebuffers(1, &fb);

    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture->GetHandle(), 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthrenderbuffer, 0);

    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, DrawBuffers);

    B2D_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return new OpenGLRenderTarget(fb, renderTexture, depthTexture);
}

void OpenGLGHI::ResizeRenderTarget(GHIRenderTarget*& renderTarget, uint32 width, uint32 height)
{
    OpenGLRenderTarget* rt = static_cast<OpenGLRenderTarget*>(renderTarget);
    
    OpenGLTexture* renderTexture = static_cast<OpenGLTexture*>(rt->GetTexture());

    DeleteRenderTarget(renderTarget, true);
    GHITexture* t = CreateTexture(nullptr, width, height, 3);
    renderTarget = CreateRenderTarget(t);
}

void OpenGLGHI::DeleteRenderTarget(GHIRenderTarget*& renderTarget, bool freeTexture)
{
    OpenGLRenderTarget* rt = static_cast<OpenGLRenderTarget*>(renderTarget);
    GLuint handle = rt->GetHandle();

    glDeleteFramebuffers(1, &handle);

    GHITexture* dtexture = const_cast<GHITexture*>(rt->GetDepthTexture());
    FreeTexture(dtexture);

    if (freeTexture)
    {
        GHITexture* texture = const_cast<GHITexture*>(rt->GetTexture());
        FreeTexture(texture);
    }

    delete rt;
    renderTarget = nullptr;
}

void OpenGLGHI::BindRenderTarget(GHIRenderTarget* renderTarget)
{
    OpenGLRenderTarget* rt = static_cast<OpenGLRenderTarget*>(renderTarget);

    glBindFramebuffer(GL_FRAMEBUFFER, rt->GetHandle());

    glViewport(0, 0, rt->GetWidth(), rt->GetHeight());
    glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenGLGHI::BindRenderTargetAndClear(GHIRenderTarget* renderTarget)
{
    BindRenderTarget(renderTarget);

    glClearColor(0.5f, 0.1f, 0.5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GHITexture* OpenGLGHI::CreateTexture(void const* data, uint32 width, uint32 height, uint8 components)
{
    GLenum format;
    switch (components)
    {
        case 1:
            format = GL_RED;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            B2D_LOG_ERROR("Cannot create texture with {} components", components);
            return nullptr;
    }

    GLuint handle;
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    B2D_ASSERT(handle != GL_INVALID_VALUE);

    return new OpenGLTexture(handle, width, height);
}

void OpenGLGHI::BindTexture(GHITexture const* texture)
{
    OpenGLTexture const* tex = static_cast<OpenGLTexture const*>(texture);
    glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
}

void OpenGLGHI::FreeTexture(GHITexture*& texture)
{
    OpenGLTexture const* tex = static_cast<OpenGLTexture*>(texture);
    GLuint handle = tex->GetHandle();
    glDeleteTextures(1, &handle);

    delete texture;
    texture = nullptr;
}

bool OpenGLGHI::CompileShader(char const* code, GLuint type, GLuint& outHandle)
{
    GLuint handle = glCreateShader(type);

    glShaderSource(handle, 1, &code, nullptr);
    glCompileShader(handle);

    int success;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        int messageLength;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &messageLength);

        char* message = static_cast<char*>(alloca(messageLength * sizeof(char)));
        glGetShaderInfoLog(handle, 512, nullptr, message);

        glDeleteShader(handle);

        B2D_LOG_ERROR("Shader Compile Error: {0}", message);
        return false;
    }

    outHandle = handle;
    return true;
}

GHIShader* OpenGLGHI::CreateShader(char const* code, GLuint type)
{
    GLuint handle = 0;
    CompileShader(code, type, handle);
    return new OpenGLShader(handle);
}

GHIShader* OpenGLGHI::CreatePixelShader(char const* code)
{
    return CreateShader(code, GL_FRAGMENT_SHADER);
}

GHIShader* OpenGLGHI::CreateVertexShader(char const* code)
{
    return CreateShader(code, GL_VERTEX_SHADER);
}

void OpenGLGHI::DeleteShader(GHIShader*& shader)
{
    OpenGLShader* sh = static_cast<OpenGLShader*>(shader);
    glDeleteShader(sh->GetHandle());

    delete shader;
    shader = nullptr;
}

GHIMaterial* OpenGLGHI::CreateMaterial(GHIShader* vertexShader, GHIShader* pixelShader)
{
    OpenGLShader* vs = static_cast<OpenGLShader*>(vertexShader);
    OpenGLShader* ps = static_cast<OpenGLShader*>(pixelShader);

    GLuint handle = glCreateProgram();

    glAttachShader(handle, vs->GetHandle());
    glAttachShader(handle, ps->GetHandle());
    glLinkProgram(handle);
    glValidateProgram(handle);

    glDetachShader(handle, vs->GetHandle());
    glDetachShader(handle, ps->GetHandle());

    return new OpenGLMaterial(handle); 
}


void OpenGLGHI::FreeMaterial(GHIMaterial*& material)
{
    OpenGLMaterial* m = static_cast<OpenGLMaterial*>(material);

    glDeleteProgram(m->GetHandle());

    delete material;
    material = nullptr;
}

void OpenGLGHI::BindMaterial(GHIMaterial* material)
{
    OpenGLMaterial* openglMaterial = static_cast<OpenGLMaterial*>(material);

    glUseProgram(openglMaterial->GetHandle());
}

bool OpenGLGHI::ImGui_Init()
{
    ImGui_ImplOpenGL3_Init("#version 410 core");
    return true;
}

void OpenGLGHI::ImGui_Shutdow()
{
    ImGui_ImplOpenGL3_Shutdown();
}

void OpenGLGHI::ImGui_BeginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
}

void OpenGLGHI::ImGui_Render()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
