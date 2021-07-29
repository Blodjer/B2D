#include "B2D_pch.h"
#include "VulkanGHI.h"

#include "GameEngine.h"
#include "Platform/GenericWindow.h"

// TMP
#include "Core/Resource.h"
#include "Graphics/Shader.h"
#include "Graphics/Viewport.h"
// TMP

#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "VulkanSurface.h"

#include "Editor/ImGuiCommon.h"
#include "Editor/imgui_impl_vulkan.h"

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

    CreateBasePipeline();

    return true;
}

void VulkanGHI::CreateBasePipeline()
{
    PixelShaderRef ps = IResourceManager::Get<PixelShader>("Content/Shader/Vulkan.fs.glsl");
    VertexShaderRef vs = IResourceManager::Get<VertexShader>("Content/Shader/Vulkan.vs.glsl");

    vk::PipelineVertexInputStateCreateInfo vertexInputCreateInfo;
    vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
    vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
    vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;

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

    std::array<vk::DynamicState, 2> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    // RenderPass
    //      Layout (Contains render targets)
    //          

    vk::PipelineDynamicStateCreateInfo dynamicState;
    dynamicState.dynamicStateCount = dynamicStates.size();
    dynamicState.pDynamicStates = dynamicStates.data();

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

    m_renderPass = m_device->GetLogical().createRenderPass(renderPassInfo);

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    m_pipelineLayout = m_device->GetLogical().createPipelineLayout(pipelineLayoutInfo);

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
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlendState;
        pipelineInfo.pDynamicState = &dynamicState; // Optional

        pipelineInfo.layout = m_pipelineLayout;
        pipelineInfo.renderPass = m_renderPass;
        pipelineInfo.subpass = 0;

        pipelineInfo.basePipelineHandle = nullptr; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        m_pipeline = m_device->GetLogical().createGraphicsPipeline(nullptr, pipelineInfo).value;
    }

    //m_framebuffers.reserve(swapchainImageViews.size());
    m_framebuffers.reserve(1);

    std::vector<vk::Image> images;

    m_extent.width = 1600;
    m_extent.height = 900;

    vk::Extent3D ex;
    ex.width = m_extent.width;
    ex.height = m_extent.height;
    ex.depth = 1;

    vk::ImageCreateInfo imageCreateInfo;
    imageCreateInfo.extent = ex;
    imageCreateInfo.imageType = vk::ImageType::e2D;
    imageCreateInfo.format = colorAttachment.format;
    imageCreateInfo.usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    //imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    //imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    images.emplace_back(m_device->GetLogical().createImage(imageCreateInfo));
    m_targetImage = images[0];

    vk::MemoryAllocateInfo memAlloc;
    vk::MemoryRequirements memReqs;

    m_device->GetLogical().getImageMemoryRequirements(images[0], &memReqs);

    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = 0;
    vk::DeviceMemory dMemory = m_device->GetLogical().allocateMemory(memAlloc);
    m_device->GetLogical().bindImageMemory(images[0], dMemory, 0);
    //m_device->GetPhysical().getMemoryProperties() :;

    std::vector<vk::ImageView> imageViews;
    imageViews.reserve(1);
    {
        for (vk::Image const& image : images)
        {
            vk::ImageViewCreateInfo createInfo;
            createInfo.image = image;
            createInfo.viewType = vk::ImageViewType::e2D;
            createInfo.format = colorAttachment.format;
            createInfo.components.r = vk::ComponentSwizzle::eIdentity;
            createInfo.components.g = vk::ComponentSwizzle::eIdentity;
            createInfo.components.b = vk::ComponentSwizzle::eIdentity;
            createInfo.components.a = vk::ComponentSwizzle::eIdentity;
            createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            imageViews.emplace_back(m_device->GetLogical().createImageView(createInfo));
        }
    }

    // UE4: Set the format from outside. Check VulkanViewport.h:116
    for (vk::ImageView& imageView : imageViews)
    {
        vk::FramebufferCreateInfo framebufferInfo;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = &imageView;
        framebufferInfo.width = m_extent.width;
        framebufferInfo.height = m_extent.height;
        framebufferInfo.layers = 1;

        m_framebuffers.emplace_back(m_device->GetLogical().createFramebuffer(framebufferInfo));
    }

    vk::CommandPoolCreateInfo commandPoolInfo;
    commandPoolInfo.queueFamilyIndex = m_device->GetGraphicsQueue().GetFamilyIndex();
    commandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer; // Optional

    vk::CommandPool commandPool = m_device->GetLogical().createCommandPool(commandPoolInfo);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    commandBufferAllocateInfo.commandBufferCount = static_cast<uint32>(m_framebuffers.size());

    m_commandBuffers = m_device->GetLogical().allocateCommandBuffers(commandBufferAllocateInfo);

    vk::SemaphoreCreateInfo semaphoreInfo;
    m_renderFinishedSemaphore = m_device->GetLogical().createSemaphore(semaphoreInfo);
}

