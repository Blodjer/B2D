#include "B2D_pch.h"
#include "WorldRenderer.h"

#include "GameEngine.h"
#include "../Renderer.h"
#include "../Viewport.h"
#include "WorldRenderDataInterface.h"
#include "Game/GameInstance.h"
#include "Game/Core/World.h"
#include "Platform/GenericWindow.h"

WorldRenderer::WorldRenderer()
    : mRenderedFrame(0)
{
}

bool WorldRenderer::ShouldRenderNextFrame()
{
    mWRDI = CGameEngine::Instance()->GetGameInstance()->GetWorld()->GetWorldRenderDataInterface();
    return mWRDI->GetPreparedFrame() != mRenderedFrame;
}

void WorldRenderer::RenderInternal()
{
    mWRDI = CGameEngine::Instance()->GetGameInstance()->GetWorld()->GetWorldRenderDataInterface();
    mViewport = CGameEngine::Instance()->GetMainWindow()->GetViewport();

    mWRDI->StartRead();

    CRenderer::DrawWorldFromViewport(mWRDI, mViewport, mViewport->GetCamera());

    mRenderedFrame.store(mWRDI->GetPreparedFrame());

    mWRDI->StopRead();
}
