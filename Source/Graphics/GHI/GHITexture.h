#pragma once
#include "IGraphicsResource.h"

class GHITexture : public IGraphicsResource
{
public:
    virtual void* GetNativePtr() = 0;
};