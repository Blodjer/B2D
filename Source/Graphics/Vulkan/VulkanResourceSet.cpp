#include "B2D_pch.h"
#include "VulkanResourceSet.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanSampler.h"

VulkanResourceSet::~VulkanResourceSet()
{
    if (m_descriptorSetBuild)
    {
        m_device->GetLogical().freeDescriptorSets(m_descriptorPool, m_descriptorSet);
        m_device->GetLogical().destroyDescriptorSetLayout(m_descriptorSetLayout);
    }
}

void VulkanResourceSet::Bind(uint32 binding, GHIBuffer const* buffer)
{
    m_descriptorSetBindingsDirty = true;
    
    for (Binding& b : m_bindings)
    {
        if (b.binding == binding)
        {
            b.buffer = buffer;
            b.type = Binding::Type::Buffer;
            return;
        }
    }

    Binding b;
    b.binding = binding;

    b.buffer = buffer;
    b.type = Binding::Type::Buffer;

    m_bindings.emplace_back(b);
}

void VulkanResourceSet::Bind(uint32 binding, GHITexture const* texture, GHISampler const* sampler)
{
    m_descriptorSetBindingsDirty = true;

    for (Binding& b : m_bindings)
    {
        if (b.binding == binding)
        {
            b.combinedSampler.texture = texture;
            b.combinedSampler.sampler = sampler;
            b.type = Binding::Type::Sampler;
            return;
        }
    }

    Binding b;
    b.binding = binding;

    b.combinedSampler.texture = texture;
    b.combinedSampler.sampler = sampler;
    b.type = Binding::Type::Sampler;

    m_bindings.emplace_back(b);
}

void VulkanResourceSet::Build()
{
    if (!m_descriptorPool)
    {
        std::array<vk::DescriptorPoolSize, 2> descriptorPoolSizes =
        { {
            { vk::DescriptorType::eUniformBuffer, 10 },
            { vk::DescriptorType::eSampler, 10 }
        } };

        vk::DescriptorPoolCreateInfo descriptorPoolInfo;
        descriptorPoolInfo.flags = vk::DescriptorPoolCreateFlags(0);
        descriptorPoolInfo.maxSets = 10;
        descriptorPoolInfo.setPoolSizes(descriptorPoolSizes);

        m_descriptorPool = m_device->GetLogical().createDescriptorPool(descriptorPoolInfo);
    }

    //

    if (!m_descriptorSetBuild)
    {
        std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings;
        for (Binding const& b : m_bindings)
        {
            vk::DescriptorSetLayoutBinding setLayoutBinding;
            setLayoutBinding.binding = b.binding;
            setLayoutBinding.descriptorCount = 1;

            vk::DescriptorType desciptorType;
            switch (b.type)
            {
                case Binding::Type::Buffer:
                    desciptorType = vk::DescriptorType::eUniformBuffer; break;
                case Binding::Type::Sampler:
                    desciptorType = vk::DescriptorType::eCombinedImageSampler; break;
                default:
                    B2D_BREAK();
                    break;
            }

            setLayoutBinding.descriptorType = desciptorType;
            setLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eAll; // TMP
            setLayoutBindings.emplace_back(setLayoutBinding);
        }

        vk::DescriptorSetLayoutCreateInfo setLayoutInfo;
        setLayoutInfo.setBindings(setLayoutBindings);
        setLayoutInfo.flags = vk::DescriptorSetLayoutCreateFlags(0);

        m_descriptorSetLayout = m_device->GetLogical().createDescriptorSetLayout(setLayoutInfo);

        vk::DescriptorSetAllocateInfo allocInfo;
        allocInfo.descriptorPool = m_descriptorPool;
        allocInfo.setSetLayouts(m_descriptorSetLayout);

        m_descriptorSet = m_device->GetLogical().allocateDescriptorSets(allocInfo)[0]; // TMP
        m_descriptorSetBuild = true;
    }

    //

    if (m_descriptorSetBindingsDirty)
    {
        std::vector<vk::DescriptorBufferInfo> bufferInfos;
        std::vector<vk::DescriptorImageInfo> imageInfos;
        for (Binding const& b : m_bindings)
        {
            if (b.type == Binding::Type::Buffer)
            {
                VulkanBuffer const* vkBuffer = static_cast<VulkanBuffer const*>(b.buffer);

                vk::DescriptorBufferInfo binfo;
                binfo.buffer = vkBuffer->buffer;
                binfo.offset = 0;
                binfo.range = VK_WHOLE_SIZE; // TODO?

                bufferInfos.emplace_back(binfo);
            }
            else if (b.type == Binding::Type::Sampler)
            {
                VulkanTexture const* vkTexture = static_cast<VulkanTexture const*>(b.combinedSampler.texture);
                VulkanSampler const* vkSampler = static_cast<VulkanSampler const*>(b.combinedSampler.sampler);

                vk::DescriptorImageInfo imageBufferInfo;
                imageBufferInfo.sampler = vkSampler->sampler;
                imageBufferInfo.imageView = vkTexture->m_imageView;
                imageBufferInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

                imageInfos.emplace_back(imageBufferInfo);
            }
            else
            {
                B2D_BREAK();
            }
        }

        std::vector<vk::WriteDescriptorSet> writes;
        if (bufferInfos.size() > 0)
        {
            vk::WriteDescriptorSet bufferWrite;
            bufferWrite.dstBinding = 0;
            bufferWrite.dstSet = m_descriptorSet;
            bufferWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
            bufferWrite.setBufferInfo(bufferInfos);

            writes.emplace_back(bufferWrite);
        }
        if (imageInfos.size() > 0)
        {
            vk::WriteDescriptorSet samplerWrite;
            samplerWrite.dstBinding = 0;
            samplerWrite.dstSet = m_descriptorSet;
            samplerWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
            samplerWrite.setImageInfo(imageInfos);

            writes.emplace_back(samplerWrite);
        }

        // TODO: Only update when changed
        m_device->GetLogical().updateDescriptorSets(writes, nullptr);
        m_descriptorSetBindingsDirty = false;
    }
}

vk::DescriptorSet VulkanResourceSet::GetDescriptorSet()
{
    if (m_descriptorSetBindingsDirty)
    {
        Build();
    }

    B2D_CHECK_f(!m_descriptorSetBuild, "Descriptor Set was not build yet");
    return m_descriptorSet;
}
