#pragma once

#include "Core/Types/Matrix.h"
#include "Graphics/Material.h"

class RenderObject
{
public:
    RenderObject(TMatrix const& matrix, Material* material)
        : mMatrix(matrix)
        , mMaterial(material)
    {
    }

    TMatrix mMatrix;
    Material* mMaterial;
};

class RenderObjectBuffer
{
public:
    RenderObjectBuffer(uint32 capacity)
    {
        mBuffer = static_cast<RenderObject*>(std::malloc(sizeof(RenderObject) * capacity));
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
    FORCEINLINE RenderObject& operator[](uint32 const i) const
    {
        B2D_ASSERT(i < mCapacity);
        return mBuffer[i];
    }

    template<typename... V>
    FORCEINLINE void Add(V&... values)
    {
        B2D_ASSERT((mSize + 1) < mCapacity);

        mBuffer[mSize] = RenderObject(values...);
        mSize++;
    }

    FORCEINLINE void Clear() { mSize = 0; }
    FORCEINLINE uint32 const Size() const { return mSize; }

private:
    RenderObject* mBuffer = nullptr;
    uint32 mCapacity = 0;
    uint32 mSize = 0;
};
