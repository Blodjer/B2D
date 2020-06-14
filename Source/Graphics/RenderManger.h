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

        if (m_multithreaded)
        {
            m_renderThreadRunnable->AddRenderer(renderer);
        }
        else
        {
            m_renderers.emplace_back(renderer);
            renderer->Init();
        }

        return renderer;
    }

    template<class T>
    void DeleteRenderer(T*& renderer)
    {
        if (m_multithreaded)
        {
            m_renderThreadRunnable->RemoveRenderer(renderer);
        }
        else
        {
            renderer->Shutdown();
            
            auto it = std::find(m_renderers.begin(), m_renderers.end(), renderer);
            if (it != m_renderers.end())
            {
                std::iter_swap(it, m_renderers.end() - 1);
                m_renderers.pop_back();
            }

            delete renderer;
        }

        renderer = nullptr;
    }

    void Tick(float deltaTime);

    void Draw();

private:
    std::vector<IRenderer*> m_renderers;

    bool m_multithreaded = false;
    Thread* m_renderThread = nullptr;
    RenderThread* m_renderThreadRunnable = nullptr;
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
