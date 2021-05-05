#pragma once
#include "Graphics/GHI/GHIShader.h"

#include "VulkanCommon.h"

class VulkanShader : public GHIShader
{
public:
    VulkanShader(vk::ShaderModule shaderModule, vk::PipelineShaderStageCreateInfo);

    vk::ShaderModule const& GetShaderModule() { return m_shaderModule; }
    vk::PipelineShaderStageCreateInfo const& GetPipelineInfo() { return m_pipelineInfo; }

private:
    vk::ShaderModule m_shaderModule;
    vk::PipelineShaderStageCreateInfo m_pipelineInfo;

};
