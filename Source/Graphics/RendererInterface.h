#pragma once

class GHIRenderTarget;
class GHITexture;

class IRenderer
{
public:
    IRenderer();
    virtual ~IRenderer();

    void Render();

    GHITexture* GetRenderOutput();
    std::mutex mMutex;

protected:
    virtual bool ShouldRenderNextFrame() { return true; }

    void PreRender();
    virtual void RenderInternal() = 0;
    void PostRender();

private:
    std::atomic<bool> mRenderToSwtich; // true = 1, false = 2

    GHIRenderTarget* mRenderTarget1 = nullptr;
    GHIRenderTarget* mRenderTarget2 = nullptr;
    GHITexture* mRenderTexture1 = nullptr;
    GHITexture* mRenderTexture2 = nullptr;
};
