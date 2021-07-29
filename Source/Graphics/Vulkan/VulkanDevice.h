#pragma once

#include "VulkanCommon.h"

class VulkanQueue
{
public:
    VulkanQueue() {}

    VulkanQueue(uint32 queueFamilyIndex, vk::Queue queueHandle)
        : m_queueFamilyIndex(queueFamilyIndex)
        , m_queueHandle(queueHandle)
    {
    }

public:
    bool IsValid() const { return m_queueHandle; }
    uint32 GetFamilyIndex() const { return m_queueFamilyIndex; }
    vk::Queue GetHandle() const { return m_queueHandle; }

private:
    bool available = false;

    uint32 m_queueFamilyIndex = -1;
    vk::Queue m_queueHandle;
};

class VulkanDevice
{
    friend class VulkanGHI;

private:
    VulkanDevice(vk::Instance instance, vk::PhysicalDevice physicalDevice, std::vector<char const*>& extensionsToEnable);
    ~VulkanDevice();

    bool FindQueueIndex(vk::PhysicalDevice const& physicalDevice, vk::QueueFlagBits flags, uint32& outIndex);

public:
    vk::PhysicalDevice const& GetPhysical() const { return m_physicalDevice; }
    vk::Device const& GetLogical() const { return m_logicalDevice; }

    VulkanQueue const& GetGraphicsQueue() const { return m_graphicsQueue; };
    VulkanQueue const& GetComputeQueue() const { return m_computeQueue; };
    VulkanQueue const& GetPresentQueue() const { return m_presentQueue; };

private:
    vk::PhysicalDevice const m_physicalDevice;
    vk::Device m_logicalDevice;

    VulkanQueue m_graphicsQueue;
    VulkanQueue m_computeQueue;
    VulkanQueue m_presentQueue;
};
