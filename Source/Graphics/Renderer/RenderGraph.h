#pragma once

class IGraphicsHardwareInterface;
class GHIRenderPass;
class GHICommandList;
class GHITexture;
class GHISurface;
enum class EGHITextureFormat;

class RenderResourcePtr
{
    friend class RenderGraph;

private:
    using TPtrType = uint;
    static const TPtrType INVALID = -1;

public:
    RenderResourcePtr() : m_virtualId(INVALID) {}

private:
    RenderResourcePtr(TPtrType const virtualId) : m_virtualId(virtualId) {}

public:
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
    TPtrType m_virtualId;
};

class RenderGraphPassBuilder
{
    friend class RenderGraph;

private:
    std::vector<RenderResourcePtr> output;
    std::vector<RenderResourcePtr> input;
    RenderResourcePtr depthStencil;

public:
    void AddOutput(RenderResourcePtr const& ptr) { output.emplace_back(ptr); }
    void AddDepthStencil(RenderResourcePtr const& ptr) { depthStencil = ptr; }

    void AddInput(RenderResourcePtr const& ptr) { input.emplace_back(ptr); }
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

struct RenderPassDesc
{
    std::function<void(RenderGraphPassBuilder&)> setupFunction = nullptr;
    std::function<void(GHICommandList&, GHIRenderPass const*)> executionFunction = nullptr;
};

struct RenderGraphPass
{
    GHIRenderPass* ghiRenderPass = nullptr;
    RenderPassDesc const* renderPassDesc = nullptr;
};

struct PresentDesc
{
    RenderResourcePtr output;
    GHISurface* surface = nullptr;
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

    void AddPass(std::function<void(RenderGraphPassBuilder& rgb)> setup, std::function<void(GHICommandList&, GHIRenderPass const*)> execution);
    void AddPresent(RenderResourcePtr const& output, GHISurface* surface);

    RenderResourcePtr const CreateRenderTarget(RenderTargetDesc const& desc);

private:
    void Prepare();
    void Execute();

    GHITexture const* GetRenderTarget(RenderResourcePtr const& ptr);

private:
    IGraphicsHardwareInterface& m_ghi;

    std::vector<RenderPassDesc> m_renderPassDescs;
    std::vector<RenderTargetDesc> m_renderTargetDescs;
    std::vector<PresentDesc> m_presentDescs;

    std::vector<GHITexture*> m_ghiRenderTargets;

    std::vector<RenderGraphPass> m_compiledRenderPasses;
};
