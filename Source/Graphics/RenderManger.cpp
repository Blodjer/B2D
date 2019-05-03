#include "B2D_pch.h"
#include "RenderManger.h"

#include "Editor/EditorModule.h"
#include "Engine/ModuleManager.h"
#include "GameEngine.h"
#include "GHI/GraphicsHardwareInterface.h"
#include "Platform/GenericWindow.h"
#include "Platform/PlatformInterface.h"
#include "RendererInterface.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

RenderManger::RenderManger()
    : mMultithreaded(false)
{

}

RenderManger::~RenderManger()
{

}

void RenderManger::Init(bool multithreaded)
{
    mMultithreaded = multithreaded;
    if (mMultithreaded)
    {
        glfwMakeContextCurrent(nullptr);
        mRenderThread = std::thread(&RenderManger::RenderLoop, this);
    }
}

void RenderManger::Shutdown()
{
    for (IRenderer* renderer : mRenderers)
    {
        DeleteRenderer(renderer);
    }
}

void RenderManger::Render()
{
    if (mMultithreaded)
    {
        return;
    }

    mOffscreenRenderContext = GameEngine::Instance()->GetPA()->CreateOffscreenRenderContext();
    mRenderThread = std::thread(&RenderManger::RenderLoop, this);
    mMultithreaded = true;
    return;

    RenderInternal();
}

void RenderManger::RenderLoop()
{
    mOffscreenRenderContext->MakeContextCurrent();

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

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.4f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (true)
    {
        RenderInternal();
    }
}

void RenderManger::RenderInternal()
{
    for (IRenderer* const renderer : mRenderers)
    {
        renderer->Render();
    }
}

void RenderManger::Draw()
{
    IGraphicsHardwareInterface* const ghi = GameEngine::Instance()->GetGHI();
    ghi->Clear(true, true, true);
    
    if (EditorModule* const editor = GameEngine::Instance()->GetModuleManager()->Get<EditorModule>())
    {
        editor->Draw();
    }

    GameEngine::Instance()->GetMainWindow()->Swap();
}

//     for (IRenderer* const renderer : mRenderers)
//     {
//         ghi->BindMaterial(rtMat->GetGHIMaterial());
//         
//         //ghi->BindTexture(renderTarget->GetTexture());
//         renderer->mMutex.lock();
//         OpenGLTexture const* tex = static_cast<OpenGLTexture const*>(renderer->GetRenderOutput());
//         glActiveTexture(GL_TEXTURE0 + 0);
//         glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
// 
//         glBindVertexArray(VAO);
// 
//         CViewport* viewport = CGameEngine::Instance()->GetMainWindow()->GetViewport();
// 
//         glViewport(0, 0, viewport->GetWidth(), viewport->GetHeight());
// 
//         glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
// 
// //         uint32 w = viewport->GetWidth() * 0.5f;
// //         uint32 h = viewport->GetHeight() * 0.5f;
//         
// //         glViewport(0, 0, w, h);
// //         glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
// //         
// //         glViewport(w, 0, w, h);
// //         glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
// //         
// //         glViewport(0, h, w, h);
// //         glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
// //         
// //         glViewport(w, h, w, h);
// //         glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//         renderer->mMutex.unlock();
//     }
