#pragma once

class GHITexture
{
protected:
    GHITexture() = default;

public:
    virtual ~GHITexture() = default;

private:
    GHITexture(GHITexture const&) = delete;
    GHITexture& operator=(GHITexture const&) = delete;

public:
    virtual void Create(void* data, uint32 width, uint32 height) = 0;
    virtual void Free() = 0;
};