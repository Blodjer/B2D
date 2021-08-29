#include "B2D_pch.h"
#include "VulkanCommandList.h"

#include "VulkanBuffer.h"
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

void VulkanCommandList::BindVertexBuffer(GHIBuffer const* buffer)
{
    VulkanBuffer const* vkBuffer = static_cast<VulkanBuffer const*>(buffer);
    vk::DeviceSize offset = 0;

    m_commandBuffer.bindVertexBuffers(0, vkBuffer->buffer, offset);
}

void VulkanCommandList::BindUniformBuffer(uint32 bindingSlot, GHIBuffer const* buffer)
{
    VulkanBuffer const* vkBuffer = static_cast<VulkanBuffer const*>(buffer);

    vk::DescriptorBufferInfo binfo;
    binfo.buffer = vkBuffer->buffer;
    binfo.offset = 0;
    binfo.range = VK_WHOLE_SIZE; // TODO?

    VulkanGHI* vghi = static_cast<VulkanGHI*>(GameEngine::Instance()->GetGHI());

    static uint i = 0;

    vk::DescriptorSet set;

    if (i == 0)
    {
        set = vghi->m_descriptorSets[2];
    }
    else if (i == 1)
    {
        set = vghi->m_descriptorSets[0];
    }
    else if (i == 2)
    {
        set = vghi->m_descriptorSet2;
    }

    vk::WriteDescriptorSet setWrite;
    setWrite.dstBinding = bindingSlot;
    setWrite.dstSet = set;
    setWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
    setWrite.setBufferInfo(binfo);

    static bool b = false;
    if (i != 0 || !b)
    {
        vghi->m_device->GetLogical().updateDescriptorSets(setWrite, nullptr);
        b = true;
    }

    if (i == 2)
    {
        m_commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, vghi->m_pipelineLayout, 0, vghi->m_descriptorSet2, nullptr);
    }
    else
    {
        m_commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, vghi->m_pipelineLayout, 0, vghi->m_descriptorSets, nullptr);
    }

    ++i;
    if (i > 2)
    {
        i = 0;
    }
}

void VulkanCommandList::SetShaderParameter(uint32 bytes, void const* ptr)
{
    m_commandBuffer.pushConstants(static_cast<VulkanGHI*>(GameEngine::Instance()->GetGHI())->m_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, bytes, ptr);
}

void VulkanCommandList::Draw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance)
{
    m_commandBuffer.draw(vertexCount, instanceCount, firstVertex, firstInstance);
}
