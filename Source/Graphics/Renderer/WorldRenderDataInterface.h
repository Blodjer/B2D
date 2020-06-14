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

    uint32 GetPreparedFrame() const { return m_preparedFrame; }

    RenderObjectBuffer<QuadRenderObject> const& GetQuadBuffer() const { return m_quadBuffer; }

public:
    FORCEINLINE void StartWrite() { m_mutex.lock(); }
    FORCEINLINE void StopWrite() { m_mutex.unlock(); }
    FORCEINLINE bool TryWrite() { return m_mutex.try_lock(); }
    FORCEINLINE void StartRead() { m_mutex.lock_shared(); }
    FORCEINLINE void StopRead() { m_mutex.unlock_shared(); }

    // TODO: Add ref count and only write if ref count > 0

private:
    RenderObjectBuffer<QuadRenderObject> m_quadBuffer;

    std::shared_mutex m_mutex;
    std::atomic<uint32> m_preparedFrame;

};

template<typename F>
bool WorldRenderDataInterface::ClearAndSetRenderData(F quadBufferFunction)
{
    if (!TryWrite())
    {
        return false;
    }

    m_quadBuffer.Clear();
    quadBufferFunction(m_quadBuffer);

    m_preparedFrame++;

    StopWrite();

    return true;
}
