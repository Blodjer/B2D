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
    virtual void Bind(uint32 binding, GHITexture const* texture, GHISampler const* sampler) override;

    void Build();

    vk::DescriptorSet GetDescriptorSet();

    VulkanDevice* m_device = nullptr;

private:
    struct Binding
    {
        uint32 binding;

        enum class Type
        {
            Buffer,
            Sampler
        } type = Type::Buffer;

        union
        {
            GHIBuffer const* buffer;

            struct CombinedSampler
            {
                GHITexture const* texture;
                GHISampler const* sampler;
            } combinedSampler;
        };
    };

    bool m_descriptorSetBindingsDirty = true;
    std::vector<Binding> m_bindings;

    bool m_descriptorSetBuild = false;
    vk::DescriptorSet m_descriptorSet;
    vk::DescriptorSetLayout m_descriptorSetLayout;

    static INLINE vk::DescriptorPool m_descriptorPool;
};

