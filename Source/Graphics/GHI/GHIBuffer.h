#pragma once

enum class EGHIBufferType
{
    VertexBuffer,
    IndexBuffer,
    UniformBuffer,
};

class GHIBuffer
{
public:
    virtual void Upload(void const* data, uint size) = 0;
};