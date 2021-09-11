#pragma once

class GHIBuffer;
class GHITexture;
class GHISampler;

class GHIResourceSet
{
public:
    virtual void Bind(uint32 binding, GHIBuffer const* buffer) = 0;
    virtual void Bind(uint32 binding, GHITexture const* texture, GHISampler const* sampler) = 0;

};
