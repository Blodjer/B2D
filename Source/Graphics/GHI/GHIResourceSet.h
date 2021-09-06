#pragma once

class GHIBuffer;

class GHIResourceSet
{
public:
    virtual void Bind(uint32 binding, GHIBuffer const* buffer) = 0;
};
