#include "B2D_pch.h"
#include "ShaderLibrary.h"

#include "GHI/GraphicsHardwareInterface.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

// CreateIntermediateDirectory
// Multithread
// Intermediate Resource

// Add Hash 

bool ShaderLibrary::GetShaderBinaries(EGraphicsAPI const targetApi, ResourcePath const& path, ShaderBinaryData*& outData)
{
    // TODO: Do we need to cache this? The resource should guarantee to only load this once.
    ShaderLibrary::ShaderSpvDataMap& targetShaderSpvDataMap = GetTargetBinaryShaderMap(targetApi);

    auto const foundIt = targetShaderSpvDataMap.find(path);
    if (foundIt != targetShaderSpvDataMap.end())
    {
        outData = &(*foundIt).second.binaryData;
        return true;
    }

    ShaderSpvData const* spvData = nullptr;
    if (!GetOrCompileSpvBinaryShader(path, spvData))
    {
        return false;
    }

    ShaderSpvData targetShaderSpvData;
    targetShaderSpvData.shaderKind = spvData->shaderKind;
    if (!CrossCompile(*spvData, targetApi, targetShaderSpvData.binaryData))
    {
        return false;
    }

    ShaderSpvData& c = targetShaderSpvDataMap[path] = std::move(targetShaderSpvData);
    outData = &c.binaryData;
    return true;
}

ShaderLayout const ShaderLibrary::GetShaderLayout(ShaderBinaryData const& shaderBinaryData)
{
    ShaderLayout shaderInfo;
    Reflect(shaderBinaryData, shaderInfo);
    return shaderInfo;
}

bool ShaderLibrary::GetOrCompileSpvBinaryShader(ResourcePath const& path, ShaderSpvData const*& outSpvData)
{
    auto const foundIt = m_spvBinaryShaderMap.find(path);
    if (foundIt != m_spvBinaryShaderMap.end())
    {
        outSpvData = &(*foundIt).second;
        return true;
    }
    else
    {
        std::string code;
        if (!ReadShaderRawCodeFromFile(path, code))
        {
            return false;
        }

        ShaderSpvData shaderBinaryInfo;

        if (!GetShaderKindFromFilePath(path, shaderBinaryInfo.shaderKind))
        {
            return false;
        }

        if (!CompileSpvShaderBinaries(shaderBinaryInfo.shaderKind, code, shaderBinaryInfo.binaryData))
        {
            return false;
        }

        outSpvData = &(m_spvBinaryShaderMap[path] = std::move(shaderBinaryInfo));
        return true;
    }
}

bool ShaderLibrary::CompileSpvShaderBinaries(shaderc_shader_kind const shaderKind, std::string const& code, ShaderBinaryData& outData)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    //options.SetOptimizationLevel(shaderc_optimization_level_performance); // TODO
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);

    shaderc::SpvCompilationResult const result = compiler.CompileGlslToSpv(code, shaderKind, "", options);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        std::string errorMessage = result.GetErrorMessage();
        if (!errorMessage.empty())
        {
            uint trimNewLine = errorMessage.find_last_not_of('\n');
            if (trimNewLine != std::string::npos)
            {
                errorMessage.resize(trimNewLine + 1);
            }

            B2D_LOG_ERROR("Failed to compile shader [Result: {}]\n{}", result.GetCompilationStatus(), errorMessage);
        }
        else
        {
            B2D_LOG_ERROR("Failed to compile shader [Result: {}]", result.GetCompilationStatus());
        }

        return false;
    }

    outData = std::vector(result.begin(), result.end());

    return true;
}

bool ShaderLibrary::CrossCompile(ShaderSpvData const& spvData, EGraphicsAPI const targetApi, ShaderBinaryData& outData)
{
    if (targetApi == EGraphicsAPI::Vulkan)
    {
        outData = spvData.binaryData;
        return true;
    }
    else if (targetApi == EGraphicsAPI::OpenGL)
    {
        spirv_cross::CompilerGLSL compilerg(spvData.binaryData);
        std::string const openglCode = compilerg.compile();

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
        options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);

        shaderc::SpvCompilationResult const result = compiler.CompileGlslToSpv(openglCode, spvData.shaderKind, "", options);
        if (result.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            std::string errorMessage = result.GetErrorMessage();
            if (!errorMessage.empty())
            {
                uint trimNewLine = errorMessage.find_last_not_of('\n');
                if (trimNewLine != std::string::npos)
                {
                    errorMessage.resize(trimNewLine + 1);
                }

                B2D_LOG_ERROR("Failed to cross compile shader [Result: {}]\n{}", result.GetCompilationStatus(), errorMessage);
            }
            else
            {
                B2D_LOG_ERROR("Failed to compile shader [Result: {}]", result.GetCompilationStatus());
            }

            return false;
        }

        outData = std::vector(result.begin(), result.end());
        return true;
    }
    //else if (targetApi == EGraphicsAPI::DX12)
    //{
    //    spirv_cross::CompilerHLSL compiler(inData);
    //    std::string code = compiler.compile();
 
    //    //D3DCompile
    //}

    return false;
}

