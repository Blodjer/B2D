#include "B2D_pch.h"
#include "RenderThread.h"

#include "Platform/GenericWindow.h"
#include "Renderer/IRenderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

RenderThread::RenderThread(GenericWindow* offscreenRenderContext)
    : m_offscreenRenderContext(offscreenRenderContext)
{

}

RenderThread::~RenderThread()
{
}

void RenderThread::AddRenderer(IRenderer* renderer)
{
    m_mutex.lock();
    m_renderersToAdd.emplace_back(renderer);
    m_mutex.unlock();
}

void RenderThread::RemoveRenderer(IRenderer* renderer)
{
    m_mutex.lock();
    m_renderersToRemove.emplace_back(renderer);
    m_mutex.unlock();
}

bool RenderThread::Init()
{
    m_offscreenRenderContext->MakeContextCurrent();

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
        m_mutex.lock();

        if (!m_renderersToRemove.empty())
        {
            for (IRenderer* renderer : m_renderersToRemove)
            {
                renderer->Shutdown();

                auto it = std::find(m_renderers.begin(), m_renderers.end(), renderer);
                if (it != m_renderers.end())
                {
                    std::iter_swap(it, m_renderers.end() - 1);
                    m_renderers.pop_back();
                }

                delete renderer;
            }
            m_renderersToRemove.clear();
            m_renderersToRemove.resize(0);
        }

        if (!m_renderersToAdd.empty())
        {
            for (IRenderer* const renderer : m_renderersToAdd)
            {
                m_renderers.emplace_back(renderer);
                renderer->Init();
            }
            m_renderersToAdd.clear();
            m_renderersToAdd.resize(0);
        }

        for (IRenderer* const renderer : m_renderers)
        {
            renderer->Render();
        }

        // TODO: Could be further up but is currently required to prevent renderer dependencies getting deleted to early
        m_mutex.unlock();
    }
}

void RenderThread::Shutdown()
{
    
}
