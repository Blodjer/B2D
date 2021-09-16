#pragma once
#include "Graphics/GHI/GHIRenderPass.h"

#include "VulkanCommon.h"

class VulkanRenderPass : public GHIRenderPass
{
public:
    vk::RenderPass m_renderPass;
    vk::Framebuffer m_frameBuffer;
    vk::Extent2D m_extent;
};
