#include "B2D_pch.h"
#include "WorldRenderer.h"

#include "GameEngine.h"
#include "../Renderer.h"
#include "../Viewport.h"
#include "WorldRenderDataInterface.h"
#include "Game/GameInstance.h"
#include "Game/Core/World.h"
#include "Platform/GenericWindow.h"
#include "../GHI/GraphicsHardwareInterface.h"

void WorldRenderer::Init()
{
    IRenderer::Init();
}

void WorldRenderer::Shutdown()
{
    IRenderer::Shutdown();
}

bool WorldRenderer::ShouldRenderNextFrame()
{
    if (!GameEngine::Instance()->GetGameInstance() || !GameEngine::Instance()->GetGameInstance()->GetWorld())
    {
        return false;
    }

    m_wrdi = GameEngine::Instance()->GetGameInstance()->GetWorld()->GetWorldRenderDataInterface();
    return m_wrdi->GetPreparedFrame() != m_renderedFrame;
}

void WorldRenderer::RenderInternal(GHIRenderTarget* const renderTarget)
{
    m_wrdi = GameEngine::Instance()->GetGameInstance()->GetWorld()->GetWorldRenderDataInterface();

    m_wrdi->StartRead();
    CRenderer::RenderWorldFromViewportToRenderTarget(renderTarget, m_wrdi, m_viewport);
    m_renderedFrame.store(m_wrdi->GetPreparedFrame());
    m_wrdi->StopRead();

    //CRenderer::PostProcessPass(renderTarget, renderTarget, nullptr);
}
