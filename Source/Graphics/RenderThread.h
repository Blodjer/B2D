#pragma once
#include "Core/Thread/Runnable.h"

class GenericWindow;
class IRenderer;

class RenderThread : public Runnable
{
public:
    RenderThread(GenericWindow* offscreenRenderContext);
    ~RenderThread();

    void AddRenderer(IRenderer* renderer);
    void RemoveRenderer(IRenderer* renderer);

protected:
    virtual bool Init() override;
    virtual void Run() override;
    virtual void Shutdown() override;

private:
    std::vector<IRenderer*> mRenderers;

    std::mutex mMutex;
    std::vector<IRenderer*> mRenderersToAdd;
    std::vector<IRenderer*> mRenderersToRemove;

    GenericWindow* const mOffscreenRenderContext;

};

