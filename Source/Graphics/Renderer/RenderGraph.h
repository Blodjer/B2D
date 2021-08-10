#pragma once

class IGraphicsHardwareInterface;
class GHIRenderPass;
class GHICommandList;
class GHIRenderTarget;

class RenderResourcePtr
{
    friend class RenderGraph;

private:
    using TPtrType = uint;
    RenderResourcePtr(TPtrType const virtualId) : m_virtualId(virtualId) {}

public:
    bool operator==(RenderResourcePtr const& other)
    {
        return other.m_virtualId == m_virtualId;
    }

private:
    TPtrType m_virtualId;
};

class RenderGraphPassBuilder
{
    friend class RenderGraph;

private:
    std::vector<RenderResourcePtr> output;
    std::vector<RenderResourcePtr> input;

public:
    void AddOutput(RenderResourcePtr const& ptr) { output.emplace_back(ptr); }
    void AddInput(RenderResourcePtr const& ptr) { input.emplace_back(ptr); }
};

struct RenderTargetDesc
{
    uint32 width;
    uint32 height;
    // format

    bool operator==(RenderTargetDesc const& other) const
    {
        return width == other.width
            && height == other.height;
    }
};

struct RenderPassDesc
{
    std::function<void(RenderGraphPassBuilder&)> setupFunction = nullptr;
    std::function<void(GHICommandList&)> executionFunction = nullptr;
};

struct RenderGraphPass
{
    GHIRenderPass* ghiRenderPass;
    RenderPassDesc const* renderPassDesc;
};

class RenderGraph
{
    friend class RenderManager;

public:
    RenderGraph() = delete;
    RenderGraph(RenderGraph const&) = delete;
    RenderGraph(IGraphicsHardwareInterface& ghi);
    ~RenderGraph() {}

public:
    // FullscreenPass?

    void AddPass(std::function<void(RenderGraphPassBuilder& rgb)> setup, std::function<void(GHICommandList&)> execution);

    RenderResourcePtr const CreateRenderTarget(RenderTargetDesc const& desc);

private:
    void Prepare();
    void Execute();

    GHIRenderTarget const* GetRenderTarget(RenderResourcePtr const& ptr); // TMP

private:
    IGraphicsHardwareInterface& m_ghi;

    std::vector<RenderPassDesc> m_renderPassDescs;
    std::vector<RenderTargetDesc> m_renderTargetDescs;

    std::vector<GHIRenderTarget*> m_ghiRenderTargets;

    std::vector<RenderGraphPass> m_compiledRenderPasses;
};
