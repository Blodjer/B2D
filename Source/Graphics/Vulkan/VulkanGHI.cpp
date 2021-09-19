#include "B2D_pch.h"
#include "VulkanGHI.h"

#include "GameEngine.h"

#include "Graphics/Mesh.h" // TMP

#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "VulkanSurface.h"
#include "VulkanRenderPass.h"
#include "VulkanTexture.h"
#include "VulkanCommandBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanResourceSet.h"
#include "VulkanSampler.h"

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
    allocatorInfo.vulkanApiVersion = appInfo.apiVersion;
    allocatorInfo.physicalDevice = m_device->GetPhysical();
    allocatorInfo.device = m_device->GetLogical();
    allocatorInfo.instance = m_instance;
    vmaCreateAllocator(&allocatorInfo, &m_allocator);

    {
        vk::CommandPoolCreateInfo commandPoolInfo;
        commandPoolInfo.queueFamilyIndex = m_device->GetGraphicsQueue().GetFamilyIndex();
        commandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

        m_commandPool = m_device->GetLogical().createCommandPool(commandPoolInfo);
    }

    {
        vk::CommandPoolCreateInfo commandPoolInfo;
        commandPoolInfo.queueFamilyIndex = m_device->GetGraphicsQueue().GetFamilyIndex();
        commandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;

        m_immediateCommandPool = m_device->GetLogical().createCommandPool(commandPoolInfo);
    }

    return true;
}

