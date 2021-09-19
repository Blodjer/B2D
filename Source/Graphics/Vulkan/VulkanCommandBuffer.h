#pragma once
#include "Graphics/GHI/GHICommandBuffer.h"

#include "VulkanCommon.h"

class VulkanGraphicsPipeline;

class VulkanCommandBuffer : public GHICommandBuffer
{
    friend class VulkanGHI;

private:
    vk::CommandBuffer m_commandBuffer;
    vk::Fence m_completionFence;
    bool m_isSubmitted;

public:
    vk::CommandBuffer Get() const { return m_commandBuffer; }
    bool IsSubmitted() const { return m_isSubmitted; }

private:
    VulkanGraphicsPipeline const* m_currentPipeline = nullptr; // TMP

public:
    virtual void Begin() override;
    void Begin(vk::CommandBufferUsageFlags flags);

    virtual void End() override;

    virtual void BindGraphicsPipeline(GHIGraphicsPipeline const* pipeline) override;

    virtual void BindVertexBuffer(GHIBuffer const* buffer) override;
    virtual void BindIndexBuffer(GHIBuffer const* buffer) override;

    virtual void BindResourceSet(uint32 slot, GHIResourceSet* set) override;
    //virtual void BindResourceSet(uint32 firstSlot, std::vector<GHIResourceSet*> sets);

    virtual void SetShaderParameter(uint32 bytes, void const* ptr) override;

    virtual void Draw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) override;
    virtual void DrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, uint32 vertexOffset, uint32 firstInstance) override;

};
