#pragma once

#include "Core/Types/Matrix.h"
#include "Graphics/Material.h"

class RenderObject
{
public:
    RenderObject(TMatrix const& matrix, CMaterial const* material)
        : mMatrix(matrix)
        , mMaterial(material)
    {
    }

    TMatrix mMatrix;
    CMaterial const* mMaterial;
};

class RenderObjectBuffer
{
public:
    RenderObjectBuffer(uint32 bufferSize)
    {
        mBuffer = static_cast<RenderObject*>(std::malloc(sizeof(RenderObject) * bufferSize));
        B2D_ASSERT(mBuffer != nullptr);

        mBufferSize = bufferSize;
    }

    ~RenderObjectBuffer()
    {
        free(mBuffer);
    }

    RenderObjectBuffer(RenderObjectBuffer const&) = delete;
    void operator=(RenderObjectBuffer const&) = delete;

    FORCEINLINE RenderObject& operator[](uint32 const i) const
    {
        B2D_ASSERT(i < mBufferSize);
        return mBuffer[i];
    }

    //FORCEINLINE RenderObject* Buffer() const { return mBuffer; }
    FORCEINLINE uint32& Size() { return mSize; }
    FORCEINLINE uint32 const Size() const { return mSize; }

private:
    RenderObject* mBuffer = nullptr;
    uint32 mBufferSize = 0;
    uint32 mSize = 0;
};