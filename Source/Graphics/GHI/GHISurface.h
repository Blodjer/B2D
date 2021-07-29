#pragma once

class GHISurface
{
public:
    virtual void Resize(uint32 width, uint32 height) = 0;

    virtual void Present() = 0;
};
