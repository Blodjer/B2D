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

    mWRDI = GameEngine::Instance()->GetGameInstance()->GetWorld()->GetWorldRenderDataInterface();
    return mWRDI->GetPreparedFrame() != mRenderedFrame;
}

void WorldRenderer::RenderInternal(GHIRenderTarget* const renderTarget)
{
    mWRDI = GameEngine::Instance()->GetGameInstance()->GetWorld()->GetWorldRenderDataInterface();
    mViewport = GameEngine::Instance()->GetMainWindow()->GetViewport();

    mWRDI->StartRead();

    static GHIRenderTarget* rt = GameEngine::Instance()->GetGHI()->CreateRenderTarget();

    CRenderer::RenderWorldFromViewportToRenderTarget(rt, mWRDI, mViewport, mViewport->GetCamera());

    CRenderer::PostProcessPass(rt, renderTarget, nullptr);

    mRenderedFrame.store(mWRDI->GetPreparedFrame());

    mWRDI->StopRead();
}
