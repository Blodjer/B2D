#include "B2D_pch.h"
#include "IRenderer.h"

#include "GameEngine.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"

#include <GL/glew.h>
#include "Graphics/OpenGL/OpenGLTexture.h"
#include "Graphics/OpenGL/OpenGLRenderTarget.h"
#include "imgui/imgui.h"
#include "../Viewport.h"

void IRenderer::Init()
{
    IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();
    GHITexture* renderTexture1 = ghi->CreateTexture(nullptr, 1, 1, 3);
    GHITexture* renderTexture2 = ghi->CreateTexture(nullptr, 1, 1, 3);
    m_renderTarget1 = ghi->CreateRenderTarget(renderTexture1);
    m_renderTarget2 = ghi->CreateRenderTarget(renderTexture2);

    m_isInit = true;
}

void IRenderer::Shutdown()
{
    IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();
    ghi->DeleteRenderTarget(m_renderTarget1, true);
    ghi->DeleteRenderTarget(m_renderTarget2, true);
}

void IRenderer::Render()
{
    if (ShouldRenderNextFrame() && m_viewport)
    {
        using duration = std::chrono::duration<float, std::milli>;
        using clock = std::chrono::high_resolution_clock;

        std::chrono::time_point<clock> start = clock::now();

        GHIRenderTarget*& target = m_renderSwtich ? m_renderTarget1 : m_renderTarget2;

        if (target->GetWidth() != m_viewport->GetWidth() || target->GetHeight() != m_viewport->GetHeight())
        {
            GameEngine::Instance()->GetGHI()->ResizeRenderTarget(target, m_viewport->GetWidth(), m_viewport->GetHeight());
        }

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

    m_renderSwtich.store(!m_renderSwtich);
}

GHITexture const* IRenderer::GetRenderOutput() const
{
    if (!m_isInit)
    {
        return nullptr;
    }

    return m_renderSwtich ? m_renderTarget2->GetTexture() : m_renderTarget1->GetTexture();
}
