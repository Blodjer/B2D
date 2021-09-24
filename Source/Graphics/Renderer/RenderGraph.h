#pragma once
#include "../Shader.h" // TMP

class IGraphicsHardwareInterface;
class GHIRenderPass;
class GHICommandBuffer;
class GHITexture;
class GHISurface;
class GHIGraphicsPipeline;
enum class EGHITextureFormat;

class RenderResourcePtr
{
    friend class RenderGraph;
    friend struct PassContext;

public:
    using TPtrType = uint;

private:
    RenderResourcePtr(TPtrType const virtualId) : m_virtualId(virtualId) {}

public:
    RenderResourcePtr() : m_virtualId(INVALID) {}

    bool IsValid() const { return m_virtualId != INVALID; }

    bool operator==(RenderResourcePtr const& other)
    {
        return other.m_virtualId == m_virtualId;
    }

    RenderResourcePtr& operator=(RenderResourcePtr const& other)
    {
        m_virtualId = other.m_virtualId;
        return *this;
    }

private:
    INLINE static const TPtrType INVALID = -1;

    TPtrType m_virtualId;
};

class RenderGraphPassBuilder
{
    friend class RenderGraph;

private:
    std::vector<RenderResourcePtr> output;
    std::vector<RenderResourcePtr> input;
    std::vector<RenderResourcePtr> graphicsPipelines;
    RenderResourcePtr depthStencil;

public:
    void AddOutput(RenderResourcePtr const& ptr) { output.emplace_back(ptr); }
    void AddDepthStencil(RenderResourcePtr const& ptr) { depthStencil = ptr; }

    void AddInput(RenderResourcePtr const& ptr) { input.emplace_back(ptr); }

    void RegisterGraphicsPipeline(RenderResourcePtr const& ptr) { graphicsPipelines.emplace_back(ptr); }
};

struct PassContext
{
    std::unordered_map<RenderResourcePtr::TPtrType, GHIGraphicsPipeline*> pipelineCache;

    GHIGraphicsPipeline* GetGraphicsPipeline(RenderResourcePtr const& ptr) const
    {
        auto it = pipelineCache.find(ptr.m_virtualId);
        if (B2D_CHECK_f(it == pipelineCache.end(), "Graphics pipeline was not registered!"))
        {
            return nullptr;
        }

        return pipelineCache.find(ptr.m_virtualId)->second;
    }
};

struct RenderTargetDesc
{
    uint32 width;
    uint32 height;
    EGHITextureFormat format;

    bool operator==(RenderTargetDesc const& other) const
    {
        return width == other.width
            && height == other.height
            && format == other.format;
    }
};

struct GraphicsPipelineDesc
{
    VertexShaderRef vs;
    PixelShaderRef ps;

    bool operator==(GraphicsPipelineDesc const& other) const
    {
        return vs == other.vs
            && ps == other.ps;
    }
};

struct RenderPassDesc
{
    std::function<void(RenderGraphPassBuilder&)> setupFunction = nullptr;
    std::function<void(GHICommandBuffer&, PassContext&)> executionFunction = nullptr;
};

struct PresentDesc
{
    RenderResourcePtr output;
    GHISurface* surface = nullptr;
};

struct RenderGraphPass
{
    GHIRenderPass* ghiRenderPass = nullptr;
    PassContext context;

    std::function<void(GHICommandBuffer&, PassContext&)>* executionFunction = nullptr;
};

class RenderGraph
{
public:
    RenderGraph() = delete;
    RenderGraph(RenderGraph const&) = delete;
    RenderGraph(IGraphicsHardwareInterface& ghi);
    ~RenderGraph() {}

public:
    void AddPass(std::function<void(RenderGraphPassBuilder&)> setup, std::function<void(GHICommandBuffer&, PassContext&)> execution); // FullscreenPass?
    void AddPresent(RenderResourcePtr const& output, GHISurface* surface);

    RenderResourcePtr const CreateRenderTarget(RenderTargetDesc const& desc);
    RenderResourcePtr const CreateGraphicsPipeline(GraphicsPipelineDesc const& desc);

    void Prepare();
    void Execute();

private:
    GHITexture const* GetRenderTarget(RenderResourcePtr const& ptr);

private:
    IGraphicsHardwareInterface& m_ghi;

    std::vector<RenderTargetDesc> m_renderTargetDescs;
    std::vector<GraphicsPipelineDesc> m_graphicsPipelineDescs;
    std::vector<RenderPassDesc> m_renderPassDescs;
    std::vector<PresentDesc> m_presentDescs;

    std::vector<GHITexture*> m_ghiRenderTargets;
    std::vector<GHIGraphicsPipeline*> m_ghiGraphicPipelines;

    std::vector<RenderGraphPass> m_compiledRenderPasses;
};
