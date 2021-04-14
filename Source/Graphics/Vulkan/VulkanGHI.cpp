#include "B2D_pch.h"
#include "VulkanGHI.h"

#include <vulkan/vulkan.hpp>

#include <imgui/imgui.h>
#include <Editor/imgui_impl_vulkan.h>

bool VulkanGHI::Init()
{
    uint32 propertyCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr);

    B2D_LOG_INFO("Initialize Vulkan...");
    B2D_LOG_INFO("Vulkan version    {}.{}.{}",
        VK_VERSION_MAJOR(VK_HEADER_VERSION_COMPLETE),
        VK_VERSION_MINOR(VK_HEADER_VERSION_COMPLETE),
        VK_VERSION_PATCH(VK_HEADER_VERSION_COMPLETE)
    );

    return true;
}

void VulkanGHI::Shutdown()
{
    B2D_NOT_IMPLEMENTED();
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

GHIShader* VulkanGHI::CreateVertexShader(char const* code)
{
    B2D_NOT_IMPLEMENTED();
}

GHIShader* VulkanGHI::CreatePixelShader(char const* code)
{
    B2D_NOT_IMPLEMENTED();
}

void VulkanGHI::DeleteShader(GHIShader*& shader)
{
    B2D_NOT_IMPLEMENTED();
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
    B2D_NOT_IMPLEMENTED();
    return false;
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
    B2D_NOT_IMPLEMENTED();
    //ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());
}
