#include "B2D_pch.h"
#include "RenderManager.h"

#include "Editor/EditorModule.h"
#include "Engine/ModuleManager.h"
#include "GameEngine.h"
#include "GHI/GraphicsHardwareInterface.h"
#include "Platform/GenericWindow.h"
#include "Platform/PlatformInterface.h"
#include "Graphics/Renderer/IRenderer.h"
#include "RenderThread.h"
#include "Core/Thread/Thread.h"

RenderManager::RenderManager()
{

}

RenderManager::~RenderManager()
{

}

void RenderManager::Init(bool multithreaded)
{
    m_multithreaded = multithreaded;
    if (m_multithreaded)
    {
        GenericWindow* offscreenRenderContext = GameEngine::Instance()->GetPAI()->CreateOffscreenRenderContext();
        m_renderThreadRunnable = new RenderThread(offscreenRenderContext);
        m_renderThread = Thread::Create(m_renderThreadRunnable);
    }
}

void RenderManager::Shutdown()
{
    for (IRenderer* renderer : m_renderers)
    {
        DeleteRenderer(renderer);
    }
}

void RenderManager::Tick(float deltaTime)
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

void RenderManager::Draw()
{
    IGraphicsHardwareInterface* const ghi = GameEngine::Instance()->GetGHI();
    ghi->Clear(true, true, true);
    
    // TODO: ImGui tries to access the render outputs from the renderers. This draw is called from the main thread but the render thread might have already cleared the render output
    // S1 Change ImGUI to always lookup the latest render output
    // S2 Change render manager to lock all needed textures before drawing. This way the renderer would have to wait for editor to be finish drawing.
    if (EditorModule* const editor = GameEngine::Instance()->GetModuleManager()->Get<EditorModule>())
    {
        editor->Draw();
    }

    GameEngine::Instance()->GetMainWindow()->Swap();
}
