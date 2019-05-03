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

WorldRenderer::WorldRenderer()
    : mRenderedFrame(0)
{

}

bool WorldRenderer::ShouldRenderNextFrame()
{
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