void VulkanGHI::Shutdown()
{
    m_device->GetLogical().waitIdle();

    UpdateCommandBufferStatus();

    B2D_ASSERT(m_pendingCommandBuffer.empty(), "There should be no command buffer left. UpdateCommandBufferStatus() should have moved all buffer to m_availableCommandBuffers!");

    for (VulkanCommandBuffer* commandBuffer : m_availableCommandBuffers)
    {
        DestroyCommandBuffer(commandBuffer);
    }
    m_availableCommandBuffers.clear();

    m_device->GetLogical().destroyCommandPool(m_commandPool);
    m_device->GetLogical().destroyCommandPool(m_immediateCommandPool);

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
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME, // TODO: Disable in non debug mode

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
#ifndef B2D_BUILD_RELEASE // TODO: Add vulkan preprocessor macro
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
    return new VulkanSurface(m_instance, *m_device, nativeWindowHandle);
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

GHIGraphicsPipeline* VulkanGHI::CreateGraphicsPipeline(GHIRenderPass const* targetPass, VertexShaderRef vertexShader, PixelShaderRef pixelShader)
{
    VulkanRenderPass const* vkRenderPass = static_cast<VulkanRenderPass const*>(targetPass);

    VulkanShader* vkVertexShader = static_cast<VulkanShader*>(vertexShader->GetGHIShader());
    VulkanShader* vkPixelShader = static_cast<VulkanShader*>(pixelShader->GetGHIShader());

    ShaderLayout const& vertexShaderLayout = vertexShader->GetLayout();
    ShaderLayout const& PixelShaderLayout = pixelShader->GetLayout();

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    shaderStages.emplace_back(vkPixelShader->GetPipelineInfo());
    shaderStages.emplace_back(vkVertexShader->GetPipelineInfo());

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

        // UV will be stored at Location 2
        vk::VertexInputAttributeDescription uvAttribute;
        uvAttribute.binding = 0;
        uvAttribute.location = 2;
        uvAttribute.format = vk::Format::eR32G32Sfloat;
        uvAttribute.offset = offsetof(Mesh::Vertex, uv);

        description.attributes.push_back(positionAttribute);
        description.attributes.push_back(normalAttribute);
        description.attributes.push_back(uvAttribute);
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

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
    std::vector<vk::DescriptorSetLayout> setLayouts;
    {
        std::vector<vk::PushConstantRange> combinedPushConstants;
        std::vector<std::unordered_map<uint32, vk::DescriptorSetLayoutBinding>> combinedDescriptorSets;

        auto CombineStageLayouts = [&combinedDescriptorSets, &combinedPushConstants](ShaderLayout const& shaderLayout, vk::ShaderStageFlagBits const shaderStage) {
            for (auto const& ub : shaderLayout.uniformBuffers)
            {
                if (combinedDescriptorSets.size() <= ub.set)
                {
                    combinedDescriptorSets.resize(ub.set + 1);
                }

                auto& set = combinedDescriptorSets[ub.set];

                auto it = set.find(ub.binding);
                if (it == set.end())
                {
                    vk::DescriptorSetLayoutBinding setLayoutBinding;
                    setLayoutBinding.binding = ub.binding;
                    setLayoutBinding.descriptorCount = 1;
                    setLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
                    setLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eAll; // TMP //shaderStage;
                    set.insert({ ub.binding, setLayoutBinding });
                }
                else
                {
                    (*it).second.stageFlags |= shaderStage;
                }
            }

            for (auto const& s : shaderLayout.samplers)
            {
                if (combinedDescriptorSets.size() <= s.set)
                {
                    combinedDescriptorSets.resize(s.set + 1);
                }

                auto& set = combinedDescriptorSets[s.set];

                auto it = set.find(s.binding);
                if (it == set.end())
                {
                    vk::DescriptorSetLayoutBinding setLayoutBinding;
                    setLayoutBinding.binding = s.binding;
                    setLayoutBinding.descriptorCount = 1;
                    setLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
                    setLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eAll; // TMP //shaderStage;
                    set.insert({ s.binding, setLayoutBinding });
                }
                else
                {
                    (*it).second.stageFlags |= shaderStage;
                }
            }

            // TODO: Needs to be combined? (https://stackoverflow.com/questions/37056159/using-different-push-constants-in-different-shader-stages)
            for (auto const& pcb : shaderLayout.pushConstantBuffers)
            {
                vk::PushConstantRange pcr;
                pcr.offset = pcb.offset;
                pcr.size = pcb.size;
                pcr.stageFlags = shaderStage;
                combinedPushConstants.emplace_back(pcr);
            }
        };

        CombineStageLayouts(vertexShaderLayout, vk::ShaderStageFlagBits::eVertex);
        CombineStageLayouts(PixelShaderLayout, vk::ShaderStageFlagBits::eFragment);

        for (auto& set : combinedDescriptorSets)
        {
            std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings;
            for (auto bindings : set)
            {
                setLayoutBindings.emplace_back(bindings.second);
            }

            vk::DescriptorSetLayoutCreateInfo setLayoutInfo;
            setLayoutInfo.setBindings(setLayoutBindings);
            setLayoutInfo.flags = vk::DescriptorSetLayoutCreateFlags(0);

            vk::DescriptorSetLayout setLayout = m_device->GetLogical().createDescriptorSetLayout(setLayoutInfo);
            setLayouts.emplace_back(setLayout);
        }

        pipelineLayoutInfo.setSetLayouts(setLayouts);
        pipelineLayoutInfo.setPushConstantRanges(combinedPushConstants);
    }
    vk::PipelineLayout pipelineLayout = m_device->GetLogical().createPipelineLayout(pipelineLayoutInfo);

    std::array<vk::DynamicState, 2> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    vk::PipelineDynamicStateCreateInfo dynamicState;
    dynamicState.setDynamicStates(dynamicStates);

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

    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = vkRenderPass->m_renderPass;
    pipelineInfo.subpass = 0;

    VulkanGraphicsPipeline* gp = new VulkanGraphicsPipeline();
    gp->m_pipelineLayout = pipelineLayout;
    gp->m_pipeline = m_device->GetLogical().createGraphicsPipeline(nullptr, pipelineInfo).value;

    return gp;
}

void VulkanGHI::DestroyGraphicsPipeline(GHIGraphicsPipeline* pipeline)
{
    VulkanGraphicsPipeline* vkPipeline = static_cast<VulkanGraphicsPipeline*>(pipeline);
    m_device->GetLogical().destroyPipeline(vkPipeline->m_pipeline);
    m_device->GetLogical().destroyPipelineLayout(vkPipeline->m_pipelineLayout);
    delete vkPipeline;
}

GHIResourceSet* VulkanGHI::CreateResourceSet()
{
    VulkanResourceSet* resourceSet = new VulkanResourceSet();
    resourceSet->m_device = m_device;
    return resourceSet;
}

void VulkanGHI::DestroyResourceSet(GHIResourceSet* resourceSet)
{
    delete resourceSet;
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
    imageCreateInfo.format = Convert(format);
    imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferSrc;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;

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

GHITexture* VulkanGHI::CreateTexture(void const* data, uint32 width, uint32 height, EGHITextureFormat format)
{
    vk::Extent3D extent;
    extent.width = width;
    extent.height = height;
    extent.depth = 1;

    vk::Format vkFormat = Convert(format);
    uint bytesPerPixel = GetBytesPerPixel(format);

    vk::FormatProperties formatProperties = m_device->GetPhysical().getFormatProperties(vkFormat);
    vk::FormatFeatureFlags featureFlags = vk::FormatFeatureFlagBits::eSampledImage | vk::FormatFeatureFlagBits::eTransferDst;
    if ((formatProperties.optimalTilingFeatures & featureFlags) != featureFlags)
    {
        B2D_BREAK_f("Fomat not supported!");
        return nullptr;
    }

    vk::ImageCreateInfo imageCreateInfo;
    imageCreateInfo.extent = extent;
    imageCreateInfo.imageType = vk::ImageType::e2D;
    imageCreateInfo.format = vkFormat;
    imageCreateInfo.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
    imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;

    uint imageSize = width * height * bytesPerPixel;
    VulkanBuffer* stagingBuffer = CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY);
    stagingBuffer->Upload(data, imageSize);

    VulkanTexture* texture = new VulkanTexture(width, height);

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocationCreateInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkImage outImage;
    vmaCreateImage(m_allocator, &static_cast<VkImageCreateInfo>(imageCreateInfo), &allocationCreateInfo, &outImage, &texture->m_allocation, nullptr);

    ImmediateSubmit([&](VulkanCommandBuffer& cmd)
        {
            vk::ImageSubresourceRange range;
            range.aspectMask = vk::ImageAspectFlagBits::eColor;
            range.baseMipLevel = 0;
            range.levelCount = 1;
            range.baseArrayLayer = 0;
            range.layerCount = 1;

            vk::ImageMemoryBarrier imageBarrier_toTransfer;
            imageBarrier_toTransfer.image = outImage;
            imageBarrier_toTransfer.subresourceRange = range;
            imageBarrier_toTransfer.oldLayout = vk::ImageLayout::eUndefined;
            imageBarrier_toTransfer.newLayout = vk::ImageLayout::eTransferDstOptimal;
            imageBarrier_toTransfer.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
            imageBarrier_toTransfer.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            cmd.m_commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags(0), nullptr, nullptr, imageBarrier_toTransfer);

            vk::BufferImageCopy copyRegion;
            copyRegion.bufferOffset = 0;
            copyRegion.bufferRowLength = 0;
            copyRegion.bufferImageHeight = 0;
            copyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
            copyRegion.imageSubresource.mipLevel = 0;
            copyRegion.imageSubresource.baseArrayLayer = 0;
            copyRegion.imageSubresource.layerCount = 1;
            copyRegion.imageExtent = extent;

            cmd.m_commandBuffer.copyBufferToImage(stagingBuffer->buffer, outImage, vk::ImageLayout::eTransferDstOptimal, copyRegion);

            vk::ImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;
            imageBarrier_toReadable.oldLayout = vk::ImageLayout::eTransferDstOptimal;
            imageBarrier_toReadable.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            imageBarrier_toReadable.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            imageBarrier_toReadable.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            cmd.m_commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlags(0), nullptr, nullptr, imageBarrier_toReadable);
        });

    DestroyBuffer(stagingBuffer);

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
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;

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

