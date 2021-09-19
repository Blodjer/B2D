#include "B2D_pch.h"
#include "VulkanSurface.h"

#include "VulkanDevice.h"
#include "VulkanTexture.h"
#include "VulkanCommandBuffer.h"

#include "GameEngine.h" // TMP
#include "VulkanGHI.h" // TMP

VulkanSurface::VulkanSurface(vk::Instance instance, VulkanDevice const& device, void* nativeWindowHandle)
    : m_instance(instance)
    , m_device(device)
{
    {
#ifdef B2D_PLATFORM_WINDOWS
        vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
        surfaceCreateInfo.hwnd = static_cast<HWND>(nativeWindowHandle);
        surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
        m_surface = m_instance.createWin32SurfaceKHR(surfaceCreateInfo);
#else
        B2D_COMPILE_ERROR("Surface creation not implemented for this platform!");
#endif

        B2D_ASSERT_f(m_surface, "Failed to create vulkan surface!");
    }

    CreateSwapchain();

    vk::SemaphoreCreateInfo semaphoreInfo;
    auto ghi = static_cast<VulkanGHI*>(GameEngine::Instance()->GetGHI());
    for (Flight& flight : m_flights)
    {
        flight.imageAvailableSemaphore = m_device.GetLogical().createSemaphore(semaphoreInfo);
        flight.transitionSemaphore = m_device.GetLogical().createSemaphore(semaphoreInfo);
        flight.commandBuffer = static_cast<VulkanCommandBuffer*>(ghi->AllocateCommandBuffer());
    }
}

VulkanSurface::~VulkanSurface()
{
    m_device.GetLogical().waitIdle();

    auto ghi = static_cast<VulkanGHI*>(GameEngine::Instance()->GetGHI());
    for (Flight& flight : m_flights)
    {
        m_device.GetLogical().destroySemaphore(flight.imageAvailableSemaphore);
        m_device.GetLogical().destroySemaphore(flight.transitionSemaphore);
        ghi->FreeCommandBuffer(flight.commandBuffer);
    }

    CleanupSwapchain();

    m_instance.destroySurfaceKHR(m_surface);
}

