#pragma once
#include "Graphics/GHI/GHISurface.h"

#include "VulkanCommon.h"

class VulkanDevice;
class VulkanTexture;
class VulkanCommandBuffer;

class VulkanSurface : public GHISurface
{
private:
    enum class EPresentResult
    {
        Success,
        Failed,
        RecreateSwapchain,
    };

public:
    VulkanSurface(vk::Instance instance, VulkanDevice const& device, void* nativeWindowHandle);
    virtual ~VulkanSurface();
    
private:
    void CreateSwapchain(vk::SwapchainKHR oldSwapchain = nullptr);
    void CleanupSwapchain();
    void RecreateSwapchain();

public:
    virtual void Resize(uint32 width, uint32 height) override;
    virtual void Present(GHITexture const* renderTarget) override;    

private:
    EPresentResult TryPresent(VulkanTexture const* renderTarget);

private:
    vk::SurfaceFormatKHR SelectSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableSurfaceFormats);
    vk::PresentModeKHR SelectSurfacePresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes);
    vk::Extent2D SelectSwapExtend(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities);

private:
    vk::SurfaceKHR m_surface;
    vk::SwapchainKHR m_swapchain;

    vk::Extent2D m_extent;

    vk::Instance m_instance;
    VulkanDevice const& m_device;

    std::vector<vk::Image> m_swapchainImages;
    uint32 m_currentImageIndex = 0;
    vk::Image m_currentImage;

    struct Flight
    {
        vk::Semaphore imageAvailableSemaphore;
        vk::Semaphore transitionSemaphore;
        VulkanCommandBuffer* commandBuffer;
    };

    INLINE static uint constexpr MAX_FRAMES_IN_FLIGHT = 2;
    std::array<Flight, MAX_FRAMES_IN_FLIGHT> m_flights;
    uint32 m_currentFlightIndex = 0;
};
