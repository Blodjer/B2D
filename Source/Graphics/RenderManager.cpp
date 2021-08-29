#include "B2D_pch.h"
#include "RenderManager.h"

#include "GameEngine.h"
#include "GHI/GraphicsHardwareInterface.h"
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
#include "GHI/GHITexture.h"
#include "GHI/GHIBuffer.h"

#include "Core/Resource.h" // TMP
#include "Mesh.h" // TMP

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

    static auto meshPtr = IResourceManager::Get<Mesh>("Content/Mesh/viking_room.obj");

    RenderGraph rg = RenderGraph(*ghi);

    uint32 targetWidth = GameEngine::Instance()->GetMainWindow()->GetViewport()->GetWidth();
    uint32 targetHeight = GameEngine::Instance()->GetMainWindow()->GetViewport()->GetHeight();

    RenderTargetDesc desc;
    desc.width = targetWidth;
    desc.height = targetHeight;
    desc.format = EGHITextureFormat::BGRA8;

    RenderTargetDesc depthDesc;
    depthDesc.width = targetWidth;
    depthDesc.height = targetHeight;
    depthDesc.format = EGHITextureFormat::Depth24Stencil8;

    RenderTargetDesc desc2;
    desc2.width = 200;
    desc2.height = 200;
    desc2.format = EGHITextureFormat::BGRA8;

    RenderTargetDesc desc3;
    desc3.width = targetWidth;
    desc3.height = targetHeight;
    desc3.format = EGHITextureFormat::RGBA8;

    RenderResourcePtr const rt0 = rg.CreateRenderTarget(desc);
    RenderResourcePtr const rt3 = rg.CreateRenderTarget(desc3);
    RenderResourcePtr const rt1 = rg.CreateRenderTarget(desc2);
    RenderResourcePtr const rt2 = rg.CreateRenderTarget(desc2);
    RenderResourcePtr const depth = rg.CreateRenderTarget(depthDesc);
    // TODO: Define clear, input,...

    struct MeshPushConstants
    {
        //glm::vec4 data;
        TMatrix render_matrix;
    } constants;

    static GHIBuffer* ubBuffer = nullptr;
    static GHIBuffer* ubBuffer2 = nullptr;
    if (ubBuffer == nullptr)
    {
        ubBuffer = ghi->CreateBuffer(EGHIBufferType::UniformBuffer, sizeof(MeshPushConstants));
        ubBuffer2 = ghi->CreateBuffer(EGHIBufferType::UniformBuffer, sizeof(MeshPushConstants));
    }

    if (!GameEngine::Instance()->GetMainWindow()->GetViewport()->GetViewProjectionMatrix(constants.render_matrix))
    {
        constants.render_matrix = TMatrix::Identity;
    }
    ubBuffer->Upload(&constants, sizeof(constants));

    float aspect = static_cast<float>(targetWidth) / static_cast<float>(targetHeight);
    constants.render_matrix = TMatrix::Perspective(45.0f, aspect, 0.1f, 10000.0f) * TMatrix::LookAt(TVec3(2.0f, 0, 0.0f), TVec3::Zero, -TVec3::Up);
    ubBuffer2->Upload(&constants, sizeof(constants));

    static GHIBuffer* ubFBuffer = ghi->CreateBuffer(EGHIBufferType::UniformBuffer, sizeof(float));

    static float f = 0.0f;
    f += 0.001f;
    float ff = UMath::Sin(f);
    ubFBuffer->Upload(&ff, sizeof(f));

    static bool b = false;

    rg.AddPass(
        [=](RenderGraphPassBuilder& rgb)
        {
            rgb.AddOutput(rt0);
            rgb.AddDepthStencil(depth);
        },
        [&constants](GHICommandList& cb)
        {
            cb.BindVertexBuffer(meshPtr->GetVertexBuffer());

            cb.BindUniformBuffer(0, ubFBuffer);
            cb.BindUniformBuffer(0, ubBuffer);

            //cb.SetShaderParameter(sizeof(constants), &constants);

            cb.Draw(meshPtr->GetVertices().size(), 1, 0, 0);

            cb.BindUniformBuffer(0, ubBuffer2);

            cb.Draw(meshPtr->GetVertices().size(), 1, 0, 0);
        }
    );

    if (EditorModule* const editor = GameEngine::Instance()->GetModuleManager()->Get<EditorModule>())
    {
        rg.AddPass(
            [=](RenderGraphPassBuilder& rgb)
            {
                rgb.AddOutput(rt0);
            },
            [editor](GHICommandList& cb)
            {
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
