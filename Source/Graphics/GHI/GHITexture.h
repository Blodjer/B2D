#pragma once
#include "IGraphicsResource.h"

enum class EGHITextureFormat
{
    RGBA8,
    BGRA8,
    Depth24,
    Depth24Stencil8,
};

namespace
{
    uint GetBytesPerPixel(EGHITextureFormat format)
    {
        switch (format)
        {
        case EGHITextureFormat::Depth24:
            return 3;
        case EGHITextureFormat::RGBA8:
        case EGHITextureFormat::BGRA8:
        case EGHITextureFormat::Depth24Stencil8:
            return 4;
        default:
            B2D_TRAP();
            return 1;
        }
    }
}

enum EGHITextureUsageFlags
{
    ColorAttachment = 1 << 1,
    DepthStencilAttachment = 1 << 2,
};

class GHITexture : public IGraphicsResource
{
public:
    GHITexture(uint32 width, uint32 height)
        : m_width(width)
        , m_height(height)
    {}

    virtual void* GetNativePtr() const { return nullptr; }; // TODO: Used for ImGui

    virtual uint32 GetWidth() const { return m_width; }
    virtual uint32 GetHeight() const { return m_height; }

protected:
    uint32 m_width;
    uint32 m_height;

};
