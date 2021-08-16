#include "B2D_pch.h"
#include "VulkanGHI.h"

#include "GameEngine.h"

#include "Graphics/Shader.h" // TMP
#include "Graphics/Mesh.h" // TMP

#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "VulkanSurface.h"
#include "VulkanRenderPass.h"
#include "VulkanTexture.h"
#include "VulkanCommandList.h"
#include "VulkanBuffer.h"

#include "Editor/ImGuiCommon.h"
#include "Editor/imgui_impl_vulkan.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace
{
    bool constexpr LOG_AVAILABLE = false;

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
        void* pUserData)
    {
        // TODO: Add custom sink?
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            B2D_LOG_INFO_s(Log::ESink::External, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            B2D_LOG_WARNING_s(Log::ESink::External, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            B2D_LOG_ERROR_s(Log::ESink::External, pCallbackData->pMessage);
            break;
        default:
            B2D_BREAK_f("Unkown Vulkan message severity!");
            B2D_LOG_WARNING_s(Log::ESink::External, pCallbackData->pMessage);
            break;
        }

        return VK_FALSE;
    }

    void FindMatchingExtensions(std::vector<vk::ExtensionProperties> const& available, std::vector<const char*> const& wanted, std::vector<const char*>& out)
    {
        for (char const* const& w : wanted)
        {
            for (vk::ExtensionProperties const& a : available)
            {
                if (strcmp(a.extensionName, w) == 0)
                {
                    out.emplace_back(w);
                    break;
                }
            }
        }
    }

    void FindMatchingLayers(std::vector<vk::LayerProperties> const& available, std::vector<const char*> const& wanted, std::vector<const char*>& out)
    {
        for (char const* const& w : wanted)
        {
            for (vk::LayerProperties const& a : available)
            {
                if (strcmp(a.layerName, w) == 0)
                {
                    out.emplace_back(w);
                    break;
                }
            }
        }
    }
}

bool VulkanGHI::Init()
{
    GameEngine const* const engine = GameEngine::Instance();

    B2D_LOG_INFO("Initialize Vulkan...");
    B2D_LOG_INFO("Vulkan version\t{}.{}.{}",
        VK_VERSION_MAJOR(VK_HEADER_VERSION_COMPLETE),
        VK_VERSION_MINOR(VK_HEADER_VERSION_COMPLETE),
        VK_VERSION_PATCH(VK_HEADER_VERSION_COMPLETE)
    );

    vk::DynamicLoader dynamicLoader;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    vk::ApplicationInfo appInfo;
    appInfo.pApplicationName = engine->GetApplicationName().c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.pEngineName = engine->GetEngineName();
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    std::vector<char const*> extensionsToEnable;
    std::vector<char const*> layersToEnable;
    SelectInstanceExtensionsAndLayers(extensionsToEnable, layersToEnable);

    vk::InstanceCreateInfo instanceCreateInfo;
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.setPEnabledLayerNames(layersToEnable);
    instanceCreateInfo.setPEnabledExtensionNames(extensionsToEnable);

    vk::Result const createInstanceResult = vk::createInstance(&instanceCreateInfo, nullptr, &m_instance);
    if (createInstanceResult != vk::Result::eSuccess)
    {
        B2D_TRAP_f("Failed to create Vulkan instance! ({})", vk::to_string(createInstanceResult), createInstanceResult);
        return false;
    }
    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance);

    vk::DebugUtilsMessengerCreateInfoEXT createDebugInfo;
    createDebugInfo.messageSeverity =
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
    createDebugInfo.messageType =
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
    createDebugInfo.pfnUserCallback = DebugCallback;

    m_debugUtilMessenger = m_instance.createDebugUtilsMessengerEXT(createDebugInfo);

    std::vector<vk::PhysicalDevice> physicalDevices = m_instance.enumeratePhysicalDevices();
    B2D_ASSERT(!physicalDevices.empty());

    vk::PhysicalDevice physicalDevice = SelectPhysicalDevice(physicalDevices);

    std::vector<char const*> deviceExtensionsToEnable;
    SelectDeviceExtensions(physicalDevice, deviceExtensionsToEnable);

    m_device = new VulkanDevice(m_instance, physicalDevice, deviceExtensionsToEnable);

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    allocatorInfo.physicalDevice = m_device->GetPhysical();
    allocatorInfo.device = m_device->GetLogical();
    allocatorInfo.instance = m_instance;
    vmaCreateAllocator(&allocatorInfo, &m_allocator);

    vk::CommandPoolCreateInfo commandPoolInfo;
    commandPoolInfo.queueFamilyIndex = m_device->GetGraphicsQueue().GetFamilyIndex();
    commandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer; // TODO

    m_commandPool = m_device->GetLogical().createCommandPool(commandPoolInfo);

    return true;
}

