#include "B2D_pch.h"
#include "WorldRenderer.h"

#include "GameEngine.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"

WorldRenderer::WorldRenderer()
    : mROBuffer(100000)
    , mPreparedFrame(0)
    , mRenderedFrame(0)
{
    IGraphicsHardwareInterface* ghi = CGameEngine::Instance()->GetGHI();
    mRenderTarget = ghi->CreateRenderTarget();
}

WorldRenderer::~WorldRenderer()
{
    IGraphicsHardwareInterface* ghi = CGameEngine::Instance()->GetGHI();
    ghi->CreateRenderTarget();
}
