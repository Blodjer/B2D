#include "B2D_pch.h"
#include "RenderManager.h"

#include "GameEngine.h"
#include "GHI/GraphicsHardwareInterface.h"
#include "Editor/EditorModule.h"
#include "Engine/ModuleManager.h"
#include "GHI/GHICommandBuffer.h"
#include "Platform/GenericWindow.h"
#include "Platform/PlatformInterface.h"
#include "Graphics/Renderer/IRenderer.h"
#include "Graphics/Renderer/RenderGraph.h"
#include "RenderThread.h"
#include "Core/Thread/Thread.h"
#include "Viewport.h"
#include "GHI/GHITexture.h"
#include "GHI/GHIBuffer.h"
#include "GHI/GHIResourceSet.h"

#include "Core/Resource.h" // TMP
#include "Mesh.h" // TMP
#include "Texture.h" // TMP

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
    static auto meshPtr2 = IResourceManager::Get<Mesh>("Content/Mesh/bunny.obj");

    static auto textureKid = IResourceManager::Get<CTexture>("Content/Textures/SuccessKid.png");
    static auto textureViking = IResourceManager::Get<CTexture>("Content/Textures/viking_room.png");

    VertexShaderRef vs = IResourceManager::Get<VertexShader>("Content/Shader/Vulkan.vs.glsl");
    PixelShaderRef ps = IResourceManager::Get<PixelShader>("Content/Shader/Vulkan.fs.glsl");
    PixelShaderRef ps2 = IResourceManager::Get<PixelShader>("Content/Shader/Vulkan2.fs.glsl");

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

    struct RenderData
    {
        TMatrix viewProjectionMatrix;
    } constants;

    static GHIBuffer* ubVPBuffer1 = ghi->CreateBuffer(EGHIBufferType::UniformBuffer, sizeof(RenderData));
    static GHIBuffer* ubVPBuffer2 = ghi->CreateBuffer(EGHIBufferType::UniformBuffer, sizeof(RenderData));

    if (!GameEngine::Instance()->GetMainWindow()->GetViewport()->GetViewProjectionMatrix(constants.viewProjectionMatrix))
    {
        constants.viewProjectionMatrix = TMatrix::Identity;
    }
    ubVPBuffer1->Upload(&constants, sizeof(constants));

    static float f = 0.0f;
    f += 0.001f;
    float sinFloat = UMath::Sin(f);

    static GHIBuffer* ubFloatBuffer = ghi->CreateBuffer(EGHIBufferType::UniformBuffer, sizeof(sinFloat));
    ubFloatBuffer->Upload(&sinFloat, sizeof(f));

    float sinFloat3[] = { UMath::Sin(f), UMath::Cos(f + UMath::PI * 0.5f), 0.5f };
    static GHIBuffer* ubFloat3Buffer = ghi->CreateBuffer(EGHIBufferType::UniformBuffer, sizeof(sinFloat3));
    ubFloat3Buffer->Upload(&sinFloat3, sizeof(sinFloat3));

    static GHIResourceSet* rs0 = nullptr;
    if (!rs0)
    {
        rs0 = ghi->CreateResourceSet();
        rs0->Bind(0, ubVPBuffer1);
        //rs0->Bind(1, ubFloat3Buffer);
    }

    static GHIResourceSet* rs0c = nullptr;
    if (!rs0c)
    {
        rs0c = ghi->CreateResourceSet();
        rs0c->Bind(0, ubVPBuffer1);
        rs0c->Bind(2, ubFloat3Buffer);
    }

    static GHIResourceSet* rs1 = nullptr;
    if (!rs1)
    {
        rs1 = ghi->CreateResourceSet();
        rs1->Bind(0, ubFloatBuffer);
    }

    static GHIResourceSet* rs2 = nullptr;
    if (!rs2)
    {
        GHISampler* sampler = ghi->CreateSampler();
        rs2 = ghi->CreateResourceSet();
        rs2->Bind(0, textureViking->GetGHITexture(), sampler);
    }

    static GHIResourceSet* rs4 = nullptr;
    if (!rs4)
    {
        GHISampler* sampler = ghi->CreateSampler();
        rs4 = ghi->CreateResourceSet();
        rs4->Bind(0, textureKid->GetGHITexture(), sampler);
    }

    static GHIBuffer* vBuffer = nullptr;
    if (!vBuffer)
    {
        Mesh::Vertex quad[6] = {
            { TVec3(-0.5f, -0.5f, 1), TVec3::Up, TVec2(1, 0) },
            { TVec3(0.5f, -0.5f, 1), TVec3::Up, TVec2(0, 0) },
            { TVec3(0.5f, 0.5f, 1), TVec3::Up, TVec2(0, 1) },
            { TVec3(-0.5f, -0.5f, 1), TVec3::Up, TVec2(1, 0) },
            { TVec3(0.5f, 0.5f, 1), TVec3::Up, TVec2(0, 1) },
            { TVec3(-0.5f, 0.5f, 1), TVec3::Up, TVec2(1, 1) },
        };

        vBuffer = ghi->CreateBuffer(EGHIBufferType::VertexBuffer, sizeof(quad));
        vBuffer->Upload(quad, sizeof(quad));
    }

    //auto p0 = rg.CreatePipeline(vs, ps);

    rg.AddPass(
        [=](RenderGraphPassBuilder& rgb)
        {
            rgb.AddOutput(rt0);
            rgb.AddDepthStencil(depth);
            //rgb.RegisterPipeline(p0);
        },
        //[](GHICommandBuffer& cb, PipelineCache const& pipelineCache)
        [ghi, vs, ps, ps2](GHICommandBuffer& cb, GHIRenderPass const* renderPass)
        {
            // cb.BindGraphicsPipeline(pipelineCache[p0]);

            // TODO: Find a better way to create pipelines
            static GHIGraphicsPipeline* graphicsPipeline1 = ghi->CreateGraphicsPipeline(renderPass, vs, ps);
            static GHIGraphicsPipeline* graphicsPipeline2 = ghi->CreateGraphicsPipeline(renderPass, vs, ps2);

            cb.BindGraphicsPipeline(graphicsPipeline1);

            cb.BindResourceSet(0, rs0);
            cb.BindResourceSet(1, rs1);
            cb.BindResourceSet(2, rs2);

            //cb.SetShaderParameter(sizeof(constants), &constants);

            cb.BindVertexBuffer(meshPtr->GetVertexBuffer());
            cb.BindIndexBuffer(meshPtr->GetIndexBuffer());

            //for (uint i = 0; i < 20000; ++i)
            {
                cb.DrawIndexed(meshPtr->GetIndices().size(), 1, 0, 0, 0);
            }

            cb.BindResourceSet(2, rs4);
            cb.BindVertexBuffer(vBuffer);
            cb.Draw(6, 1, 0, 0);

            //cb.BindGraphicsPipeline(graphicsPipeline2);

            //cb.BindResourceSet(0, rs0c);

            //cb.BindVertexBuffer(meshPtr2->GetVertexBuffer());
            //cb.BindIndexBuffer(meshPtr2->GetIndexBuffer());
            //cb.DrawIndexed(meshPtr2->GetIndices().size(), 1, 0, 0, 0);
        }
    );

    if (EditorModule* const editor = GameEngine::Instance()->GetModuleManager()->Get<EditorModule>())
    {
        rg.AddPass(
            [=](RenderGraphPassBuilder& rgb)
            {
                rgb.AddOutput(rt0);
            },
            [editor](GHICommandBuffer& cb, GHIRenderPass const* renderPass)
            {
                // TODO: ImGui tries to access the render outputs from the renderers. This draw is called from the main thread but the render thread might have already cleared the render output
                // S1 Change ImGUI to always lookup the latest render output
                // S2 Change render manager to lock all needed textures before drawing. This way the renderer would have to wait for editor to be finish drawing.
                editor->Draw(cb);
            }
        );
    }

    if (!GameEngine::Instance()->GetMainWindow()->IsMinimized())
    {
        rg.AddPresent(rt0, GameEngine::Instance()->GetMainWindow()->GetSurface());
    }

    rg.Prepare();
    rg.Execute();
}
