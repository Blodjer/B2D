#include "B2D_pch.h"
#include "IRenderer.h"

#include "GameEngine.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"

#include <GL/glew.h>
#include "Graphics/OpenGL/OpenGLTexture.h"
#include "Graphics/OpenGL/OpenGLRenderTarget.h"
#include "imgui/imgui.h"

void IRenderer::Init()
{
    IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();
    m_renderTexture1 = ghi->CreateTexture(nullptr, 1920, 1080, 3);
    m_renderTexture2 = ghi->CreateTexture(nullptr, 1920, 1080, 3);
    m_renderTarget1 = ghi->CreateRenderTarget(m_renderTexture1);
    m_renderTarget2 = ghi->CreateRenderTarget(m_renderTexture2);
}

void IRenderer::Shutdown()
{
    IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();
    ghi->DeleteRenderTarget(m_renderTarget1, true);
    ghi->DeleteRenderTarget(m_renderTarget2, true);
}

void IRenderer::Render()
{
    if (ShouldRenderNextFrame())
    {
        using duration = std::chrono::duration<float, std::milli>;
        using clock = std::chrono::high_resolution_clock;

        std::chrono::time_point<clock> start = clock::now();

        GHIRenderTarget* const target = m_renderToSwtich ? m_renderTarget1 : m_renderTarget2;

        PreRender();
        RenderInternal(target);
        PostRender();

        duration ChronoTick = clock::now() - start;
        m_renderTime = ChronoTick.count();
    }
}

void IRenderer::PreRender()
{

}

void IRenderer::PostRender()
{
    glFinish();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_mutex.lock();
    m_renderToSwtich.store(!m_renderToSwtich);
    m_mutex.unlock();
}

GHITexture* IRenderer::GetRenderOutput()
{
    return m_renderToSwtich ? m_renderTexture2 : m_renderTexture1;
}
