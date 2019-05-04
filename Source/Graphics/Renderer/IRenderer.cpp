#include "B2D_pch.h"
#include "IRenderer.h"

#include "GameEngine.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"

#include <GL/glew.h>
#include "Graphics/OpenGL/OpenGLTexture.h"
#include "Graphics/OpenGL/OpenGLRenderTarget.h"
#include "imgui/imgui.h"

void IRenderer::Init()
{
    IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();
    mRenderTexture1 = ghi->CreateTexture(nullptr, 1920, 1080, 3);
    mRenderTexture2 = ghi->CreateTexture(nullptr, 1920, 1080, 3);
    mRenderTarget1 = ghi->CreateRenderTarget(mRenderTexture1);
    mRenderTarget2 = ghi->CreateRenderTarget(mRenderTexture2);
}

void IRenderer::Shutdown()
{
    IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();
    ghi->DeleteRenderTarget(mRenderTarget1, true);
    ghi->DeleteRenderTarget(mRenderTarget2, true);
}

void IRenderer::Render()
{
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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    mMutex.lock();
    mRenderToSwtich.store(!mRenderToSwtich);
    mMutex.unlock();
}

GHITexture* IRenderer::GetRenderOutput()
{
    return mRenderToSwtich ? mRenderTexture2 : mRenderTexture1;
}
