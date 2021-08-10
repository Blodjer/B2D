#include "B2D_pch.h"
#include "RenderManager.h"

#include "GameEngine.h"
#include "Editor/EditorModule.h"
#include "Engine/ModuleManager.h"
#include "GHI/GHICommandList.h"
#include "Platform/GenericWindow.h"
#include "Platform/PlatformInterface.h"
#include "Graphics/Renderer/IRenderer.h"
#include "Graphics/Renderer/RenderGraph.h"
#include "RenderThread.h"
#include "Core/Thread/Thread.h"
#include "Viewport.h"

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
    B2D_ASSERT(ghi);

    RenderGraph rg = RenderGraph(*ghi);

    uint32 targetWidth = GameEngine::Instance()->GetMainWindow()->GetViewport()->GetWidth();
    uint32 targetHeight = GameEngine::Instance()->GetMainWindow()->GetViewport()->GetHeight();

    RenderTargetDesc desc;
    desc.width = targetWidth;
    desc.height = targetHeight;

    RenderTargetDesc desc2;
    desc2.width = 200;
    desc2.height = 200;

    RenderResourcePtr const rt0 = rg.CreateRenderTarget(desc);
    RenderResourcePtr const rt1 = rg.CreateRenderTarget(desc2);
    RenderResourcePtr const rt2 = rg.CreateRenderTarget(desc2);
    RenderResourcePtr const rt3 = rg.CreateRenderTarget(desc2);
    // TODO: Define clear, input,...

    static bool ii = false;
    if (ii)
    {
        rg.AddPass(
            [=](RenderGraphPassBuilder& rgb)
            {
                rgb.AddOutput(rt1);
            },
            [](GHICommandList& cb)
            {
                cb.DrawTest();
            }
            );

        ii = false;
    }
    else
    {
        ii = true;
    }

    if (EditorModule* const editor = GameEngine::Instance()->GetModuleManager()->Get<EditorModule>())
    {
        rg.AddPass(
            [=](RenderGraphPassBuilder& rgb)
            {
                rgb.AddOutput(rt0);
            },
            [editor](GHICommandList& cb)
            {
                cb.DrawTest();

                // TODO: ImGui tries to access the render outputs from the renderers. This draw is called from the main thread but the render thread might have already cleared the render output
                // S1 Change ImGUI to always lookup the latest render output
                // S2 Change render manager to lock all needed textures before drawing. This way the renderer would have to wait for editor to be finish drawing.
                editor->Draw(cb);
            }
        );
    }

    rg.Prepare();
    rg.Execute();

    if (!GameEngine::Instance()->GetMainWindow()->IsMinimized())
    {
        GameEngine::Instance()->GetMainWindow()->GetSurface()->Present(rg.GetRenderTarget(rt0));
    }
}