GHISampler* VulkanGHI::CreateSampler()
{
    VulkanSampler* sampler = new VulkanSampler();

    vk::SamplerCreateInfo info;
    info.magFilter = vk::Filter::eLinear;
    info.minFilter = vk::Filter::eLinear;
    info.addressModeU = vk::SamplerAddressMode::eRepeat;
    info.addressModeV = vk::SamplerAddressMode::eRepeat;
    info.addressModeW = vk::SamplerAddressMode::eRepeat;

    sampler->sampler = m_device->GetLogical().createSampler(info);

    return sampler;
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

        vk::AttachmentDescription& colorAttachment = attachmentDescriptions[i];
        colorAttachment.format = vkRenderTarget->m_format;
        colorAttachment.loadOp = depthTarget != nullptr ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad;
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = depthTarget != nullptr ? vk::ImageLayout::eUndefined : vk::ImageLayout::eTransferSrcOptimal;
        colorAttachment.finalLayout = vk::ImageLayout::eTransferSrcOptimal;

        vk::AttachmentReference& colorAttachmentRef = colorAttachmentRefs[i];
        colorAttachmentRef.attachment = i;
        colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;
    }
    subpass.setColorAttachments(colorAttachmentRefs);

    vk::AttachmentReference depthAttachmentRef;
    if (depthTarget != nullptr)
    {
        VulkanTexture const* vkDepthTarget = static_cast<VulkanTexture const*>(depthTarget);

        vk::AttachmentDescription depthAttachment;
        depthAttachment.format = vkDepthTarget->m_format;
        depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachment.storeOp = vk::AttachmentStoreOp::eStore;
        depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eClear;
        depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
        depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        depthAttachmentRef.attachment = attachmentDescriptions.size();
        depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        attachmentDescriptions.emplace_back(depthAttachment);
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

    return renderPass;
}

