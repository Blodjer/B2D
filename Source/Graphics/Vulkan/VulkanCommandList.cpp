#include "B2D_pch.h"
#include "VulkanCommandList.h"

#include "VulkanGraphicsPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanResourceSet.h"

#include "GameEngine.h" // TMP
#include "VulkanGHI.h" // TMP
#include "VulkanDevice.h" // TMP

void VulkanCommandList::Begin()
{
    vk::CommandBufferBeginInfo commandBufferBeginInfo;
    commandBufferBeginInfo.flags = vk::CommandBufferUsageFlags(0);
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

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
