#pragma once

#include "Graphics/RenderObject.h"

#include <atomic>

class GHIRenderTarget;
class IGraphicsHardwareInterface;

class WorldRenderer final
{
public:
    WorldRenderer();
    ~WorldRenderer();

    bool IsWaitingForNewData() const { return mPreparedFrame == mRenderedFrame; }
    bool PendingRendering() const { return mPreparedFrame != mRenderedFrame; }

    FORCEINLINE bool TryLock() { return mMutex.try_lock(); };
    FORCEINLINE void Lock() { mMutex.lock(); }
    FORCEINLINE void Unlock() { mMutex.unlock(); }

    RenderObjectBuffer& GetRenderObjectBuffer() { return mROBuffer; }

private:
    std::mutex mMutex;

    GHIRenderTarget* mRenderTarget = nullptr;
    RenderObjectBuffer mROBuffer;

    std::atomic<uint32> mPreparedFrame;
    std::atomic<uint32> mRenderedFrame;
};
