#pragma once
#include "IGraphicsResource.h"

class GHITexture : public IGraphicsResource
{
public:
    GHITexture(uint32 width, uint32 height)
        : m_width(width)
        , m_height(height)
    {

    }

    virtual void* GetNativePtr() const = 0;

    // TODO: add TVec2i GetSize()
    virtual uint32 GetWidth() const { return m_width; }
    virtual uint32 GetHeight() const { return m_height; }

protected:
    uint32 m_width = 0;
    uint32 m_height = 0;
};
