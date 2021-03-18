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
    m_multithreaded = multithreaded;
    if (m_multithreaded)
    {
        GenericWindow* offscreenRenderContext = GameEngine::Instance()->GetPAI()->CreateOffscreenRenderContext();
        m_renderThreadRunnable = new RenderThread(offscreenRenderContext);
        m_renderThread = Thread::Create(m_renderThreadRunnable);
    }
}

void RenderManger::Shutdown()
{
    for (IRenderer* renderer : m_renderers)
    {
        DeleteRenderer(renderer);
    }
}

void RenderManger::Tick(float deltaTime)
{
    if (m_multithreaded)
    {
        return;
    }

    // TODO: We just render manually here
    for (IRenderer* const renderer : m_renderers)
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
