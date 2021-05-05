#include "B2D_pch.h"
#include "VulkanShader.h"

VulkanShader::VulkanShader(vk::ShaderModule shaderModule, vk::PipelineShaderStageCreateInfo pipelineInfo)
    : m_shaderModule(shaderModule)
    , m_pipelineInfo(pipelineInfo)
{

}
