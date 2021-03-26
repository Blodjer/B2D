#pragma once
#include "IGraphicsResource.h"

class GHITexture;

class GHIRenderTarget : public IGraphicsResource
{
public:
    virtual GHITexture* GetTexture() const = 0;

    // TODO: add TVec2i GetSize()
    virtual uint32 GetWidth() const;
    virtual uint32 GetHeight() const;
};
