#include "B2D_pch.h"
#include "VulkanDevice.h"
#include <set>

VulkanDevice::VulkanDevice(vk::Instance instance, vk::PhysicalDevice physicalDevice, std::vector<char const*>& extensionsToEnable)
    : m_physicalDevice(physicalDevice)
{
    uint32 graphicsQueueFamilyIndex;
    B2D_ASSERT(FindQueueIndex(m_physicalDevice, vk::QueueFlagBits::eGraphics, graphicsQueueFamilyIndex));

    // TODO: Compute queue should probably be optional
    uint32 computeQueueFamilyIndex;
    B2D_ASSERT(FindQueueIndex(m_physicalDevice, vk::QueueFlagBits::eCompute, computeQueueFamilyIndex));

    std::set<uint32> uniqueQueueFamilies = {
        graphicsQueueFamilyIndex,
        computeQueueFamilyIndex
    };

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    float const queuePriority = 1.0f;
    for (uint32 const familyIndex : uniqueQueueFamilies)
    {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.queueFamilyIndex = familyIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.emplace_back(queueCreateInfo);
    }

    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.setPEnabledExtensionNames(extensionsToEnable);
    deviceCreateInfo.setQueueCreateInfos(queueCreateInfos);

    m_logicalDevice = m_physicalDevice.createDevice(deviceCreateInfo);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_logicalDevice);

    vk::Queue graphicsQueueHandle = m_logicalDevice.getQueue(graphicsQueueFamilyIndex, 0);
    m_graphicsQueue = VulkanQueue(graphicsQueueFamilyIndex, graphicsQueueHandle);

    vk::Queue computeQueueHandle = m_logicalDevice.getQueue(computeQueueFamilyIndex, 0);
    m_computeQueue = VulkanQueue(computeQueueFamilyIndex, computeQueueHandle);

    // TODO: For now we just expect the graphics queue to support presentation
    vk::Queue presentQueueHandle = m_logicalDevice.getQueue(graphicsQueueFamilyIndex, 0);
    m_presentQueue = VulkanQueue(graphicsQueueFamilyIndex, presentQueueHandle);
}

VulkanDevice::~VulkanDevice()
{
}

bool VulkanDevice::FindQueueIndex(vk::PhysicalDevice const& physicalDevice, vk::QueueFlagBits flags, uint32& outIndex)
{
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    B2D_ASSERT(!queueFamilyProperties.empty());

    for (size_t i = 0; i < queueFamilyProperties.size(); ++i)
    {
        if (queueFamilyProperties[i].queueFlags & flags)
        {
            outIndex = static_cast<uint32>(i);
            return true;
        }
    }

    return false;
}