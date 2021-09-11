#pragma once
#include "Graphics/GHI/GHISampler.h"

#include "VulkanCommon.h"

class VulkanSampler : public GHISampler
{
public:
    vk::Sampler sampler;
};