void VulkanGHI::DestroyRenderPass(GHIRenderPass* renderPass)
{
    VulkanRenderPass* vkRenderPass = static_cast<VulkanRenderPass*>(renderPass);
    m_device->GetLogical().destroyRenderPass(vkRenderPass->m_renderPass);
    m_device->GetLogical().destroyFramebuffer(vkRenderPass->m_frameBuffer);
    delete vkRenderPass;
}

void VulkanGHI::BeginRenderPass(GHIRenderPass* renderPass, GHICommandBuffer* commandBuffer)
{
    VulkanRenderPass* vkRenderPass = static_cast<VulkanRenderPass*>(renderPass);
    vk::CommandBuffer& vkCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer)->m_commandBuffer;

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
}

void VulkanGHI::EndRenderPass(GHIRenderPass* renderPass, GHICommandBuffer* commandBuffer)
{
    vk::CommandBuffer& vkCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer)->m_commandBuffer;

    vkCommandBuffer.endRenderPass();
}

GHICommandBuffer* VulkanGHI::AllocateCommandBuffer()
{
    UpdateCommandBufferStatus();

    if (!m_availableCommandBuffers.empty())
    {
        VulkanCommandBuffer* commandBuffer = m_availableCommandBuffers.back();
        m_availableCommandBuffers.pop_back();
        return commandBuffer;
    }

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.commandPool = m_commandPool;
    commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VulkanCommandBuffer* commandBuffer = new VulkanCommandBuffer();
    commandBuffer->m_commandBuffer = m_device->GetLogical().allocateCommandBuffers(commandBufferAllocateInfo)[0];

    return commandBuffer;
}

void VulkanGHI::FreeCommandBuffer(GHICommandBuffer* commandBuffer)
{
    VulkanCommandBuffer* vkCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);
    if (vkCommandBuffer->m_isSubmitted)
    {
        m_pendingCommandBuffer.emplace_back(vkCommandBuffer);
    }
    else
    {
        m_availableCommandBuffers.emplace_back(vkCommandBuffer);
    }
}

void VulkanGHI::WaitAndResetCommandBuffer(VulkanCommandBuffer* commandBuffer)
{
    m_device->GetLogical().waitForFences(commandBuffer->m_completionFence, true, UINT64_MAX);
    m_device->GetLogical().destroyFence(commandBuffer->m_completionFence);
    commandBuffer->m_completionFence = nullptr;
    
    commandBuffer->m_commandBuffer.reset();
    commandBuffer->m_isSubmitted = false;
}

void VulkanGHI::DestroyCommandBuffer(VulkanCommandBuffer* commandBuffer)
{
    m_device->GetLogical().freeCommandBuffers(m_commandPool, commandBuffer->m_commandBuffer);

    if (commandBuffer->m_completionFence)
    {
        // TODO: Fences can be use by multiple command buffers. This fence might have been already destroyed.
        //m_device->GetLogical().destroyFence(commandBuffer->m_completionFence);
    }

    delete commandBuffer;
}

