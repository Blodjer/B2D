#pragma once

#include "Core/Core.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"

#include "VulkanCommon.h"

class VulkanGHI final : public IGraphicsHardwareInterface
{
public:
    virtual bool Init() override;
    virtual void Shutdown() override;
    virtual void BeginRenderPass() override;
    virtual void EndRenderPass() override;

    EGraphicsAPI GetGraphicsAPI() const override { return EGraphicsAPI::Vulkan; };

private:
    void SelectInstanceExtensionsAndLayers(std::vector<char const*>& outExtensions, std::vector<char const*>& outLayers) const;
    void SelectDeviceExtensions(vk::PhysicalDevice const& device, std::vector<char const*>& outExtensions) const;
    bool FindQueueIndex(vk::PhysicalDevice& physicalDevice, vk::QueueFlagBits flags, uint32& outIndex);

    static std::vector<char const*> GetWantedInstanceExtenstions();
    static std::vector<char const*> GetWantedInstanceLayers();
    static std::vector<char const*> GetWantedDeviceExtenstions();
    static void LogMissing(std::string const& name, std::vector<const char*> const& wanted, std::vector<const char*>& received);

    vk::PhysicalDevice SelectPhysicalDevice(std::vector<vk::PhysicalDevice> const& physicalDevices) const;

    vk::SurfaceFormatKHR SelectSurfacePresentMode(std::vector<vk::SurfaceFormatKHR> const& availableSurfaceFormats);
    vk::PresentModeKHR SelectSurfacePresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes);
    vk::Extent2D SelectSwapExtend(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities);

public:
    virtual GHITexture* CreateTexture(void const* data, uint32 width, uint32 height, uint8 components) override;
    virtual void FreeTexture(GHITexture*& texture) override;

    virtual GHIShader* CreateVertexShader(std::vector<uint32> const& data) override;
    virtual GHIShader* CreatePixelShader(std::vector<uint32> const& data) override;
    GHIShader* CreateShader(std::vector<uint32> const& data, vk::ShaderStageFlagBits stage);
    virtual void DeleteShader(GHIShader*& shader) override;

    virtual GHIMaterial* CreateMaterial(GHIShader* vertexShader, GHIShader* pixelShader) override;
    virtual void FreeMaterial(GHIMaterial*& material) override;

    virtual GHIRenderTarget* CreateRenderTarget() override;
    virtual GHIRenderTarget* CreateRenderTarget(GHITexture* texture) override;
    virtual void ResizeRenderTarget(GHIRenderTarget*& renderTarget, uint32 width, uint32 height);
    virtual void DeleteRenderTarget(GHIRenderTarget*& renderTarget, bool freeTexture) override;

public:
    virtual void Clear(bool color, bool depth, bool stencil) override;
    virtual void BindTexture(GHITexture const* texture) override;
    virtual void BindMaterial(GHIMaterial* material) override;
    virtual void BindRenderTarget(GHIRenderTarget* renderTarget) override;
    virtual void BindRenderTargetAndClear(GHIRenderTarget* renderTarget) override;

protected:
    virtual bool ImGui_Init() override;
    virtual void ImGui_Shutdow() override;
    virtual void ImGui_BeginFrame() override;
    virtual void ImGui_Render() override;

private:
    vk::Instance m_instance;
    vk::DebugUtilsMessengerEXT m_debugUtilMessenger;

    vk::PhysicalDevice m_physicalDevice;
    vk::Device m_device;

    vk::PipelineLayout m_pipelineLayout;
    vk::Pipeline m_pipeline;

    vk::SwapchainKHR m_swapchain;
    vk::Extent2D m_extent;
    std::vector<vk::Framebuffer> m_framebuffers;
    std::vector<vk::CommandBuffer> m_commandBuffers;

    vk::RenderPass m_renderPass;

    vk::Queue m_graphicsQueue;
    vk::Queue m_presentationQueue;

    vk::Semaphore m_imageAvailableSemaphore;
    vk::Semaphore m_renderFinishedSemaphore;

    uint32 m_currentImageIndex = 0; // TMP
};

