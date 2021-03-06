#pragma once

class CViewport;
class GHIRenderTarget;
class GHITexture;

class IRenderer
{
public:
    IRenderer() = default;
    virtual ~IRenderer() = default;

    virtual void Init();
    void Render();
    virtual void Shutdown();

    GHITexture const* GetRenderOutput() const;
    float GetRenderTime() const { return m_renderTime; }

protected:
    virtual bool ShouldRenderNextFrame() { return true; }

    void PreRender();
    virtual void RenderInternal(GHIRenderTarget* const renderTarget) = 0;
    void PostRender();

public:
    CViewport* m_viewport = nullptr;

private:
    std::atomic<bool> m_isInit = false;
    std::atomic<bool> m_renderSwtich = true; // true = 1, false = 2
    std::atomic<float> m_renderTime = 0.0f;

    GHIRenderTarget* m_renderTarget1 = nullptr;
    GHIRenderTarget* m_renderTarget2 = nullptr;
};
