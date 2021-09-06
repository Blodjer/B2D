#pragma once

#include "Graphics/GHI/GHIGraphicsPipeline.h"

class VulkanGraphicsPipeline : public GHIGraphicsPipeline
{
public:
    vk::PipelineLayout m_pipelineLayout;
    vk::Pipeline m_pipeline;
};