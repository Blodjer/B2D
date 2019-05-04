#pragma once

#include "Graphics/Renderer/IRenderer.h"

class GenericWindow;
class GHIRenderTarget;

class RenderManger
{
public:
    RenderManger();
    ~RenderManger();

    void Init(bool multithreaded);
    void Shutdown();

    template<class T>
    T* CreateRenderer()
    {
        B2D_STATIC_ASSERT_TYPE(IRenderer, T);

        T* renderer = new T();

        mRenderers.emplace_back(renderer);

        return renderer;
    }

    template<class T>
    void DeleteRenderer(T*& renderer)
    {
        auto it = std::find(mRenderers.begin(), mRenderers.end(), renderer);
        if (it != mRenderers.end())
        {
            std::iter_swap(it, mRenderers.end());
            mRenderers.pop_back();
        }

        delete renderer;
        renderer = nullptr;
    }

    void Render();
    void RenderLoop();
    void RenderInternal();

    void Draw();

private:
    std::vector<IRenderer*> mRenderers;

    bool mMultithreaded;
    std::thread mRenderThread;
    GenericWindow* mOffscreenRenderContext;
};

/*
class RenderThread : public Thread
{

public:
    virtual bool Init() override;

    virtual void Run() override;

    virtual void Stop() override;

};
*/
