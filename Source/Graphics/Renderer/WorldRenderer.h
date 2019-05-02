#pragma once

#include "Graphics/RendererInterface.h"

#include <atomic>

class CViewport;
class WorldRenderDataInterface;

class WorldRenderer final : public IRenderer
{
public:
    WorldRenderer();
    virtual ~WorldRenderer() = default;

    virtual bool ShouldRenderNextFrame() override;
    virtual void RenderInternal() override;

private:
    WorldRenderDataInterface* mWRDI = nullptr;
    CViewport const* mViewport = nullptr;

    std::atomic<uint32> mRenderedFrame;
};