void VulkanGHI::Shutdown()
{
    m_device->GetLogical().waitIdle();

    m_device->GetLogical().destroyCommandPool(m_commandPool);

    m_device->GetLogical().destroyPipeline(m_pipeline);
    m_device->GetLogical().destroyPipelineLayout(m_pipelineLayout);

    vmaDestroyAllocator(m_allocator);

    delete m_device;
    m_device = nullptr;

    // Don't destroy the debug messenger to catch messages triggered by the instance destroy function
    //m_instance.destroyDebugUtilsMessengerEXT(m_debugUtilMessenger);
    //m_debugUtilMessenger = nullptr;

    m_instance.destroy();
    m_instance = nullptr;
}

void VulkanGHI::SelectInstanceExtensionsAndLayers(std::vector<char const*>& outExtensions, std::vector<char const*>& outLayers) const
{
    //Extensions
    std::vector<vk::ExtensionProperties> const availableExtensions = vk::enumerateInstanceExtensionProperties();

    if constexpr (LOG_AVAILABLE)
    {
        B2D_LOG_INFO("{} available extensions:", availableExtensions.size());
        for (vk::ExtensionProperties const& extension : availableExtensions)
        {
            B2D_LOG_INFO("\t{}", extension.extensionName);
        }
    }

    std::vector<char const*> wantedExtensions = GetWantedInstanceExtenstions();
    FindMatchingExtensions(availableExtensions, wantedExtensions, outExtensions);
    LogMissing("Extensions not available...", wantedExtensions, outExtensions);

    //Layers
    std::vector<vk::LayerProperties> const availableLayers = vk::enumerateInstanceLayerProperties();

    if constexpr (LOG_AVAILABLE)
    {
        B2D_LOG_INFO("{} available layers:", availableLayers.size());
        for (vk::LayerProperties const& layer : availableLayers)
        {
            B2D_LOG_INFO("\t{} ({})", layer.layerName, layer.description);
        }
    }

    std::vector<char const*> const wantedLayers = GetWantedInstanceLayers();
    FindMatchingLayers(availableLayers, wantedLayers, outLayers);
    LogMissing("Layers not available...", wantedLayers, outLayers);
}

void VulkanGHI::SelectDeviceExtensions(vk::PhysicalDevice const& device, std::vector<char const*>& outExtensions) const
{
    std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();

    if constexpr (LOG_AVAILABLE)
    {
        B2D_LOG_INFO("{} available device extensions:", availableExtensions.size());
        for (vk::ExtensionProperties const& extension : availableExtensions)
        {
            B2D_LOG_INFO("\t{}", extension.extensionName);
        }
    }

    std::vector<char const*> wantedExtensions = GetWantedDeviceExtenstions();
    FindMatchingExtensions(availableExtensions, wantedExtensions, outExtensions);
    LogMissing("Device extensions not available...", wantedExtensions, outExtensions);
    B2D_ASSERT(wantedExtensions.size() == outExtensions.size());
}

std::vector<char const*> VulkanGHI::GetWantedInstanceExtenstions()
{
    std::vector<char const*> const wantedExtensions =
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,

#ifdef VK_USE_PLATFORM_WIN32_KHR
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif VK_USE_PLATFORM_MACOS_MVK
        VK_MVK_MACOS_SURFACE_EXTENSION_NAME,
#elif VK_USE_PLATFORM_XCB_KHR
        VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#elif VK_USE_PLATFORM_ANDROID_KHR
        VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
#elif VK_USE_PLATFORM_XLIB_KHR
        VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#elif VK_USE_PLATFORM_XCB_KHR
        VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#elif VK_USE_PLATFORM_WAYLAND_KHR
        VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
#elif VK_USE_PLATFORM_MIR_KHR || VK_USE_PLATFORM_DISPLAY_KHR
        VK_KHR_DISPLAY_EXTENSION_NAME,
#elif VK_USE_PLATFORM_ANDROID_KHR
        VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
#elif VK_USE_PLATFORM_IOS_MVK
        VK_MVK_IOS_SURFACE_EXTENSION_NAME,
#endif
    };

    return wantedExtensions;
}

std::vector<char const*> VulkanGHI::GetWantedInstanceLayers()
{
    std::vector<char const*> const wantedLayers =
    {
#ifdef B2D_BUILD_DEBUG
          "VK_LAYER_KHRONOS_validation"
#endif
    };

    return wantedLayers;
}

std::vector<char const*> VulkanGHI::GetWantedDeviceExtenstions()
{
    std::vector<char const*> wantedExtensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    return wantedExtensions;
}

