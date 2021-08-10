#pragma once
#include "Graphics/GHI/GHICommandList.h"

#include "VulkanCommon.h"

class VulkanCommandList : public GHICommandList
{
public:
    vk::CommandBuffer m_commandBuffer;

public:
    virtual void Begin() override;
    virtual void End() override;

    virtual void DrawTest() override;
};
