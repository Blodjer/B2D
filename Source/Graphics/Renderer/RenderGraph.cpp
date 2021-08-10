#include "B2D_pch.h"
#include "RenderGraph.h"

#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "Graphics/GHI/GHICommandList.h"

namespace std
{
    template <>
    struct hash<std::vector<GHIRenderTarget*>>
    {
        std::size_t operator()(std::vector<GHIRenderTarget*> const& vec) const
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
            return ((std::hash<uint32>()(k.height) ^ (std::hash<uint32>()(k.width) << 1)) >> 1);
        }
    };
}

RenderGraph::RenderGraph(IGraphicsHardwareInterface& ghi)
    : m_ghi(ghi)
{

}

void RenderGraph::AddPass(std::function<void(RenderGraphPassBuilder& rgb)> setup, std::function<void(GHICommandList&)> execution)
{
    RenderPassDesc r;
    r.setupFunction = setup;
    r.executionFunction = execution;

    m_renderPassDescs.emplace_back(r);
}

RenderResourcePtr const RenderGraph::CreateRenderTarget(RenderTargetDesc const& desc)
{
    m_renderTargetDescs.emplace_back(desc);
    return RenderResourcePtr(m_renderTargetDescs.size() - 1);
}

void RenderGraph::Prepare()
{
    static std::unordered_map<RenderTargetDesc, std::queue<GHIRenderTarget*>> s_ghiRenderTargetCache;
    static std::unordered_map<std::vector<GHIRenderTarget*>, GHIRenderPass*> s_ghiRenderPassCache;

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
            m_ghiRenderTargets.emplace_back(
                m_ghi.CreateRenderTarget(desc.width, desc.height)
            );
        }
    }

    // Create actual render passes (cached)

    for (uint i = 0; i < passBuilders.size(); ++i)
    {
        std::vector<GHIRenderTarget*> outputRenderTargets;
        outputRenderTargets.reserve(passBuilders[i].output.size());

        for (RenderResourcePtr const& output : passBuilders[i].output)
        {
            outputRenderTargets.emplace_back(m_ghiRenderTargets[output.m_virtualId]);
        }

        GHIRenderPass* ghiRenderPass = nullptr;

        auto it = s_ghiRenderPassCache.find(outputRenderTargets);
        if (it != s_ghiRenderPassCache.end())
        {
            ghiRenderPass = it->second;
        }
        else
        {
            ghiRenderPass = m_ghi.CreateRenderPass(outputRenderTargets);
            s_ghiRenderPassCache.insert_or_assign(outputRenderTargets, ghiRenderPass);
        }

        RenderGraphPass graphPass;
        graphPass.ghiRenderPass = ghiRenderPass;
        graphPass.renderPassDesc = &m_renderPassDescs[i];
        m_compiledRenderPasses.emplace_back(graphPass);
    }

    // Destroy unused render targets

    // OMG WHAT A MONSTER
    for (auto itRenderTargetCache : s_ghiRenderTargetCache)
    {
        while (!itRenderTargetCache.second.empty())
        {
            GHIRenderTarget* const renderTarget = itRenderTargetCache.second.front();
            itRenderTargetCache.second.pop();
            
            m_ghi.DestroyRenderTarget(renderTarget);

            // Destroy all render passes using this render target
            for (auto itRenderPassCache = s_ghiRenderPassCache.begin(); itRenderPassCache != s_ghiRenderPassCache.end();)
            {
                bool erased = false;
                for (GHIRenderTarget* const passRenderTarget : itRenderPassCache->first)
                {
                    if (passRenderTarget == renderTarget)
                    {
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
    static std::vector<GHICommandList*> commandLists;
    for (uint i = commandLists.size(); i < m_compiledRenderPasses.size(); ++i)
    {
        commandLists.emplace_back(m_ghi.AllocateCommandBuffer());
    }
    for (uint i = commandLists.size(); i > m_compiledRenderPasses.size(); --i)
    {
        m_ghi.FreeCommandBuffer(commandLists[i - 1]);
        commandLists.pop_back();
    }

    uint commandIndex = 0;
    for (auto& pass : m_compiledRenderPasses)
    {
        GHICommandList* commandList = commandLists[commandIndex++];
        commandList->Begin();

        m_ghi.BeginRenderPass(pass.ghiRenderPass, commandList);

        pass.renderPassDesc->executionFunction(*commandList);

        m_ghi.EndRenderPass(pass.ghiRenderPass, commandList);

        commandList->End();
    }

    m_ghi.Submit(commandLists);
}

GHIRenderTarget const* RenderGraph::GetRenderTarget(RenderResourcePtr const& ptr)
{
    return m_ghiRenderTargets[ptr.m_virtualId];
}