void VulkanGHI::BeginRenderPass()
{
    vk::CommandBufferBeginInfo commandBufferBeginInfo;
    commandBufferBeginInfo.flags = vk::CommandBufferUsageFlags(0);
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    vk::ClearColorValue clearColorValue;
    clearColorValue.setFloat32({ 0.5f, 0.5f, 0.5f, 1.0f });
    vk::ClearValue clearColor(clearColorValue);

    vk::RenderPassBeginInfo renderPassBeginInfo;
    renderPassBeginInfo.renderPass = m_renderPass;
    //renderPassBeginInfo.framebuffer = m_framebuffers[m_currentImageIndex];
    renderPassBeginInfo.framebuffer = m_framebuffers[0];
    renderPassBeginInfo.renderArea.offset = vk::Offset2D({ 0, 0 });
    renderPassBeginInfo.renderArea.extent = m_extent;
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;

    //vk::CommandBuffer& commandBuffer = m_commandBuffers[m_currentImageIndex];
    vk::CommandBuffer& commandBuffer = m_commandBuffers[0];

    commandBuffer.begin(commandBufferBeginInfo);

    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    CViewport* v = GameEngine::Instance()->GetMainWindow()->GetViewport();

    m_extent.width = v->GetWidth();
    m_extent.height = v->GetHeight();

    vk::Viewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_extent.width);
    viewport.height = static_cast<float>(m_extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    commandBuffer.setViewport(0, 1, &viewport);

    vk::Rect2D scissor;
    scissor.offset = vk::Offset2D({ 0, 0 });
    scissor.extent = m_extent;
    commandBuffer.setScissor(0, 1, &scissor);

    if (m_pipeline)
    {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline);
        commandBuffer.draw(3, 1, 0, 0);
    }
}

void VulkanGHI::EndRenderPass()
{
    //vk::CommandBuffer& commandBuffer = m_commandBuffers[m_currentImageIndex];
    vk::CommandBuffer& commandBuffer = m_commandBuffers[0];

    commandBuffer.endRenderPass();

    commandBuffer.end();

    std::array<vk::Semaphore, 1> waitSemaphores = { m_primarySurface->m_imageAvailableSemaphore };
    std::array<vk::PipelineStageFlags, 1> waitStages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

    vk::Semaphore signalSemaphores[] = { m_renderFinishedSemaphore };

    vk::SubmitInfo submitInfo;
    submitInfo.setWaitSemaphores(waitSemaphores);
    submitInfo.setWaitDstStageMask(waitStages);
    submitInfo.setCommandBuffers(m_commandBuffers[0]);

    m_device->GetGraphicsQueue().GetHandle().submit(1, &submitInfo, nullptr);
    m_device->GetGraphicsQueue().GetHandle().waitIdle();
}

