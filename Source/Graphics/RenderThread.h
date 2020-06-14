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
    std::vector<IRenderer*> m_renderers;

    std::mutex m_mutex;
    std::vector<IRenderer*> m_renderersToAdd;
    std::vector<IRenderer*> m_renderersToRemove;

    GenericWindow* const m_offscreenRenderContext;

};

