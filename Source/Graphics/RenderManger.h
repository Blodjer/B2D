#pragma once

#include "Graphics/Renderer/IRenderer.h"
#include "RenderThread.h"

class GenericWindow;
class GHIRenderTarget;
class Thread;

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

        if (mMultithreaded)
        {
            mRenderThreadRunnable->AddRenderer(renderer);
        }
        else
        {
            mRenderers.emplace_back(renderer);
            renderer->Init();
        }

        return renderer;
    }

    template<class T>
    void DeleteRenderer(T*& renderer)
    {
        if (mMultithreaded)
        {
            mRenderThreadRunnable->RemoveRenderer(renderer);
        }
        else
        {
            renderer->Shutdown();
            
            auto it = std::find(mRenderers.begin(), mRenderers.end(), renderer);
            if (it != mRenderers.end())
            {
                std::iter_swap(it, mRenderers.end() - 1);
                mRenderers.pop_back();
            }

            delete renderer;
        }

        renderer = nullptr;
    }

    void Tick(float deltaTime);

    void Draw();

private:
    std::vector<IRenderer*> mRenderers;

    bool mMultithreaded = false;
    Thread* mRenderThread = nullptr;
    RenderThread* mRenderThreadRunnable = nullptr;
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
