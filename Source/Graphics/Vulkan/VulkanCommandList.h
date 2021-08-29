#pragma once
#include "Graphics/GHI/GHICommandList.h"

#include "VulkanCommon.h"

class VulkanCommandList : public GHICommandList
{
public:
    vk::CommandBuffer m_commandBuffer;

public:
    virtual void Begin() override;
    virtual void End() override;

    virtual void BindVertexBuffer(GHIBuffer const* buffer) override;

    virtual void BindUniformBuffer(uint32 bindingSlot, GHIBuffer const* buffer) override;
    virtual void SetShaderParameter(uint32 bytes, void const* ptr) override;

    virtual void Draw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) override;
};
