#include "B2D_pch.h"
#include "RenderManger.h"

#include "Editor/EditorModule.h"
#include "Engine/ModuleManager.h"
#include "GameEngine.h"
#include "GHI/GraphicsHardwareInterface.h"
#include "Platform/GenericWindow.h"
#include "Platform/PlatformInterface.h"
#include "Graphics/Renderer/IRenderer.h"
#include "RenderThread.h"
#include "Core/Thread/Thread.h"

#include <GLFW/glfw3.h>

RenderManger::RenderManger()
{

}

RenderManger::~RenderManger()
{

}

void RenderManger::Init(bool multithreaded)
{
    mMultithreaded = multithreaded;
    if (mMultithreaded)
    {
        GenericWindow* offscreenRenderContext = GameEngine::Instance()->GetPA()->CreateOffscreenRenderContext();
        mRenderThreadRunnable = new RenderThread(offscreenRenderContext);
        mRenderThread = Thread::Create(mRenderThreadRunnable);
    }
}

void RenderManger::Shutdown()
{
    for (IRenderer* renderer : mRenderers)
    {
        DeleteRenderer(renderer);
    }
}

void RenderManger::Tick(float deltaTime)
{
    if (mMultithreaded)
    {
        return;
    }

    // TODO: We just render manually here
    for (IRenderer* const renderer : mRenderers)
    {
        renderer->Render();
    }
}

void RenderManger::Draw()
{
    IGraphicsHardwareInterface* const ghi = GameEngine::Instance()->GetGHI();
    ghi->Clear(true, true, true);
    
    if (EditorModule* const editor = GameEngine::Instance()->GetModuleManager()->Get<EditorModule>())
    {
        editor->Draw();
    }

    GameEngine::Instance()->GetMainWindow()->Swap();
}
