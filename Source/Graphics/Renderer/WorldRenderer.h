#pragma once

#include "Graphics/Renderer/IRenderer.h"

#include <atomic>

class CViewport;
class WorldRenderDataInterface;

class WorldRenderer final : public IRenderer
{
public:
    virtual void Init() override;
    virtual void Shutdown() override;

protected:
    virtual bool ShouldRenderNextFrame() override;
    virtual void RenderInternal(GHIRenderTarget* const renderTarget) override;

private:
    WorldRenderDataInterface const* m_wrdi = nullptr;

    std::atomic<uint32> m_renderedFrame = 0;
};


