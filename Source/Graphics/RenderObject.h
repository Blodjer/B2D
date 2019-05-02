#pragma once

#include "Core/Types/Matrix.h"
#include "Graphics/Material.h"

template<class C>
class RenderObjectBuffer
{
public:
    RenderObjectBuffer(uint32 capacity)
    {
        mBuffer = static_cast<C*>(std::malloc(sizeof(C) * capacity));
        B2D_ASSERT(mBuffer != nullptr);

        mCapacity = capacity;
    }

    ~RenderObjectBuffer()
    {
        free(mBuffer);
    }

    RenderObjectBuffer(RenderObjectBuffer const&) = delete;
    void operator=(RenderObjectBuffer const&) = delete;

public:
    FORCEINLINE C& operator[](uint32 const i) const
    {
        B2D_ASSERT(i < mCapacity);
        return mBuffer[i];
    }

    template<typename... V>
    FORCEINLINE void Add(V&... values)
    {
        if (B2D_CHECKf(mSize >= mCapacity, "Cannot add RenderObject. RenderObjectBuffer has reached it's maximum capacity of {} elements", mCapacity))
        {
            return;
        }

        mBuffer[mSize] = C(values...);
        mSize++;
    }

    FORCEINLINE void Clear() { mSize = 0; }
    FORCEINLINE uint32 const Size() const { return mSize; }

private:
    C* mBuffer = nullptr;
    uint32 mCapacity = 0;
    uint32 mSize = 0;
};

class QuadRenderObject
{
public:
    QuadRenderObject(TMatrix const& matrix, Material* material)
        : mMatrix(matrix)
        , mMaterial(material)
    {
    }

    TMatrix mMatrix;
    Material* mMaterial;
};
