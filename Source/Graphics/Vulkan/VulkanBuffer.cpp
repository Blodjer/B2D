#include "B2D_pch.h"
#include "VulkanBuffer.h"

void VulkanBuffer::Upload(void const* data, uint size)
{
    void* mappedMemory;
    vmaMapMemory(allocator, allocation, &mappedMemory);

    memcpy(mappedMemory, data, size);

    vmaUnmapMemory(allocator, allocation);
}
