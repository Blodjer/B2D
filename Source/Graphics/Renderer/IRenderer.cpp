#include "B2D_pch.h"
#include "IRenderer.h"

#include "GameEngine.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "Graphics/Viewport.h"

void IRenderer::Init()
{
    IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();
    GHITexture* renderTexture1 = ghi->CreateTexture(nullptr, 1, 1, 3);
    GHITexture* renderTexture2 = ghi->CreateTexture(nullptr, 1, 1, 3);

    // Add viewport
    // 

    m_isInit = true;
}

void IRenderer::Shutdown()
{
    // Clear viewport/images
}

void IRenderer::Render()
{
    if (ShouldRenderNextFrame() && m_viewport)
    {
        using duration = std::chrono::duration<float, std::milli>;
        using clock = std::chrono::high_resolution_clock;

        std::chrono::time_point<clock> start = clock::now();

        B2D_NOT_IMPLEMENTED();

        //PreRender();
        //RenderInternal(target);
        //PostRender();

        duration ChronoTick = clock::now() - start;
        m_renderTime = ChronoTick.count();
    }
}

void IRenderer::PreRender()
{

}

void IRenderer::PostRender()
{
    //glFinish();
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GHITexture const* IRenderer::GetRenderOutput() const
{
    if (!m_isInit)
    {
        return nullptr;
    }

    B2D_NOT_IMPLEMENTED();

    //return m_renderSwtich ? m_renderTarget2->GetTexture() : m_renderTarget1->GetTexture();
}
