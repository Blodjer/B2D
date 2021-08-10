#include "B2D_pch.h"
#include "VulkanCommandList.h"

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

void VulkanCommandList::DrawTest()
{
    m_commandBuffer.draw(3, 1, 0, 0);
}