void VulkanGHI::LogMissing(std::string const& message, std::vector<const char*> const& wanted, std::vector<const char*>& received)
{
    if (wanted.size() != received.size())
    {
        B2D_LOG_WARNING(message.c_str());

        for (char const* const& w : wanted)
        {
            bool contains = false;
            for (char const* const& r : received)
            {
                if (strcmp(w, r) == 0)
                {
                    contains = true;
                    break;
                }
            }

            if (!contains)
            {
                B2D_LOG_WARNING("\t{}", w);
            }
        }
    }
}

vk::PhysicalDevice VulkanGHI::SelectPhysicalDevice(std::vector<vk::PhysicalDevice> const& physicalDevices) const
{
    if constexpr (LOG_AVAILABLE)
    {
        B2D_LOG_INFO("{} available devices:", physicalDevices.size());
        for (vk::PhysicalDevice const& p : physicalDevices)
        {
            auto result = p.getProperties2KHR<vk::PhysicalDeviceProperties2, vk::PhysicalDeviceDriverProperties>();

            vk::PhysicalDeviceProperties& deviceProperties = result.get<vk::PhysicalDeviceProperties2>().properties;
            vk::PhysicalDeviceDriverPropertiesKHR& deviceDriverProperties = result.get<vk::PhysicalDeviceDriverPropertiesKHR>();
            B2D_LOG_INFO("\t {}", deviceProperties.deviceName);
            B2D_LOG_INFO("\t\t Driver \t{} {}", deviceDriverProperties.driverName, deviceDriverProperties.driverInfo);
            B2D_LOG_INFO("\t\t Device Type \t{}", vk::to_string(deviceProperties.deviceType));
        }
    }

    return physicalDevices[0];
}

GHISurface* VulkanGHI::CreateSurface(void* nativeWindowHandle, uint32 width, uint32 height)
{
    return m_primarySurface = new VulkanSurface(m_instance, *m_device, nativeWindowHandle);
}

void VulkanGHI::DestroySurface(GHISurface* surface)
{
    delete surface;
}

GHIShader* VulkanGHI::CreateVertexShader(std::vector<uint32> const& data)
{
    return CreateShader(data, vk::ShaderStageFlagBits::eVertex);
}

GHIShader* VulkanGHI::CreatePixelShader(std::vector<uint32> const& data)
{
    return CreateShader(data, vk::ShaderStageFlagBits::eFragment);
}

GHIShader* VulkanGHI::CreateShader(std::vector<uint32> const& data, vk::ShaderStageFlagBits stage)
{
    // Compile Project/Content/Shaders/ to intermediate directory Project/Intermediate/Shaders/
    // 
    // Load shader function only takes name of shader. Extension and path is added at runtime.
    // Path might be engine or project relative
    // 
    // Preload all shaders at beginning
    // (optional) Access shaders by key, Preload creates key

    vk::ShaderModuleCreateInfo shaderCreateInfo;
    shaderCreateInfo.codeSize = data.size() * sizeof(uint32);
    shaderCreateInfo.pCode = data.data();

    vk::ShaderModule shaderModule = m_device->GetLogical().createShaderModule(shaderCreateInfo);

    vk::PipelineShaderStageCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.stage = stage;
    pipelineCreateInfo.module = shaderModule;
    pipelineCreateInfo.pName = "main";
    
    return new VulkanShader(shaderModule, pipelineCreateInfo);
}

void VulkanGHI::DestroyShader(GHIShader*& ghiShader)
{
    VulkanShader* shader = static_cast<VulkanShader*>(ghiShader);
    m_device->GetLogical().destroyShaderModule(shader->GetShaderModule());

    delete shader;
    ghiShader = nullptr;
}

