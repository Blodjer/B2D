#pragma once

#include "Graphics/RenderObject.h"
#include "Graphics/Viewport.h"
#include <shared_mutex>

class WorldRenderDataInterface final
{
public:
    WorldRenderDataInterface();
    ~WorldRenderDataInterface();

    template<typename F>
    bool ClearAndSetRenderData(F fillRenderObjectBufferFunction);

    uint32 GetPreparedFrame() const { return mPreparedFrame; }

    RenderObjectBuffer<QuadRenderObject> const& GetQuadBuffer() const { return mQuadBuffer; }

public:
    FORCEINLINE void StartWrite() { mMutex.lock(); }
    FORCEINLINE void StopWrite() { mMutex.unlock(); }
    FORCEINLINE bool TryWrite() { return mMutex.try_lock(); }
    FORCEINLINE void StartRead() { mMutex.lock_shared(); }
    FORCEINLINE void StopRead() { mMutex.unlock_shared(); }

    // TODO: Add ref count and only write if ref count > 0

private:
    RenderObjectBuffer<QuadRenderObject> mQuadBuffer;

    std::shared_mutex mMutex;
    std::atomic<uint32> mPreparedFrame;

};

template<typename F>
bool WorldRenderDataInterface::ClearAndSetRenderData(F quadBufferFunction)
{
    if (!TryWrite())
    {
        return false;
    }

    mQuadBuffer.Clear();
    quadBufferFunction(mQuadBuffer);

    mPreparedFrame++;

    StopWrite();

    return true;
}
