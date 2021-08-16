#pragma once
#include "Graphics/GHI/GHITexture.h"

#include "VulkanCommon.h"

class VulkanTexture : public GHITexture
{
public:
    VulkanTexture(uint32 width, uint32 height)
        : GHITexture(width, height)
    {}

public:
    vk::Image m_image;
    vk::ImageView m_imageView;
    vk::Format m_format;

    VmaAllocation m_allocation;
};