void ShaderLibrary::Reflect(ShaderBinaryData const& shaderBinaryData, ShaderLayout& outShaderLayout)
{
    spirv_cross::Compiler compiler(shaderBinaryData);
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();
    
    for (auto const& resource : resources.uniform_buffers)
    {
        auto const& bufferType = compiler.get_type(resource.base_type_id);
        uint bufferSize = compiler.get_declared_struct_size(bufferType);
        uint32 set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
        uint32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
        uint memberCount = bufferType.member_types.size();

        ShaderLayout::UniformBuffer ub;
        ub.set = set;
        ub.binding = binding;
        
        outShaderLayout.uniformBuffers.emplace_back(ub);

        B2D_LOG_INFO("\t  {}", resource.name);
        B2D_LOG_INFO("\t    Size = {}, Set = {}, Binding = {}, Members = {}", bufferSize, set, binding, memberCount);
    }

    for (auto const& resource : resources.push_constant_buffers)
    {
        auto const& bufferType = compiler.get_type(resource.base_type_id);
        uint bufferSize = compiler.get_declared_struct_size(bufferType);
        uint32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
        uint32 offset = compiler.get_decoration(resource.id, spv::DecorationOffset);
        uint memberCount = bufferType.member_types.size();

        ShaderLayout::PushConstantBuffers pcb;
        pcb.offset = offset;
        pcb.size = bufferSize;

        outShaderLayout.pushConstantBuffers.emplace_back(pcb);

        B2D_LOG_INFO("\t  {}", resource.name);
        B2D_LOG_INFO("\t    Size = {}, Offset = {}, Binding = {}, Members = {}", bufferSize, offset, binding, memberCount);
    }

    for (auto const& resource : resources.sampled_images)
    {
        auto const& bufferType = compiler.get_type(resource.base_type_id);
        uint32 set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
        uint32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

        ShaderLayout::Sampler sampler;
        sampler.set = set;
        sampler.binding = binding;

        outShaderLayout.samplers.emplace_back(sampler);

        B2D_LOG_INFO("\t  {}", resource.name);
        B2D_LOG_INFO("\t    Set = {}, Binding = {}", set, binding);
    }
}

void ShaderLibrary::PreloadAll(EGraphicsAPI const targetApi)
{
    B2D_LOG_INFO("Preloading shaders...");

    // TODO: Involve resource manager
    // Add FileManager which is used by the resource manager to map a resource path to the system path (or package path)
    // The file manager would allow the shader libary to access the shader files
    // The file manager should only allow to load data as a resource (so we can listen to data change events)
    std::string const shaderContentPath = "Content/Shader/";
    for (auto const& entry : std::filesystem::recursive_directory_iterator(shaderContentPath))
    {
        std::string path = entry.path().string();

        B2D_LOG_INFO("\t{}", path);

        std::vector<uint32>* data = nullptr;
        GetShaderBinaries(targetApi, path, data);
    }
}

bool ShaderLibrary::ReadShaderRawCodeFromFile(ResourcePath const& path, std::string& outCode)
{
    std::ifstream fileStream;
    fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        fileStream.open(path);

        std::stringstream stringStream;
        stringStream << fileStream.rdbuf();

        fileStream.close();

        outCode = stringStream.str();
        return true;
    }
    catch (std::ifstream::failure e)
    {
        B2D_LOG_ERROR("Shader read error [{}]", path);
    }

    return false;
}

bool ShaderLibrary::GetShaderKindFromFilePath(ResourcePath const& path, shaderc_shader_kind& outShaderKind)
{
    uint32 const shaderTypeSuffixLength = 3;
    std::string const shaderExtensionSuffix = ".glsl";

    uint const offsetLength = shaderExtensionSuffix.size() + shaderTypeSuffixLength;
    if (path.size() >= offsetLength)
    {
        std::string const file = path.substr(path.size() - offsetLength, shaderTypeSuffixLength);

        if (file == ".vs")
        {
            outShaderKind = shaderc_shader_kind::shaderc_glsl_vertex_shader;
            return true;
        }
        else if (file == ".fs")
        {
            outShaderKind = shaderc_shader_kind::shaderc_glsl_fragment_shader;
            return true;
        }
        else if (file == ".cs")
        {
            outShaderKind = shaderc_shader_kind::shaderc_glsl_compute_shader;
            return true;
        }
        else if (file == ".gs")
        {
            outShaderKind = shaderc_shader_kind::shaderc_glsl_geometry_shader;
            return true;
        }
    }

    B2D_LOG_WARNING("Unknown shader type [{}]", path);
    return false;
}

shaderc_shader_kind ShaderLibrary::ShaderTypeToShaderKind(EShaderType const shaderType)
{
    switch (shaderType)
    {
        case EShaderType::Vertex: return shaderc_shader_kind::shaderc_glsl_vertex_shader;
        case EShaderType::Pixel: return shaderc_shader_kind::shaderc_glsl_fragment_shader;
        case EShaderType::Compute: return shaderc_shader_kind::shaderc_glsl_compute_shader;
        case EShaderType::Geometry: return shaderc_shader_kind::shaderc_glsl_geometry_shader;
    }

    B2D_TRAP_f("Unknown shader type [ShaderType: {}]", shaderType);
    return shaderc_shader_kind::shaderc_glsl_infer_from_source;
}

ShaderLibrary::ShaderSpvDataMap& ShaderLibrary::GetTargetBinaryShaderMap(EGraphicsAPI const targetApi)
{
    B2D_STATIC_ASSERT(m_targetBinaryShaderMaps.size() == static_cast<size_t>(EGraphicsAPI::_Size), "m_targetBinaryShaderMaps is not the same size as available graphics api's!");
    return m_targetBinaryShaderMaps[(int)targetApi];
}