GHITexture* VulkanGHI::CreateTexture(uint32 width, uint32 height, EGHITextureFormat format, EGHITextureUsageFlags usage)
{
    vk::Extent3D extent;
    extent.width = width;
    extent.height = height;
    extent.depth = 1;

    vk::ImageCreateInfo imageCreateInfo;
    imageCreateInfo.extent = extent;
    imageCreateInfo.imageType = vk::ImageType::e2D;
    imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferSrc;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    //imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    //imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    switch (format)
    {
    case EGHITextureFormat::RGBA8:
        imageCreateInfo.format = vk::Format::eR8G8B8A8Unorm;
        break;
    case EGHITextureFormat::BGRA8:
        imageCreateInfo.format = vk::Format::eB8G8R8A8Unorm;
        break;
    case EGHITextureFormat::Depth24:
        imageCreateInfo.format = vk::Format::eD24UnormS8Uint;
        break;
    case EGHITextureFormat::Depth24Stencil8:
        imageCreateInfo.format = vk::Format::eD24UnormS8Uint;
        break;
    default:
        B2D_TRAP_f("Format not supported by Vulkan!");
    }

    if (usage & EGHITextureUsageFlags::ColorAttachment)
    {
        imageCreateInfo.usage |= vk::ImageUsageFlagBits::eColorAttachment;
    }
    if (usage & EGHITextureUsageFlags::DepthStencilAttachment)
    {
        imageCreateInfo.usage |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
    }

    VulkanTexture* texture = new VulkanTexture(width, height);

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocationCreateInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkImage outImage;
    vmaCreateImage(m_allocator, &static_cast<VkImageCreateInfo>(imageCreateInfo), &allocationCreateInfo, &outImage, &texture->m_allocation, nullptr);

    vk::ImageViewCreateInfo createInfo;
    createInfo.image = outImage;
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = imageCreateInfo.format;
    createInfo.components.r = vk::ComponentSwizzle::eIdentity;
    createInfo.components.g = vk::ComponentSwizzle::eIdentity;
    createInfo.components.b = vk::ComponentSwizzle::eIdentity;
    createInfo.components.a = vk::ComponentSwizzle::eIdentity;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (usage & EGHITextureUsageFlags::ColorAttachment)
    {
        createInfo.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eColor;
    }
    if (usage & EGHITextureUsageFlags::DepthStencilAttachment)
    {
        createInfo.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eDepth;
    }

    texture->m_image = outImage;
    texture->m_imageView = m_device->GetLogical().createImageView(createInfo);
    texture->m_format = imageCreateInfo.format;

    return texture;
}

void VulkanGHI::DestroyTexture(GHITexture* texture)
{
    VulkanTexture* vkTexture = static_cast<VulkanTexture*>(texture);
    m_device->GetLogical().destroyImageView(vkTexture->m_imageView);
    vmaDestroyImage(m_allocator, vkTexture->m_image, vkTexture->m_allocation);
    delete vkTexture;
}

GHIRenderPass* VulkanGHI::CreateRenderPass(std::vector<GHITexture*> const& renderTargets, GHITexture const* depthTarget)
{
    VulkanRenderPass* renderPass = new VulkanRenderPass();

    vk::SubpassDescription subpass;
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

    std::vector<vk::AttachmentDescription> attachmentDescriptions(renderTargets.size());

    std::vector<vk::AttachmentReference> colorAttachmentRefs(renderTargets.size());
    for (uint i = 0; i < renderTargets.size(); ++i)
    {
        VulkanTexture const* vkRenderTarget = static_cast<VulkanTexture const*>(renderTargets[i]);

        vk::AttachmentDescription colorAttachment;
        colorAttachment.format = vkRenderTarget->m_format;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = depthTarget != nullptr ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = depthTarget != nullptr ? vk::ImageLayout::eUndefined : vk::ImageLayout::eTransferSrcOptimal;
        colorAttachment.finalLayout = vk::ImageLayout::eTransferSrcOptimal;

        attachmentDescriptions[i] = colorAttachment;

        vk::AttachmentReference colorAttachmentRef;
        colorAttachmentRef.attachment = i;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        colorAttachmentRefs[i] = colorAttachmentRef;
    }
    subpass.setColorAttachments(colorAttachmentRefs);

    if (depthTarget != nullptr)
    {
        VulkanTexture const* vkDepthTarget = static_cast<VulkanTexture const*>(depthTarget);

        vk::AttachmentDescription depthAttachment;
        depthAttachment.format = vkDepthTarget->m_format;
        depthAttachment.samples = vk::SampleCountFlagBits::e1;
        depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eClear;
        depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
        depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        attachmentDescriptions.emplace_back(depthAttachment);

        vk::AttachmentReference depthAttachmentRef;
        depthAttachmentRef.attachment = colorAttachmentRefs.size();
        depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        subpass.pDepthStencilAttachment = &depthAttachmentRef;
    }

    vk::SubpassDependency subpassDependency;
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependency.srcAccessMask = vk::AccessFlags(0);
    subpassDependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    vk::RenderPassCreateInfo renderPassInfo;
    renderPassInfo.setAttachments(attachmentDescriptions);
    renderPassInfo.setSubpasses(subpass);
    renderPassInfo.setDependencies(subpassDependency);

    renderPass->m_renderPass = m_device->GetLogical().createRenderPass(renderPassInfo);

    std::vector<vk::ImageView> imageViews;
    imageViews.reserve(renderTargets.size());

    uint32 width = 0;
    uint32 height = 0;

    for (GHITexture const* renderTarget : renderTargets)
    {
        VulkanTexture const* vkRenderTarget = static_cast<VulkanTexture const*>(renderTarget);
        imageViews.emplace_back(vkRenderTarget->m_imageView);

        if (width == 0 || height == 0)
        {
            width = vkRenderTarget->GetWidth();
            height = vkRenderTarget->GetHeight();
        }
        else
        {
            B2D_CHECK_f(width != vkRenderTarget->GetWidth() || height != vkRenderTarget->GetHeight(), "The size of all images needs to be equal to create a framebuffer!");
        }
    }

    if (depthTarget != nullptr)
    {
        VulkanTexture const* vkDepthTarget = static_cast<VulkanTexture const*>(depthTarget);
        imageViews.emplace_back(vkDepthTarget->m_imageView);
    }

    vk::FramebufferCreateInfo framebufferInfo;
    framebufferInfo.renderPass = renderPass->m_renderPass;
    framebufferInfo.setAttachments(imageViews);
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;

    renderPass->m_frameBuffer = m_device->GetLogical().createFramebuffer(framebufferInfo);
    renderPass->m_extent.width = width;
    renderPass->m_extent.height = height;

    vk::SemaphoreCreateInfo semaphoreInfo;
    renderPass->m_renderFinishedSemaphore = m_device->GetLogical().createSemaphore(semaphoreInfo);

    CreateBasePipeline(renderPass);

    return renderPass;
}

