#pragma once
#include "IGraphicsResource.h"

class GHITexture;

class GHIRenderTarget : public IGraphicsResource
{
public:
    virtual GHITexture* GetTexture() const = 0;

};
