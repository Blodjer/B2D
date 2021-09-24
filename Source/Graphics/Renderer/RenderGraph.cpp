#include "B2D_pch.h"
#include "RenderGraph.h"

#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "Graphics/GHI/GHICommandBuffer.h"
#include "Graphics/GHI/GHITexture.h"
#include "Graphics/GHI/GHISurface.h"

struct GHIGraphicsPipelineDesc
{
    GHIRenderPass* renderPass;
    GraphicsPipelineDesc desc;

    bool operator==(GHIGraphicsPipelineDesc const& other) const
    {
        return renderPass == other.renderPass
            && desc == other.desc;
    }
};

namespace std
{
    template <>
    struct hash<std::vector<GHITexture*>>
    {
        std::size_t operator()(std::vector<GHITexture*> const& vec) const
        {
            std::size_t seed = vec.size();
            for (auto& i : vec)
            {
                seed ^= reinterpret_cast<std::intptr_t>(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

    template <>
    struct hash<RenderTargetDesc>
    {
        std::size_t operator()(RenderTargetDesc const& k) const
        {
            std::size_t seed = std::hash<uint32>()(k.height);
            seed ^= std::hash<uint32>()(k.width) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<uint32>()(static_cast<uint32>(k.format)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };

    template <>
    struct hash<GHIGraphicsPipelineDesc>
    {
        std::size_t operator()(GHIGraphicsPipelineDesc const& k) const
        {
            std::size_t seed = std::hash<void*>()(k.renderPass);
            seed ^= std::hash<PixelShaderRef>()(k.desc.ps) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<VertexShaderRef>()(k.desc.vs) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
}

RenderGraph::RenderGraph(IGraphicsHardwareInterface& ghi)
    : m_ghi(ghi)
{

}

void RenderGraph::AddPass(std::function<void(RenderGraphPassBuilder&)> setup, std::function<void(GHICommandBuffer&, PassContext&)> execution)
{
    RenderPassDesc r;
    r.setupFunction = setup;
    r.executionFunction = execution;

    m_renderPassDescs.emplace_back(r);
}

void RenderGraph::AddPresent(RenderResourcePtr const& output, GHISurface* surface)
{
    PresentDesc p;
    p.output = output;
    p.surface = surface;
    m_presentDescs.emplace_back(p);
}

RenderResourcePtr const RenderGraph::CreateRenderTarget(RenderTargetDesc const& desc)
{
    m_renderTargetDescs.emplace_back(desc);
    return RenderResourcePtr(m_renderTargetDescs.size() - 1);
}

RenderResourcePtr const RenderGraph::CreateGraphicsPipeline(GraphicsPipelineDesc const& desc)
{
    m_graphicsPipelineDescs.emplace_back(desc);
    return RenderResourcePtr(m_graphicsPipelineDescs.size() - 1);
}

void RenderGraph::Prepare()
{
    static std::unordered_map<RenderTargetDesc, std::queue<GHITexture*>> s_ghiRenderTargetCache;
    static std::unordered_map<std::vector<GHITexture*>, GHIRenderPass*> s_ghiRenderPassCache;
    static std::unordered_map<GHIGraphicsPipelineDesc, GHIGraphicsPipeline*> s_ghiGraphicsPipelineCache;

    /*
    RenderTarget cache (multiple)
        key:    RenderTargetDesc
        value:  GHITexture* (multiple instances)

    RenderPass cache (unique)
        key:    GHITexture*[] (TODO: change to RenderPassDesc { outputs, depth, inputs } )
        value:  GHIRenderPass* (unique)
        GHI -> Caches render pass and can reuse with framebuffers (still returns unique render pass)

    GraphicsPipeline cache (unique)
        key:    GHIRenderPass* + GraphicsPipelineDesc
        value:  GHIGraphicsPipeline* (not unique)
        GHI -> Can return compatible graphics pipeline
    */

    // Setup

    std::vector<RenderGraphPassBuilder> passBuilders(m_renderPassDescs.size());
    for (uint i = 0; i < passBuilders.size(); ++i)
    {
        m_renderPassDescs[i].setupFunction(passBuilders[i]);
    }

    // Create actual render targets (cached)

    for (RenderTargetDesc const& desc : m_renderTargetDescs)
    {
        auto it = s_ghiRenderTargetCache.find(desc);
        if (it != s_ghiRenderTargetCache.end())
        {
            m_ghiRenderTargets.emplace_back(it->second.front());
            it->second.pop();

            if (it->second.empty())
            {
                s_ghiRenderTargetCache.erase(it);
            }
        }
        else
        {
            // TODO: determine usage
            EGHITextureUsageFlags usage = (desc.format == EGHITextureFormat::Depth24 || desc.format == EGHITextureFormat::Depth24Stencil8) ? EGHITextureUsageFlags::DepthStencilAttachment : EGHITextureUsageFlags::ColorAttachment;

            m_ghiRenderTargets.emplace_back(
                m_ghi.CreateTexture(desc.width, desc.height, desc.format, usage)
            );
        }
    }

    // Create render graph passes

    for (uint i = 0; i < passBuilders.size(); ++i)
    {
        RenderGraphPass graphPass;

        // Set execution stage

        graphPass.executionFunction = &m_renderPassDescs[i].executionFunction; // TODO: Does this copy the whole lambda object? Use std::move()?

        // Set render pass

        std::vector<GHITexture*> outputRenderTargets;
        outputRenderTargets.reserve(passBuilders[i].output.size());

        for (RenderResourcePtr const& output : passBuilders[i].output)
        {
            outputRenderTargets.emplace_back(m_ghiRenderTargets[output.m_virtualId]);
        }

        GHITexture* depthRenderTarget = nullptr;
        if (passBuilders[i].depthStencil.IsValid())
        {
            depthRenderTarget = m_ghiRenderTargets[passBuilders[i].depthStencil.m_virtualId];
        }

        std::vector<GHITexture*> key = outputRenderTargets;
        if (depthRenderTarget)
        {
            key.emplace_back(depthRenderTarget);
        }

        GHIRenderPass* ghiRenderPass = nullptr;

        auto it = s_ghiRenderPassCache.find(key);
        if (it != s_ghiRenderPassCache.end())
        {
            ghiRenderPass = it->second;
        }
        else
        {
            ghiRenderPass = m_ghi.CreateRenderPass(outputRenderTargets, depthRenderTarget);
            s_ghiRenderPassCache.insert_or_assign(key, ghiRenderPass);
        }

        graphPass.ghiRenderPass = ghiRenderPass;

        // Set graphic pipelines (cached)

        for (RenderResourcePtr const& graphicsPipeline : passBuilders[i].graphicsPipelines)
        {
            GraphicsPipelineDesc const& desc = m_graphicsPipelineDescs[graphicsPipeline.m_virtualId];

            GHIGraphicsPipelineDesc ghiDesc;
            ghiDesc.desc = desc;
            ghiDesc.renderPass = ghiRenderPass;

            GHIGraphicsPipeline* ghiGraphicsPipeline = nullptr;

            auto it = s_ghiGraphicsPipelineCache.find(ghiDesc);
            if (it != s_ghiGraphicsPipelineCache.end())
            {
                ghiGraphicsPipeline = it->second;
            }
            else
            {
                ghiGraphicsPipeline = m_ghi.CreateGraphicsPipeline(ghiDesc.renderPass, ghiDesc.desc.vs, ghiDesc.desc.ps);
                s_ghiGraphicsPipelineCache.insert_or_assign(ghiDesc, ghiGraphicsPipeline);
            }

            graphPass.context.pipelineCache[graphicsPipeline.m_virtualId] = ghiGraphicsPipeline;
        }

        m_compiledRenderPasses.emplace_back(graphPass);
    }

    // Destroy unused render targets

    // TODO: Destruction needs to be delayed if the resource is still in use

    // OMG WHAT A MONSTER
    for (auto itRenderTargetCache : s_ghiRenderTargetCache)
    {
        while (!itRenderTargetCache.second.empty())
        {
            GHITexture* const renderTarget = itRenderTargetCache.second.front();
            itRenderTargetCache.second.pop();
            
            m_ghi.DestroyTexture(renderTarget);

            // Destroy all render passes using this render target
            for (auto itRenderPassCache = s_ghiRenderPassCache.begin(); itRenderPassCache != s_ghiRenderPassCache.end();)
            {
                bool erased = false;
                for (GHITexture* const passRenderTarget : itRenderPassCache->first)
                {
                    if (passRenderTarget == renderTarget)
                    {
                        // Destroy all pipelines using this render pass
                        for (auto itGraphicsPipeline = s_ghiGraphicsPipelineCache.begin(); itGraphicsPipeline != s_ghiGraphicsPipelineCache.end();)
                        {
                            if (itGraphicsPipeline->first.renderPass == itRenderPassCache->second)
                            {
                                m_ghi.DestroyGraphicsPipeline(itGraphicsPipeline->second);
                                itGraphicsPipeline = s_ghiGraphicsPipelineCache.erase(itGraphicsPipeline);
                            }
                            else
                            {
                                itGraphicsPipeline++;
                            }
                        }

                        m_ghi.DestroyRenderPass(itRenderPassCache->second);
                        itRenderPassCache = s_ghiRenderPassCache.erase(itRenderPassCache);
                        erased = true;

                        break;
                    }
                }

                if (!erased)
                {
                    itRenderPassCache++;
                }
            }
        }
    }
    s_ghiRenderTargetCache.clear();

    // Add new render targets to cache

    for (uint i = 0; i < m_ghiRenderTargets.size(); ++i)
    {
        s_ghiRenderTargetCache[m_renderTargetDescs[i]].push(m_ghiRenderTargets[i]);
    }
}

void RenderGraph::Execute()
{
    std::vector<GHICommandBuffer*> commandBuffers;
    commandBuffers.reserve(m_compiledRenderPasses.size());

    for (uint i = 0; i < m_compiledRenderPasses.size(); ++i)
    {
        commandBuffers.emplace_back(m_ghi.AllocateCommandBuffer());
    }

    uint commandIndex = 0;
    for (RenderGraphPass& pass : m_compiledRenderPasses)
    {
        GHICommandBuffer* commandBuffer = commandBuffers[commandIndex++];
        commandBuffer->Begin();

        m_ghi.BeginRenderPass(pass.ghiRenderPass, commandBuffer);

        (*pass.executionFunction)(*commandBuffer, pass.context);

        m_ghi.EndRenderPass(pass.ghiRenderPass, commandBuffer);

        commandBuffer->End();
    }

    m_ghi.Submit(commandBuffers);

    for (GHICommandBuffer* commandBuffer : commandBuffers)
    {
        m_ghi.FreeCommandBuffer(commandBuffer);
    }

    for (PresentDesc const& present : m_presentDescs)
    {
        present.surface->Present(GetRenderTarget(present.output));
    }
}

GHITexture const* RenderGraph::GetRenderTarget(RenderResourcePtr const& ptr)
{
    return m_ghiRenderTargets[ptr.m_virtualId];
}
