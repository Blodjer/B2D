#include "B2D_pch.h"
#include "RenderThread.h"

#include "Platform/GenericWindow.h"
#include "Renderer/IRenderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

RenderThread::RenderThread(GenericWindow* offscreenRenderContext)
    : mOffscreenRenderContext(offscreenRenderContext)
{

}

RenderThread::~RenderThread()
{
}

void RenderThread::AddRenderer(IRenderer* renderer)
{
    mMutex.lock();
    mRenderersToAdd.emplace_back(renderer);
    mMutex.unlock();
}

void RenderThread::RemoveRenderer(IRenderer* renderer)
{
    mMutex.lock();
    mRenderersToRemove.emplace_back(renderer);
    mMutex.unlock();
}

bool RenderThread::Init()
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

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.001f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void RenderThread::Run()
{
    while (true)
    {
        mMutex.lock();

        if (!mRenderersToRemove.empty())
        {
            for (IRenderer* renderer : mRenderersToRemove)
            {
                renderer->Shutdown();

                auto it = std::find(mRenderers.begin(), mRenderers.end(), renderer);
                if (it != mRenderers.end())
                {
                    std::iter_swap(it, mRenderers.end() - 1);
                    mRenderers.pop_back();
                }

                delete renderer;
            }
            mRenderersToRemove.clear();
            mRenderersToRemove.resize(0);
        }

        if (!mRenderersToAdd.empty())
        {
            for (IRenderer* const renderer : mRenderersToAdd)
            {
                mRenderers.emplace_back(renderer);
                renderer->Init();
            }
            mRenderersToAdd.clear();
            mRenderersToAdd.resize(0);
        }

        mMutex.unlock();

        for (IRenderer* const renderer : mRenderers)
        {
            renderer->Render();
        }
    }
}

void RenderThread::Shutdown()
{
    
}
