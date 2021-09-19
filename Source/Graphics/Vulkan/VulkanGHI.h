#pragma once

#include "Core/Core.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"

#include "VulkanCommon.h"

class VulkanDevice;
class VulkanSurface;
class VulkanRenderPass;
class VulkanBuffer;
class VulkanCommandBuffer;

class VulkanGHI final : public IGraphicsHardwareInterface
{
public:
    virtual bool Init() override;
    virtual void Shutdown() override;

    EGraphicsAPI GetGraphicsAPI() const override { return EGraphicsAPI::Vulkan; };

private:
    void SelectInstanceExtensionsAndLayers(std::vector<char const*>& outExtensions, std::vector<char const*>& outLayers) const;
    void SelectDeviceExtensions(vk::PhysicalDevice const& device, std::vector<char const*>& outExtensions) const;

    static std::vector<char const*> GetWantedInstanceExtenstions();
    static std::vector<char const*> GetWantedInstanceLayers();
    static std::vector<char const*> GetWantedDeviceExtenstions();
    static void LogMissing(std::string const& name, std::vector<const char*> const& wanted, std::vector<const char*>& received);

    vk::PhysicalDevice SelectPhysicalDevice(std::vector<vk::PhysicalDevice> const& physicalDevices) const;

public:
    virtual GHISurface* CreateSurface(void* nativeWindowHandle, uint32 width, uint32 height) override;
    virtual void DestroySurface(GHISurface* surface) override;

public:
    virtual GHIShader* CreateVertexShader(std::vector<uint32> const& data) override;
    virtual GHIShader* CreatePixelShader(std::vector<uint32> const& data) override;
    GHIShader* CreateShader(std::vector<uint32> const& data, vk::ShaderStageFlagBits stage);
    virtual void DestroyShader(GHIShader*& shader) override;

    virtual GHIGraphicsPipeline* CreateGraphicsPipeline(GHIRenderPass const* targetPass, VertexShaderRef vertexShader, PixelShaderRef pixelShader) override;
    virtual void DestroyGraphicsPipeline(GHIGraphicsPipeline* pipeline) override;

    virtual GHIResourceSet* CreateResourceSet() override;
    virtual void DestroyResourceSet(GHIResourceSet* resourceSet) override;

    virtual GHITexture* CreateTexture(uint32 width, uint32 height, EGHITextureFormat format, EGHITextureUsageFlags usage) override;
    virtual GHITexture* CreateTexture(void const* data, uint32 width, uint32 height, EGHITextureFormat format) override;
    virtual void DestroyTexture(GHITexture* texture) override;

    virtual GHISampler* CreateSampler() override;

    virtual GHIRenderPass* CreateRenderPass(std::vector<GHITexture*> const& renderTargets, GHITexture const* depthTarget) override;
    virtual void DestroyRenderPass(GHIRenderPass* renderPass) override;

    virtual void BeginRenderPass(GHIRenderPass* renderPass, GHICommandBuffer* commandBuffer) override;
    virtual void EndRenderPass(GHIRenderPass* renderPass, GHICommandBuffer* commandBuffer) override;

    virtual GHICommandBuffer* AllocateCommandBuffer();
    virtual void FreeCommandBuffer(GHICommandBuffer* commandBuffer) override;
    void WaitAndResetCommandBuffer(VulkanCommandBuffer* commandBuffer);
    void DestroyCommandBuffer(VulkanCommandBuffer* commandBuffer);
    void UpdateCommandBufferStatus();

    virtual void Submit(std::vector<GHICommandBuffer*>& commandBuffer) override;
    void Submit(std::vector<GHICommandBuffer*>& commandBuffer, vk::ArrayProxyNoTemporaries<vk::Semaphore const> const& waitSemaphores, vk::ArrayProxyNoTemporaries<vk::PipelineStageFlags const> const& waitStages, vk::ArrayProxyNoTemporaries<vk::Semaphore const> const& signalSemaphores);
    void ImmediateSubmit(std::function<void(VulkanCommandBuffer&)>&& function);

    virtual GHIBuffer* CreateBuffer(EGHIBufferType bufferType, uint size) override;
    VulkanBuffer* CreateBuffer(uint size, vk::BufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage);
    virtual void DestroyBuffer(GHIBuffer* buffer) override;

    static vk::Format Convert(EGHITextureFormat textureFormat);

protected:
    virtual bool ImGui_Init() override;
    virtual void ImGui_Shutdow() override;
    virtual void ImGui_BeginFrame() override;
    virtual void ImGui_Render(GHICommandBuffer* commandBuffer) override;
    vk::DescriptorPool m_imguiDescriptorpool;

private:
    vk::Instance m_instance;
    VulkanDevice* m_device = nullptr;

    VmaAllocator m_allocator;

    vk::DebugUtilsMessengerEXT m_debugUtilMessenger;

    vk::CommandPool m_commandPool;
    vk::CommandPool m_immediateCommandPool;

    std::vector<VulkanCommandBuffer*> m_availableCommandBuffers;
    std::vector<VulkanCommandBuffer*> m_pendingCommandBuffer;
};

