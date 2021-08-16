#pragma once

class GHITexture;

class GHISurface
{
public:
    virtual ~GHISurface() = default;

    virtual void Resize(uint32 width, uint32 height) = 0;
    virtual void Present(GHITexture const* renderTarget) = 0;
};
