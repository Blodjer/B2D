#pragma once

class GHIBuffer;
class GHIGraphicsPipeline;
class GHIResourceSet;

class GHICommandList
{
public:
    virtual void Begin() {};
    virtual void End() {};

    virtual void BindGraphicsPipeline(GHIGraphicsPipeline const* pipeline) = 0;

    virtual void BindVertexBuffer(GHIBuffer const* buffer) = 0;
    virtual void BindIndexBuffer(GHIBuffer const* buffer) = 0;

    virtual void BindResourceSet(uint32 slot, GHIResourceSet* set) = 0;

    virtual void SetShaderParameter(uint32 bytes, void const* ptr) = 0;

    virtual void Draw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) = 0;
    virtual void DrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, uint32 vertexOffset, uint32 firstInstance) = 0;
};