void VulkanSurface::CreateSwapchain(vk::SwapchainKHR oldSwapchain)
{
    vk::PhysicalDevice const& physicalDevice = m_device.GetPhysical();
    vk::Device const& logicalDevice = m_device.GetLogical();

    vk::Bool32 isPresentSupported = m_device.GetPhysical().getSurfaceSupportKHR(m_device.GetPresentQueue().GetFamilyIndex(), m_surface);
    B2D_ASSERT(isPresentSupported);

    vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(m_surface);
    m_extent = SelectSwapExtend(surfaceCapabilities);

    std::vector<vk::SurfaceFormatKHR> surfaceFormats = physicalDevice.getSurfaceFormatsKHR(m_surface);
    B2D_ASSERT(!surfaceFormats.empty());

    vk::SurfaceFormatKHR surfaceFormat = SelectSurfaceFormat(surfaceFormats);

    std::vector<vk::PresentModeKHR> surfacePresentModes = physicalDevice.getSurfacePresentModesKHR(m_surface);
    B2D_ASSERT(!surfacePresentModes.empty());

    vk::PresentModeKHR surfacePresentMode = SelectSurfacePresentMode(surfacePresentModes);

    uint32 imageCount = UMath::Max<uint32>(surfaceCapabilities.minImageCount, 3);
    if (surfaceCapabilities.maxImageCount > 0)
    {
        imageCount = UMath::Min<uint32>(imageCount, surfaceCapabilities.maxImageCount);
    }

    vk::SwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.surface = m_surface;
    swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = m_extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst; // TODO: Is eTransferDst supported?
    swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapchainCreateInfo.presentMode = surfacePresentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = oldSwapchain;

    if (isPresentSupported)
    {
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    }
    else
    {
        B2D_NOT_IMPLEMENTED();
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        //swapchainCreateInfo.setQueueFamilyIndices();
        //swapchainCreateInfo.queueFamilyIndexCount = 2;
        //swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    m_swapchain = logicalDevice.createSwapchainKHR(swapchainCreateInfo);

    m_swapchainImages = logicalDevice.getSwapchainImagesKHR(m_swapchain);
}

void VulkanSurface::CleanupSwapchain()
{
    m_swapchainImages.clear();
    m_currentImage = nullptr;

    m_device.GetLogical().destroySwapchainKHR(m_swapchain);
    m_swapchain = nullptr;
}

void VulkanSurface::RecreateSwapchain()
{
    m_device.GetLogical().waitIdle();

    CleanupSwapchain();
    CreateSwapchain(m_swapchain);
}

void VulkanSurface::Resize(uint32 width, uint32 height)
{
    RecreateSwapchain();
}

// TMP
void Transition(VulkanCommandBuffer& cb,
    vk::PipelineStageFlags srcStageMask,
    vk::PipelineStageFlags dstStageMask,
    vk::AccessFlags srcAccessMask,
    vk::AccessFlags dstAccessMask,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout,
    vk::Image image)
{
    vk::ImageMemoryBarrier imageMemoryBarrier;
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;
    imageMemoryBarrier.oldLayout = oldLayout;
    imageMemoryBarrier.newLayout = newLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    imageMemoryBarrier.subresourceRange.layerCount = 1;
    imageMemoryBarrier.subresourceRange.levelCount = 1;

    cb.Get().pipelineBarrier(srcStageMask, dstStageMask, vk::DependencyFlags(0), nullptr, nullptr, imageMemoryBarrier);
}

void VulkanSurface::Present(GHITexture const* renderTarget)
{
    uint32 const maxAttempts = 1; // Max attempts to recreate the swapchain
    
    for (uint32 attempts = 0; ; ++attempts)
    {
        EPresentResult const presentResult = TryPresent(static_cast<VulkanTexture const*>(renderTarget));
        if (presentResult == EPresentResult::Success)
        {
            break;
        }
        else if (attempts < maxAttempts && presentResult == EPresentResult::RecreateSwapchain)
        {
            RecreateSwapchain();
        }
        else
        {
            B2D_TRAP_f("Failed to present!");
            return;
        }
    }

    m_currentFlightIndex = (m_currentFlightIndex + 1) % m_flights.size();
}

VulkanSurface::EPresentResult VulkanSurface::TryPresent(VulkanTexture const* renderTarget)
{
    if (renderTarget->GetWidth() > m_extent.width || renderTarget->GetHeight() > m_extent.height)
    {
        // Maybe the window was resized and we just need to recreate the swapchain
        B2D_LOG_WARNING("Render target is larger the swapchain. Presenting might fail!");
        return EPresentResult::RecreateSwapchain;
    }

    VulkanGHI* ghi = static_cast<VulkanGHI*>(GameEngine::Instance()->GetGHI());

    Flight& flight = m_flights[m_currentFlightIndex];

    if (flight.commandBuffer->IsSubmitted())
    {
        ghi->WaitAndResetCommandBuffer(flight.commandBuffer);
    }

    vk::ResultValue<uint32> acquireImageResult = m_device.GetLogical().acquireNextImageKHR(m_swapchain, UINT64_MAX, flight.imageAvailableSemaphore);
    if (acquireImageResult.result == vk::Result::eErrorOutOfDateKHR || acquireImageResult.result == vk::Result::eSuboptimalKHR)
    {
        return EPresentResult::RecreateSwapchain;
    }
    else if (acquireImageResult.result != vk::Result::eSuccess)
    {
        B2D_TRAP_f("Failed to acquire next image from swapchain!");
        return EPresentResult::Failed;
    }

    m_currentImageIndex = acquireImageResult.value;
    m_currentImage = m_swapchainImages[m_currentImageIndex];

    {
        VulkanCommandBuffer& commandBuffer = *flight.commandBuffer;

        commandBuffer.Begin();

        Transition(commandBuffer,
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eTransfer,
            vk::AccessFlags(0),
            vk::AccessFlagBits::eTransferWrite,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal,
            m_currentImage);

        vk::ImageCopy copy;
        copy.extent.width = renderTarget->GetWidth();
        copy.extent.height = renderTarget->GetHeight();
        copy.extent.depth = 1;
        copy.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        copy.srcSubresource.layerCount = 1;
        copy.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        copy.dstSubresource.layerCount = 1;

        commandBuffer.Get().copyImage(renderTarget->m_image, vk::ImageLayout::eTransferSrcOptimal, m_currentImage, vk::ImageLayout::eTransferDstOptimal, copy);

        Transition(commandBuffer,
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::AccessFlagBits::eTransferWrite,
            vk::AccessFlagBits::eMemoryRead,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::ePresentSrcKHR,
            m_currentImage);

        commandBuffer.End();

        std::vector<GHICommandBuffer*> commandBuffers = { &commandBuffer };
        std::array<vk::Semaphore, 1> waitSemaphores = { flight.imageAvailableSemaphore };
        std::array<vk::PipelineStageFlags, 1> waitStages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        std::array<vk::Semaphore, 1> signalSemaphores = { flight.transitionSemaphore };

        ghi->Submit(commandBuffers, waitSemaphores, waitStages, signalSemaphores);
    }

    {
        std::array<vk::Semaphore, 1> waitSemaphores = { flight.transitionSemaphore };

        vk::PresentInfoKHR presentInfo;
        presentInfo.setWaitSemaphores(waitSemaphores);
        presentInfo.setSwapchains(m_swapchain);
        presentInfo.pImageIndices = &m_currentImageIndex;

        vk::Queue presentQueue = m_device.GetPresentQueue().GetHandle();

        VkPresentInfoKHR const vkPresentInfo = presentInfo;
        vk::Result presentResult = static_cast<vk::Result>(vkQueuePresentKHR(presentQueue, &vkPresentInfo)); // Use C implementation to prevent an assert on error
        if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR)
        {
            return EPresentResult::RecreateSwapchain;
        }
        else if (presentResult != vk::Result::eSuccess)
        {
            B2D_TRAP_f("Failed to present to swapchain!");
            return EPresentResult::Failed;
        }
    }

    return EPresentResult::Success;
}

vk::SurfaceFormatKHR VulkanSurface::SelectSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const& availableSurfaceFormats)
{
    for (vk::SurfaceFormatKHR const& surfaceFormat : availableSurfaceFormats)
    {
        //if (surfaceFormat.format == vk::Format::eB8G8R8A8Srgb && surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        if (surfaceFormat.format == vk::Format::eB8G8R8A8Unorm && surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) // TODO: To be compatible with ImGui
        {
            return surfaceFormat;
        }
    }

    // Fallback
    return availableSurfaceFormats[0];
}

vk::PresentModeKHR VulkanSurface::SelectSurfacePresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes)
{
    for (vk::PresentModeKHR const& presentMode : availablePresentModes)
    {
        if (presentMode == vk::PresentModeKHR::eMailbox)
        {
            return presentMode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanSurface::SelectSwapExtend(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities)
{
    if (surfaceCapabilities.currentExtent.width != 0xFFFFFFFF)
    {
        return surfaceCapabilities.currentExtent;
    }

    vk::Extent2D extend;
    extend.width = UMath::Clamp(100u, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
    extend.height = UMath::Clamp(100u, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

    return extend;
}
