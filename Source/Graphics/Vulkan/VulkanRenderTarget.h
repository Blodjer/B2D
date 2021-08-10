#pragma once
#include "Graphics/GHI/GHIRenderTarget.h"

#include "VulkanCommon.h"

class VulkanRenderTarget : public GHIRenderTarget
{
public:
    vk::Image m_image;
    vk::ImageView m_imageView;
    vk::DeviceMemory m_imageMemory;

    uint32 m_width;
    uint32 m_height;
};

