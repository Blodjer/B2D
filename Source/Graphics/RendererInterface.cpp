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
     IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();
     mRenderTexture1 = ghi->CreateTexture(nullptr, 1920, 1080, 3);
     mRenderTexture2 = ghi->CreateTexture(nullptr, 1920, 1080, 3);
}

IRenderer::~IRenderer()
{
    IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();
    ghi->FreeTexture(mRenderTexture1);
    ghi->FreeTexture(mRenderTexture2);
}

void IRenderer::Render()
{
    static bool b = true;
    static bool b2 = true;
    if (b)
    {
        IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();
        mRenderTarget1 = ghi->CreateRenderTarget(mRenderTexture1);
        mRenderTarget2 = ghi->CreateRenderTarget(mRenderTexture2);

        b = false;
    }
    else if (b2)
    {
        IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();
        mRenderTarget1 = ghi->CreateRenderTarget(mRenderTexture1);
        mRenderTarget2 = ghi->CreateRenderTarget(mRenderTexture2);

        b2 = false;
    }

    if (ShouldRenderNextFrame())
    {
        using duration = std::chrono::duration<float, std::milli>;
        using clock = std::chrono::high_resolution_clock;

        std::chrono::time_point<clock> start = clock::now();

        GHIRenderTarget* const target = mRenderToSwtich ? mRenderTarget1 : mRenderTarget2;

        PreRender();
        RenderInternal(target);
        PostRender();

        duration ChronoTick = clock::now() - start;
        mRenderTime = ChronoTick.count();
    }
}

void IRenderer::PreRender()
{

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
