#include "B2D_pch.h"
#include "RendererInterface.h"

#include "GameEngine.h"
#include "GHI/GraphicsHardwareInterface.h"

#include <GL/glew.h>
#include "OpenGL/OpenGLTexture.h"
#include "OpenGL/OpenGLRenderTarget.h"
#include "imgui/imgui.h"

IRenderer::IRenderer()
{
     IGraphicsHardwareInterface* ghi = CGameEngine::Instance()->GetGHI();
     mRenderTexture1 = ghi->CreateTexture(nullptr, 1920, 1080, 3);
     mRenderTexture2 = ghi->CreateTexture(nullptr, 1920, 1080, 3);
}

IRenderer::~IRenderer()
{
    IGraphicsHardwareInterface* ghi = CGameEngine::Instance()->GetGHI();
    ghi->FreeTexture(mRenderTexture1);
    ghi->FreeTexture(mRenderTexture2);
}

void IRenderer::Render()
{
    static bool b = true;
    static bool b2 = true;
    if (b)
    {
        IGraphicsHardwareInterface* ghi = CGameEngine::Instance()->GetGHI();
        mRenderTarget1 = ghi->CreateRenderTarget(mRenderTexture1);
        mRenderTarget2 = ghi->CreateRenderTarget(mRenderTexture2);

        b = false;
    }
    else if (b2)
    {
        IGraphicsHardwareInterface* ghi = CGameEngine::Instance()->GetGHI();
        mRenderTarget1 = ghi->CreateRenderTarget(mRenderTexture1);
        mRenderTarget2 = ghi->CreateRenderTarget(mRenderTexture2);

        b2 = false;
    }

    if (ShouldRenderNextFrame())
    {
        PreRender();
        RenderInternal();
        PostRender();
    }
}

void IRenderer::PreRender()
{
    CGameEngine::Instance()->GetGHI()->BindRenderTargetAndClear(mRenderToSwtich ? mRenderTarget1 : mRenderTarget2);
}

void IRenderer::PostRender()
{
    glFinish();
    mMutex.lock();
    mRenderToSwtich.store(!mRenderToSwtich);
    mMutex.unlock();
}

GHITexture* IRenderer::GetRenderOutput()
{
    return mRenderToSwtich ? mRenderTexture2 : mRenderTexture1;
}