void VulkanGHI::Shutdown()
{
    m_device->GetLogical().waitIdle();

    delete m_device;
    m_device = nullptr;

    B2D_LOG_WARNING("TODO: Destroy Command Pool");
    B2D_LOG_WARNING("TODO: Destroy Framebuffers");
    B2D_LOG_WARNING("TODO: Destroy Surfaces");

    m_device->GetLogical().destroySemaphore(m_renderFinishedSemaphore);

    m_device->GetLogical().destroyPipeline(m_pipeline);
    m_device->GetLogical().destroyPipelineLayout(m_pipelineLayout);
    m_device->GetLogical().destroyRenderPass(m_renderPass);

    m_instance.destroyDebugUtilsMessengerEXT(m_debugUtilMessenger);
    m_debugUtilMessenger = nullptr;

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

void VulkanGHI::Clear(bool color, bool depth, bool stencil)
{
    B2D_NOT_IMPLEMENTED();
}

GHITexture* VulkanGHI::CreateTexture(void const* data, uint32 width, uint32 height, uint8 components)
{
    B2D_NOT_IMPLEMENTED();
}

void VulkanGHI::BindTexture(GHITexture const* texture)
{
    B2D_NOT_IMPLEMENTED();
}

void VulkanGHI::FreeTexture(GHITexture*& texture)
{
    B2D_NOT_IMPLEMENTED();
}

GHISurface* VulkanGHI::CreateSurface(void* nativeWindowHandle, uint32 width, uint32 height)
{
    return m_primarySurface = new VulkanSurface(m_instance, *m_device, nativeWindowHandle);
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

void VulkanGHI::DeleteShader(GHIShader*& ghiShader)
{
    VulkanShader* shader = static_cast<VulkanShader*>(ghiShader);
    m_device->GetLogical().destroyShaderModule(shader->GetShaderModule());

    delete shader;
    ghiShader = nullptr;
}

GHIMaterial* VulkanGHI::CreateMaterial(GHIShader* vertexShader, GHIShader* pixelShader)
{
    B2D_NOT_IMPLEMENTED();
}

void VulkanGHI::FreeMaterial(GHIMaterial*& material)
{
    B2D_NOT_IMPLEMENTED();
}

void VulkanGHI::BindMaterial(GHIMaterial* material)
{
    B2D_NOT_IMPLEMENTED();
}

GHIRenderTarget* VulkanGHI::CreateRenderTarget()
{
    B2D_NOT_IMPLEMENTED();
}

GHIRenderTarget* VulkanGHI::CreateRenderTarget(GHITexture* texture)
{
    B2D_NOT_IMPLEMENTED();
}

void VulkanGHI::ResizeRenderTarget(GHIRenderTarget*& renderTarget, uint32 width, uint32 height)
{
    B2D_NOT_IMPLEMENTED();
}

void VulkanGHI::DeleteRenderTarget(GHIRenderTarget*& renderTarget, bool freeTexture)
{
    B2D_NOT_IMPLEMENTED();
}

void VulkanGHI::BindRenderTarget(GHIRenderTarget* renderTarget)
{
    B2D_NOT_IMPLEMENTED();
}

void VulkanGHI::BindRenderTargetAndClear(GHIRenderTarget* renderTarget)
{
    B2D_NOT_IMPLEMENTED();
}

bool VulkanGHI::ImGui_Init()
{
    if (B2D_CHECK(!m_instance))
    {
        return false;
    }

    VkDescriptorPool pool;

    VkDescriptorPoolSize poolSizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };
    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);

    vk::PhysicalDeviceProperties pp = m_device->GetPhysical().getProperties();
    vk::DeviceSize limits = pp.limits.nonCoherentAtomSize;

    poolInfo.poolSizeCount = (uint32_t)IM_ARRAYSIZE(poolSizes);
    poolInfo.pPoolSizes = poolSizes;
    VkResult err = vkCreateDescriptorPool(m_device->GetLogical(), &poolInfo, nullptr, &pool);

    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = m_instance;
    initInfo.PhysicalDevice = m_device->GetPhysical();
    initInfo.Device = m_device->GetLogical();
    initInfo.QueueFamily = m_device->GetGraphicsQueue().GetFamilyIndex();
    initInfo.Queue = m_device->GetGraphicsQueue().GetHandle();
    initInfo.DescriptorPool = pool;
    initInfo.MinImageCount = 2; // TODO
    initInfo.ImageCount = initInfo.MinImageCount;

    ImGui_ImplVulkan_Init(&initInfo, m_renderPass);

    //vk::CommandPool commandPool = m_commandBuffers[0];
    vk::CommandBuffer commandBuffer = m_commandBuffers[0];

    commandBuffer.reset();

    //m_device.resetCommandPool(commandPool);

    vk::CommandBufferBeginInfo begin_info;
    begin_info.flags |= vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    commandBuffer.begin(begin_info);

    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    commandBuffer.end();

    m_device->GetGraphicsQueue().GetHandle().submit(1, &submitInfo, nullptr);

    m_device->GetLogical().waitIdle();

    return true;
}

void VulkanGHI::ImGui_Shutdow()
{
    ImGui_ImplVulkan_Shutdown();
}

void VulkanGHI::ImGui_BeginFrame()
{
    ImGui_ImplVulkan_NewFrame();
}

void VulkanGHI::ImGui_Render()
{
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_commandBuffers[0]);
}
