#pragma once

#include "Graphics/RenderObject.h"

#include <atomic>

class CViewport;
class GHIRenderTarget;
class IGraphicsHardwareInterface;

class WorldRenderer final
{
public:
    WorldRenderer();
    ~WorldRenderer();

    bool PendingRendering() const { return mPreparedFrame != mRenderedFrame; }

    template<typename F>
    bool ClearAndSetRenderData(CViewport const* viewport, F fillRenderObjectBufferFunction)
    {
        if (PendingRendering())
        {
            return false;
        }

        Lock();

        mViewport = viewport;

        mRenderObjectBuffer.Clear();
        fillRenderObjectBufferFunction(mRenderObjectBuffer);

        mPreparedFrame++;

        Unlock();

        return true;
    }

    void Render();

    RenderObjectBuffer const& GetRenderObjectBuffer() { return mRenderObjectBuffer; }

private:
    FORCEINLINE void Lock() { mMutex.lock(); }
    FORCEINLINE void Unlock() { mMutex.unlock(); }

private:
    std::mutex mMutex;

    GHIRenderTarget* mRenderTarget = nullptr;
    RenderObjectBuffer mRenderObjectBuffer;

    CViewport const* mViewport;

    std::atomic<uint32> mPreparedFrame;
    std::atomic<uint32> mRenderedFrame;
};
