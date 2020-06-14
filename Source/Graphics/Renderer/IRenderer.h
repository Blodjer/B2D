#pragma once

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

    GHITexture* GetRenderOutput();
    float GetRenderTime() const { return m_renderTime; }

    std::mutex m_mutex;

protected:
    virtual bool ShouldRenderNextFrame() { return true; }

    void PreRender();
    virtual void RenderInternal(GHIRenderTarget* const renderTarget) = 0;
    void PostRender();

private:
    std::atomic<bool> m_renderToSwtich; // true = 1, false = 2
    std::atomic<float> m_renderTime;

    GHIRenderTarget* m_renderTarget1 = nullptr;
    GHIRenderTarget* m_renderTarget2 = nullptr;
    GHITexture* m_renderTexture1 = nullptr;
    GHITexture* m_renderTexture2 = nullptr;
};