void VulkanGHI::UpdateCommandBufferStatus()
{
    std::unordered_map<vk::Fence, bool> fenceStatusMap;
    for (auto it = m_pendingCommandBuffer.begin(); it != m_pendingCommandBuffer.end();)
    {
        VulkanCommandBuffer* const commandBuffer = *it;
        
        auto [its, isNew] = fenceStatusMap.emplace(commandBuffer->m_completionFence, false);
        if (isNew)
        {
            bool completed = m_device->GetLogical().getFenceStatus(commandBuffer->m_completionFence) == vk::Result::eSuccess;
            its->second = completed;

            if (completed)
            {
                m_device->GetLogical().destroyFence(commandBuffer->m_completionFence);
                commandBuffer->m_completionFence = nullptr;
            }
        }

        if (its->second)
        {
            commandBuffer->m_commandBuffer.reset();
            commandBuffer->m_isSubmitted = false;
            commandBuffer->m_completionFence = nullptr;

            m_availableCommandBuffers.emplace_back(commandBuffer);
            it = m_pendingCommandBuffer.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void VulkanGHI::Submit(std::vector<GHICommandBuffer*>& commandBuffers)
{
    Submit(commandBuffers, nullptr, nullptr, nullptr);
}

void VulkanGHI::Submit(std::vector<GHICommandBuffer*>& commandBuffers, vk::ArrayProxyNoTemporaries<vk::Semaphore const> const& waitSemaphores, vk::ArrayProxyNoTemporaries<vk::PipelineStageFlags const> const& waitStages, vk::ArrayProxyNoTemporaries<vk::Semaphore const> const& signalSemaphores)
{
    vk::FenceCreateInfo fenceCreateInfo;
    vk::Fence fence = m_device->GetLogical().createFence(fenceCreateInfo); // TODO: Reuse

    std::vector<vk::CommandBuffer> vkCommandBuffers;
    vkCommandBuffers.reserve(commandBuffers.size());

    for (GHICommandBuffer* commandBuffer : commandBuffers)
    {
        VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);
        vulkanCommandBuffer->m_completionFence = fence;
        vulkanCommandBuffer->m_isSubmitted = true;

        vkCommandBuffers.emplace_back(vulkanCommandBuffer->m_commandBuffer);
    }

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(vkCommandBuffers);
    if (!waitSemaphores.empty())
    {
        submitInfo.setWaitSemaphores(waitSemaphores);
    }
    if (!waitStages.empty())
    {
        submitInfo.setWaitDstStageMask(waitStages);
    }
    if (!signalSemaphores.empty())
    {
        submitInfo.setSignalSemaphores(signalSemaphores);
    }

    m_device->GetGraphicsQueue().GetHandle().submit({ submitInfo }, fence);
}

void VulkanGHI::ImmediateSubmit(std::function<void(VulkanCommandBuffer&)>&& function)
{
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.commandPool = m_immediateCommandPool;
    commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VulkanCommandBuffer* cmd = new VulkanCommandBuffer();
    cmd->m_commandBuffer = m_device->GetLogical().allocateCommandBuffers(commandBufferAllocateInfo)[0];

    cmd->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    function(*cmd);

    cmd->End();

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(cmd->m_commandBuffer);

    m_device->GetGraphicsQueue().GetHandle().submit(1, &submitInfo, nullptr);
    m_device->GetGraphicsQueue().GetHandle().waitIdle();

    m_device->GetLogical().resetCommandPool(m_immediateCommandPool);
}

GHIBuffer* VulkanGHI::CreateBuffer(EGHIBufferType bufferType, uint size)
{
    vk::BufferUsageFlags vkBufferType;

    switch (bufferType)
    {
        case EGHIBufferType::VertexBuffer:
            vkBufferType = vk::BufferUsageFlagBits::eVertexBuffer;
            break;
        case EGHIBufferType::IndexBuffer:
            vkBufferType = vk::BufferUsageFlagBits::eIndexBuffer;
            break;
        case EGHIBufferType::UniformBuffer:
            vkBufferType = vk::BufferUsageFlagBits::eUniformBuffer;
            break;
        default:
            B2D_BREAK_f("Buffer type not supported");
            return nullptr;
    }

    return CreateBuffer(size, vkBufferType, VMA_MEMORY_USAGE_CPU_TO_GPU);
}

VulkanBuffer* VulkanGHI::CreateBuffer(uint size, vk::BufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage)
{
    vk::BufferCreateInfo bufferInfo;
    bufferInfo.usage = bufferUsage;
    bufferInfo.size = size;

    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = memoryUsage;

    VulkanBuffer* vulkanBuffer = new VulkanBuffer();

    VkBuffer buffer;
    vmaCreateBuffer(m_allocator, &static_cast<VkBufferCreateInfo>(bufferInfo), &vmaallocInfo, &buffer, &vulkanBuffer->allocation, nullptr);

    vulkanBuffer->allocator = m_allocator;
    vulkanBuffer->buffer = buffer;

    return vulkanBuffer;
}

void VulkanGHI::DestroyBuffer(GHIBuffer* buffer)
{
    VulkanBuffer* vkBuffer = static_cast<VulkanBuffer*>(buffer);
    vmaDestroyBuffer(vkBuffer->allocator, vkBuffer->buffer, vkBuffer->allocation);
    delete vkBuffer;
}

vk::Format VulkanGHI::Convert(EGHITextureFormat textureFormat)
{
    switch (textureFormat)
    {
        case EGHITextureFormat::RGBA8:              return vk::Format::eR8G8B8A8Unorm;
        case EGHITextureFormat::BGRA8:              return vk::Format::eB8G8R8A8Unorm;
        case EGHITextureFormat::Depth24:            return vk::Format::eD24UnormS8Uint;
        case EGHITextureFormat::Depth24Stencil8:    return vk::Format::eD24UnormS8Uint;

        default:
            B2D_TRAP_f("Format not supported by Vulkan!");
    }

    return vk::Format::eUndefined;
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
    m_device->GetGraphicsQueue().GetHandle().waitIdle();

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

void VulkanGHI::ImGui_Render(GHICommandBuffer* commandBuffer)
{
    VulkanCommandBuffer* vkCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), vkCommandBuffer->m_commandBuffer);
}
