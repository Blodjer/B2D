#pragma once

#include "Core/Core.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"

#include "VulkanCommon.h"

class VulkanDevice;
class VulkanSurface;
class VulkanRenderPass;

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

    virtual GHITexture* CreateTexture(uint32 width, uint32 height, EGHITextureFormat format, EGHITextureUsageFlags usage) override;
    virtual GHITexture* CreateTexture(void const* data, uint32 width, uint32 height, uint8 components) override { B2D_NOT_IMPLEMENTED(); }
    virtual void DestroyTexture(GHITexture* texture) override;

    virtual GHIRenderPass* CreateRenderPass(std::vector<GHITexture*> const& renderTargets, GHITexture const* depthTarget) override;
    virtual void DestroyRenderPass(GHIRenderPass* renderPass) override;
    /* TMP */ void CreateBasePipeline(GHIRenderPass const* renderPass);

    virtual void BeginRenderPass(GHIRenderPass* renderPass, GHICommandList* commandBuffer) override;
    virtual void EndRenderPass(GHIRenderPass* renderPass, GHICommandList* commandBuffer) override;

    virtual GHICommandList* AllocateCommandBuffer();
    virtual void FreeCommandBuffer(GHICommandList* commandBuffer) override;

    virtual void Submit(std::vector<GHICommandList*>& commandLists) override;

    virtual GHIBuffer* CreateBuffer(EGHIBufferType bufferType, uint size) override;

protected:
    virtual bool ImGui_Init() override;
    virtual void ImGui_Shutdow() override;
    virtual void ImGui_BeginFrame() override;
    virtual void ImGui_Render(GHICommandList* commandBuffer) override;
    vk::DescriptorPool m_imguiDescriptorpool;

private:
    vk::Instance m_instance;
public: // TMP
    VulkanDevice* m_device = nullptr;
private:
    VmaAllocator m_allocator;

    vk::DebugUtilsMessengerEXT m_debugUtilMessenger;

    vk::CommandPool m_commandPool;

    vk::Pipeline m_pipeline;
    
    VulkanSurface* m_primarySurface = nullptr; // TMP

public:
    vk::PipelineLayout m_pipelineLayout; // TMP
    std::vector<vk::DescriptorSet> m_descriptorSets; // TMP
    vk::DescriptorSet m_descriptorSet2; // TMP
};