void VulkanGHI::DestroyRenderPass(GHIRenderPass* renderPass)
{
    VulkanRenderPass* vkRenderPass = static_cast<VulkanRenderPass*>(renderPass);
    m_device->GetLogical().destroyRenderPass(vkRenderPass->m_renderPass);
    m_device->GetLogical().destroyFramebuffer(vkRenderPass->m_frameBuffer);
    m_device->GetLogical().destroySemaphore(vkRenderPass->m_renderFinishedSemaphore);
    delete vkRenderPass;
}

void VulkanGHI::CreateBasePipeline(GHIRenderPass const* renderPass)
{
    if (m_pipeline)
    {
        return;
    }

    VulkanRenderPass const* vkRenderPass = static_cast<VulkanRenderPass const*>(renderPass);

    PixelShaderRef ps = IResourceManager::Get<PixelShader>("Content/Shader/Vulkan.fs.glsl");
    VertexShaderRef vs = IResourceManager::Get<VertexShader>("Content/Shader/Vulkan.vs.glsl");

    struct VertexInputDescription {

        std::vector<vk::VertexInputBindingDescription> bindings;
        std::vector<vk::VertexInputAttributeDescription> attributes;

        vk::PipelineVertexInputStateCreateFlags flags;
    } description;

    {
        // We will have just 1 vertex buffer binding, with a per-vertex rate
        vk::VertexInputBindingDescription mainBinding;
        mainBinding.binding = 0;
        mainBinding.stride = sizeof(Mesh::Vertex);
        mainBinding.inputRate = vk::VertexInputRate::eVertex;

        description.bindings.push_back(mainBinding);

        // Position will be stored at Location 0
        vk::VertexInputAttributeDescription positionAttribute;
        positionAttribute.binding = 0;
        positionAttribute.location = 0;
        positionAttribute.format = vk::Format::eR32G32B32Sfloat;
        positionAttribute.offset = offsetof(Mesh::Vertex, position);
        
        // Normal will be stored at Location 1
        vk::VertexInputAttributeDescription normalAttribute;
        normalAttribute.binding = 0;
        normalAttribute.location = 1;
        normalAttribute.format = vk::Format::eR32G32B32Sfloat;
        normalAttribute.offset = offsetof(Mesh::Vertex, normal);

        description.attributes.push_back(positionAttribute);
        description.attributes.push_back(normalAttribute);
    }

    vk::PipelineVertexInputStateCreateInfo vertexInputCreateInfo;
    vertexInputCreateInfo.setVertexBindingDescriptions(description.bindings);
    vertexInputCreateInfo.setVertexAttributeDescriptions(description.attributes);

    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
    inputAssemblyCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = vk::PolygonMode::eFill;
    rasterizationStateCreateInfo.lineWidth = 1.0f;
    rasterizationStateCreateInfo.cullMode = vk::CullModeFlagBits::eBack;
    rasterizationStateCreateInfo.frontFace = vk::FrontFace::eClockwise;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f; // Optional
    rasterizationStateCreateInfo.depthBiasClamp = 0.0f; // Optional
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f; // Optional

    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampleStateCreateInfo.minSampleShading = 1.0f; // Optional
    multisampleStateCreateInfo.pSampleMask = nullptr; // Optional
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE; // Optional

    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState;
    colorBlendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachmentState.blendEnable = VK_FALSE;
    colorBlendAttachmentState.srcColorBlendFactor = vk::BlendFactor::eOne; // Optional
    colorBlendAttachmentState.dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
    colorBlendAttachmentState.colorBlendOp = vk::BlendOp::eAdd; // Optional
    colorBlendAttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eOne; // Optional
    colorBlendAttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
    colorBlendAttachmentState.alphaBlendOp = vk::BlendOp::eAdd; // Optional

    vk::PipelineColorBlendStateCreateInfo colorBlendState;
    colorBlendState.logicOpEnable = VK_FALSE;
    colorBlendState.logicOp = vk::LogicOp::eCopy; // Optional
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &colorBlendAttachmentState;
    colorBlendState.blendConstants[0] = 0.0f; // Optional
    colorBlendState.blendConstants[1] = 0.0f; // Optional
    colorBlendState.blendConstants[2] = 0.0f; // Optional
    colorBlendState.blendConstants[3] = 0.0f; // Optional

    vk::PipelineDepthStencilStateCreateInfo depthStencilState;
    depthStencilState.depthTestEnable = true ? VK_TRUE : VK_FALSE; // TODO
    depthStencilState.depthWriteEnable = true ? VK_TRUE : VK_FALSE;
    depthStencilState.depthCompareOp = true ? vk::CompareOp::eLessOrEqual : vk::CompareOp::eAlways;
    depthStencilState.depthBoundsTestEnable = VK_FALSE;
    depthStencilState.minDepthBounds = 0.0f; // Optional
    depthStencilState.maxDepthBounds = 1.0f; // Optional
    depthStencilState.stencilTestEnable = VK_FALSE;

    /*
    if (blendEnable)
    {
        finalColor.rgb = (srcColorBlendFactor * newColor.rgb) < colorBlendOp > (dstColorBlendFactor * oldColor.rgb);
        finalColor.a = (srcAlphaBlendFactor * newColor.a) < alphaBlendOp > (dstAlphaBlendFactor * oldColor.a);
    }
    else
    {
        finalColor = newColor;
    }

    finalColor = finalColor & colorWriteMask;
    */

    struct MeshPushConstants
    {
        glm::vec4 data;
        TMatrix render_matrix;
    };

    vk::PushConstantRange push_constant;
    push_constant.offset = 0;
    push_constant.size = sizeof(MeshPushConstants);
    push_constant.stageFlags = vk::ShaderStageFlagBits::eVertex;

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.setPushConstantRanges(push_constant);

    m_pipelineLayout = m_device->GetLogical().createPipelineLayout(pipelineLayoutInfo);

    std::array<vk::DynamicState, 2> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    vk::PipelineDynamicStateCreateInfo dynamicState;
    dynamicState.setDynamicStates(dynamicStates);

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    if (ps.IsValid() && vs.IsValid())
    {
        VulkanShader* v_ps = static_cast<VulkanShader*>(ps->GetGHIShader());
        VulkanShader* v_vs = static_cast<VulkanShader*>(vs->GetGHIShader());

        shaderStages.emplace_back(v_ps->GetPipelineInfo());
        shaderStages.emplace_back(v_vs->GetPipelineInfo());

        vk::GraphicsPipelineCreateInfo pipelineInfo;
        pipelineInfo.setStages(shaderStages);
        pipelineInfo.pVertexInputState = &vertexInputCreateInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
        pipelineInfo.pViewportState = &viewportStateCreateInfo;
        pipelineInfo.pRasterizationState = &rasterizationStateCreateInfo;
        pipelineInfo.pMultisampleState = &multisampleStateCreateInfo;
        pipelineInfo.pDepthStencilState = &depthStencilState;
        pipelineInfo.pColorBlendState = &colorBlendState;
        pipelineInfo.pDynamicState = &dynamicState;

        pipelineInfo.layout = m_pipelineLayout;
        pipelineInfo.renderPass = vkRenderPass->m_renderPass;
        pipelineInfo.subpass = 0;

        pipelineInfo.basePipelineHandle = nullptr; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        m_pipeline = m_device->GetLogical().createGraphicsPipeline(nullptr, pipelineInfo).value;
    }
}

