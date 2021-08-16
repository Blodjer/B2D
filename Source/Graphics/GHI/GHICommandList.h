#pragma once

class GHIBuffer;

class GHICommandList
{
public:
    virtual void Begin() {};
    virtual void End() {};

    virtual void BindVertexBuffer(GHIBuffer const* buffer) = 0;

    virtual void SetShaderParameter(uint32 bytes, void const* ptr) = 0;

    virtual void Draw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) = 0;
};
