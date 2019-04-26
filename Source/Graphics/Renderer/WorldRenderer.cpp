#include "B2D_pch.h"
#include "WorldRenderer.h"

#include "GameEngine.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "../Renderer.h"
#include "../Viewport.h"

WorldRenderer::WorldRenderer()
    : mRenderObjectBuffer(100000)
    , mPreparedFrame(0)
    , mRenderedFrame(0)
{
    IGraphicsHardwareInterface* ghi = CGameEngine::Instance()->GetGHI();
    mRenderTarget = ghi->CreateRenderTarget();
}

WorldRenderer::~WorldRenderer()
{
    IGraphicsHardwareInterface* ghi = CGameEngine::Instance()->GetGHI();
    ghi->DeleteRenderTarget(mRenderTarget);
}

void WorldRenderer::Render()
{
    if (!PendingRendering())
    {
        return;
    }

    Lock();

    CRenderer* r = CGameEngine::Instance()->GetRenderer();

    r->BeginRender();
    r->DrawSceneToRenderTarget(mRenderTarget, mRenderObjectBuffer, mViewport, mViewport->GetCamera());
    r->EndRender();

    mRenderedFrame.store(mPreparedFrame);

    Unlock();
}
