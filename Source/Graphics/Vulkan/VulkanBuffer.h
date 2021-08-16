#pragma once
#include "Graphics/GHI/GHIBuffer.h"

#include "VulkanCommon.h"

class VulkanBuffer : public GHIBuffer
{
public:
    vk::Buffer buffer;
    VmaAllocation allocation;
    VmaAllocator allocator;

public:
    void Upload(void const* data, uint size) override;

};

