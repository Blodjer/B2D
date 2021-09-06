#pragma once

#include "Graphics/GHI/GHIResourceSet.h"

#include "Graphics/Vulkan/VulkanCommon.h"

class VulkanDevice;

class VulkanResourceSet : public GHIResourceSet
{
public:
    VulkanResourceSet() = default;
    virtual ~VulkanResourceSet();

    virtual void Bind(uint32 binding, GHIBuffer const* buffer) override;

    void Build();

    vk::DescriptorSet GetDescriptorSet();

    VulkanDevice* m_device = nullptr;

private:
    struct Binding
    {
        uint32 binding;
        GHIBuffer const* buffer;
    };

    bool m_descriptorSetBindingsDirty = true;
    std::vector<Binding> m_bindings;

    bool m_descriptorSetBuild = false;
    vk::DescriptorSet m_descriptorSet;
    vk::DescriptorSetLayout m_descriptorSetLayout;

    static INLINE vk::DescriptorPool m_descriptorPool;
};