void VulkanGHI::BeginRenderPass(GHIRenderPass* renderPass, GHICommandList* commandBuffer)
{
    VulkanRenderPass* vkRenderPass = static_cast<VulkanRenderPass*>(renderPass);
    vk::CommandBuffer& vkCommandBuffer = static_cast<VulkanCommandList*>(commandBuffer)->m_commandBuffer;

    vk::ClearColorValue clearColorValue;
    clearColorValue.setFloat32({ 0.5f, 0.5f, 0.5f, 1.0f });

    vk::ClearDepthStencilValue depthClear;
    depthClear.depth = 1.f;

    std::array<vk::ClearValue, 2> clearValues = { clearColorValue, depthClear };

    vk::RenderPassBeginInfo renderPassBeginInfo;
    renderPassBeginInfo.renderPass = vkRenderPass->m_renderPass;
    renderPassBeginInfo.framebuffer = vkRenderPass->m_frameBuffer;
    renderPassBeginInfo.renderArea.offset = vk::Offset2D({ 0, 0 });
    renderPassBeginInfo.renderArea.extent = vkRenderPass->m_extent;
    renderPassBeginInfo.setClearValues(clearValues);

    vkCommandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(vkRenderPass->m_extent.width);
    viewport.height = static_cast<float>(vkRenderPass->m_extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCommandBuffer.setViewport(0, 1, &viewport);

    vk::Rect2D scissor;
    scissor.offset = vk::Offset2D({ 0, 0 });
    scissor.extent = vkRenderPass->m_extent;
    vkCommandBuffer.setScissor(0, 1, &scissor);

    if (m_pipeline)
    {
        vkCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
    }

    //vk::ClearAttachment c;
    //c.clearValue = clearColorValue;
    //c.colorAttachment = 0;
    //c.aspectMask = vk::ImageAspectFlagBits::eColor;

    //vk::Rect2D r;
    //r.extent = vkRenderPass->m_extent;

    //vk::ClearRect cr;
    //cr.rect = r;
    //cr.baseArrayLayer = 0;
    //cr.layerCount = 1;
    //commandBuffer.clearAttachments(c, cr);
}

void VulkanGHI::EndRenderPass(GHIRenderPass* renderPass, GHICommandList* commandBuffer)
{
    vk::CommandBuffer& vkCommandBuffer = static_cast<VulkanCommandList*>(commandBuffer)->m_commandBuffer;

    vkCommandBuffer.endRenderPass();
}

GHICommandList* VulkanGHI::AllocateCommandBuffer()
{
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.commandPool = m_commandPool;
    commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VulkanCommandList* cmd = new VulkanCommandList();
    cmd->m_commandBuffer = m_device->GetLogical().allocateCommandBuffers(commandBufferAllocateInfo)[0];

    return cmd;
}

void VulkanGHI::FreeCommandBuffer(GHICommandList* commandBuffer)
{
    VulkanCommandList* vkCommandBuffer = static_cast<VulkanCommandList*>(commandBuffer);
    m_device->GetLogical().freeCommandBuffers(m_commandPool, vkCommandBuffer->m_commandBuffer);
}

void VulkanGHI::Submit(std::vector<GHICommandList*>& commandLists)
{
    std::vector<vk::CommandBuffer> vkCommandBuffers;
    vkCommandBuffers.reserve(commandLists.size());

    for (GHICommandList* commandBuffer : commandLists)
    {
        vk::CommandBuffer vkCommandBuffer = static_cast<VulkanCommandList*>(commandBuffer)->m_commandBuffer;
        vkCommandBuffers.emplace_back(vkCommandBuffer);
    }

    std::array<vk::Semaphore, 1> waitSemaphores = { m_primarySurface->m_imageAvailableSemaphore };
    std::array<vk::PipelineStageFlags, 1> waitStages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

    //vk::Semaphore signalSemaphores[] = { m_renderFinishedSemaphore };

    vk::SubmitInfo submitInfo;
    submitInfo.setWaitSemaphores(waitSemaphores);
    submitInfo.setWaitDstStageMask(waitStages);
    submitInfo.setCommandBuffers(vkCommandBuffers);

    m_device->GetGraphicsQueue().GetHandle().submit(1, &submitInfo, nullptr);
    m_device->GetGraphicsQueue().GetHandle().waitIdle();
}

GHIBuffer* VulkanGHI::CreateBuffer(EGHIBufferType bufferType, uint size)
{
    vk::BufferCreateInfo bufferInfo;    
    bufferInfo.size = size;

    switch (bufferType)
    {
    case EGHIBufferType::VertexBuffer:
        bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
        break;
    case EGHIBufferType::IndexBuffer:
        bufferInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer;
        break;
    }
    
    // Let the VMA library know that this data should be writeable by CPU, but also readable by GPU
    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    VulkanBuffer* vulkanBuffer = new VulkanBuffer();
    vulkanBuffer->allocator = m_allocator;

    VkBuffer buffer;
    vmaCreateBuffer(m_allocator, &static_cast<VkBufferCreateInfo>(bufferInfo), &vmaallocInfo, &buffer, &vulkanBuffer->allocation, nullptr);
    vulkanBuffer->buffer = buffer;

    return vulkanBuffer;
}

bool VulkanGHI::ImGui_Init()
{
    if (B2D_CHECK(!m_instance) || B2D_CHECK(!m_device))
    {
        return false;
    }

    vk::CommandPoolCreateInfo commandPoolInfo;
    commandPoolInfo.queueFamilyIndex = m_device->GetGraphicsQueue().GetFamilyIndex();
    commandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;

    vk::CommandPool commandPool = m_device->GetLogical().createCommandPool(commandPoolInfo);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    commandBufferAllocateInfo.commandBufferCount = 1;

    vk::CommandBuffer commandBuffer = m_device->GetLogical().allocateCommandBuffers(commandBufferAllocateInfo)[0];

    // Create Descriptor Pool
    {
        std::array<vk::DescriptorPoolSize, 11> poolSizes{
        {
            { vk::DescriptorType::eSampler, 1000 },
            { vk::DescriptorType::eCombinedImageSampler, 1000 },
            { vk::DescriptorType::eSampledImage, 1000 },
            { vk::DescriptorType::eStorageImage, 1000 },
            { vk::DescriptorType::eUniformTexelBuffer, 1000 },
            { vk::DescriptorType::eStorageTexelBuffer, 1000 },
            { vk::DescriptorType::eUniformBuffer, 1000 },
            { vk::DescriptorType::eStorageBuffer, 1000 },
            { vk::DescriptorType::eUniformBufferDynamic, 1000 },
            { vk::DescriptorType::eStorageBufferDynamic, 1000 },
            { vk::DescriptorType::eInputAttachment, 1000 }
        } }; // TODO: Allocate via vma? Is this too much?

        vk::DescriptorPoolCreateInfo poolInfo;
        poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        poolInfo.maxSets = 1000 * poolSizes.size();
        poolInfo.setPoolSizes(poolSizes);

        m_imguiDescriptorpool = m_device->GetLogical().createDescriptorPool(poolInfo);
    }

    vk::RenderPass renderPass;
    {
        vk::AttachmentDescription colorAttachment;
        colorAttachment.format = vk::Format::eB8G8R8A8Unorm; // TMP
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::eTransferSrcOptimal;

        vk::AttachmentReference colorAttachmentRef;
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::SubpassDescription subpass;
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        vk::SubpassDependency subpassDependency;
        subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependency.dstSubpass = 0;
        subpassDependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        subpassDependency.srcAccessMask = vk::AccessFlags(0);
        subpassDependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        subpassDependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

        vk::RenderPassCreateInfo renderPassInfo;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &subpassDependency;

        renderPass = m_device->GetLogical().createRenderPass(renderPassInfo);
    }

    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = m_instance;
    initInfo.PhysicalDevice = m_device->GetPhysical();
    initInfo.Device = m_device->GetLogical();
    initInfo.QueueFamily = m_device->GetGraphicsQueue().GetFamilyIndex();
    initInfo.Queue = m_device->GetGraphicsQueue().GetHandle();
    initInfo.DescriptorPool = m_imguiDescriptorpool;
    initInfo.MinImageCount = 2; // TODO
    initInfo.ImageCount = initInfo.MinImageCount;

    ImGui_ImplVulkan_Init(&initInfo, renderPass);

    vk::CommandBufferBeginInfo begin_info;
    begin_info.flags |= vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    commandBuffer.begin(begin_info);

    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    commandBuffer.end();

    m_device->GetGraphicsQueue().GetHandle().submit(1, &submitInfo, nullptr);
    m_device->GetLogical().waitIdle(); // TODO: Enough to wait for queue?
    //m_device->GetGraphicsQueue().GetHandle().waitIdle();

    m_device->GetLogical().freeCommandBuffers(commandPool, commandBuffer);
    m_device->GetLogical().destroyCommandPool(commandPool);

    ImGui_ImplVulkan_DestroyFontUploadObjects();

    // We can destroy the temporary renderpass at this point but need to make sure to use imgui with an identical render pass later
    m_device->GetLogical().destroyRenderPass(renderPass);

    return true;
}

void VulkanGHI::ImGui_Shutdow()
{
    ImGui_ImplVulkan_Shutdown();

    m_device->GetLogical().destroyDescriptorPool(m_imguiDescriptorpool);
}

void VulkanGHI::ImGui_BeginFrame()
{
    ImGui_ImplVulkan_NewFrame();
}

void VulkanGHI::ImGui_Render(GHICommandList* commandBuffer)
{
    VulkanCommandList* vkCommandBuffer = static_cast<VulkanCommandList*>(commandBuffer);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), vkCommandBuffer->m_commandBuffer);
}
