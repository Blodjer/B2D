#include "B2D_pch.h"
#include "VulkanCommandList.h"

#include "VulkanGraphicsPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanResourceSet.h"

void VulkanCommandList::Begin()
{
    Begin(vk::CommandBufferUsageFlags(0));
}

void VulkanCommandList::Begin(vk::CommandBufferUsageFlags flags)
{
    vk::CommandBufferBeginInfo commandBufferBeginInfo;
    commandBufferBeginInfo.flags = flags;

    m_commandBuffer.begin(commandBufferBeginInfo);
}

void VulkanCommandList::End()
{
    m_commandBuffer.end();
}

void VulkanCommandList::BindGraphicsPipeline(GHIGraphicsPipeline const* pipeline)
{
    VulkanGraphicsPipeline const* vkPipeline = static_cast<VulkanGraphicsPipeline const*>(pipeline);
    
    m_commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, vkPipeline->m_pipeline);
    m_currentPipeline = vkPipeline;
}

void VulkanCommandList::BindVertexBuffer(GHIBuffer const* buffer)
{
    VulkanBuffer const* vkBuffer = static_cast<VulkanBuffer const*>(buffer);
    vk::DeviceSize offset = 0;

    m_commandBuffer.bindVertexBuffers(0, vkBuffer->buffer, offset);
}

void VulkanCommandList::BindIndexBuffer(GHIBuffer const* buffer)
{
    VulkanBuffer const* vkBuffer = static_cast<VulkanBuffer const*>(buffer);
    vk::DeviceSize offset = 0;

    m_commandBuffer.bindIndexBuffer(vkBuffer->buffer, offset, vk::IndexType::eUint32);
}

void VulkanCommandList::BindResourceSet(uint32 slot, GHIResourceSet* set)
{
    VulkanResourceSet* vkResourceSet = static_cast<VulkanResourceSet*>(set);
    m_commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_currentPipeline->m_pipelineLayout, slot, vkResourceSet->GetDescriptorSet(), nullptr);
}

void VulkanCommandList::SetShaderParameter(uint32 bytes, void const* ptr)
{
    m_commandBuffer.pushConstants(m_currentPipeline->m_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, bytes, ptr);
}

void VulkanCommandList::Draw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance)
{
    m_commandBuffer.draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanCommandList::DrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, uint32 vertexOffset, uint32 firstInstance)
{
    m_commandBuffer.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}
