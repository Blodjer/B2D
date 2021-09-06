#include "B2D_pch.h"
#include "VulkanResourceSet.h"
#include "VulkanDevice.h"
#include "VulkanBuffer.h"

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
            return;
        }
    }

    Binding b;
    b.binding = binding;
    b.buffer = buffer;
    m_bindings.emplace_back(b);
}

void VulkanResourceSet::Build()
{
    if (!m_descriptorPool)
    {
        std::array<vk::DescriptorPoolSize, 1> descriptorPoolSizes =
        {
            { vk::DescriptorType::eUniformBuffer, 10 }
        };

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
            setLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
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
        for (Binding const& b : m_bindings)
        {
            VulkanBuffer const* vkBuffer = static_cast<VulkanBuffer const*>(b.buffer);

            vk::DescriptorBufferInfo binfo;
            binfo.buffer = vkBuffer->buffer;
            binfo.offset = 0;
            binfo.range = VK_WHOLE_SIZE; // TODO?
            bufferInfos.emplace_back(binfo);
        }

        vk::WriteDescriptorSet setWrite;
        setWrite.dstBinding = 0;
        setWrite.dstSet = m_descriptorSet;
        setWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
        setWrite.setBufferInfo(bufferInfos);

        // TODO: Only update when changed
        m_device->GetLogical().updateDescriptorSets(setWrite, nullptr);
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
