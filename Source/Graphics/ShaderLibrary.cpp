#include "B2D_pch.h"
#include "ShaderLibrary.h"

#include "GameEngine.h"
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

bool ShaderLibrary::GetShaderBinaries(EGraphicsAPI const targetApi, ResourcePath const& path, std::vector<uint32>*& outData)
{
    // TODO: Do we need to cache this? The resource should guarantee to only load this once.
    ShaderLibrary::ShaderBinaryMap& targetShaderBinaryMap = GetTargetBinaryShaderMap(targetApi);

    auto const foundIt = targetShaderBinaryMap.find(path);
    if (foundIt != targetShaderBinaryMap.end())
    {
        outData = &(*foundIt).second.binaryData;
        return true;
    }

    ShaderBinaryInfo const* spvShaderBinaryInfo = nullptr;
    if (!GetOrCompileSpvBinaryShader(path, spvShaderBinaryInfo))
    {
        return false;
    }

    ShaderBinaryInfo targetShaderBinaryInfo;
    targetShaderBinaryInfo.shaderKind = spvShaderBinaryInfo->shaderKind;
    if (!CrossCompile(spvShaderBinaryInfo->binaryData, spvShaderBinaryInfo->shaderKind, targetApi, targetShaderBinaryInfo.binaryData))
    {
        return false;
    }

    ShaderBinaryInfo& c = targetShaderBinaryMap[path] = std::move(targetShaderBinaryInfo);
    outData = &c.binaryData;
    return true;
}

bool ShaderLibrary::GetOrCompileSpvBinaryShader(ResourcePath const& path, ShaderBinaryInfo const*& outShaderBinaryData)
{
    auto const foundIt = m_spvBinaryShaderMap.find(path);
    if (foundIt != m_spvBinaryShaderMap.end())
    {
        outShaderBinaryData = &(*foundIt).second;
        return true;
    }
    else
    {
        std::string code;
        if (!ReadShaderRawCodeFromFile(path, code))
        {
            return false;
        }

        ShaderBinaryInfo shaderBinaryInfo;

        if (!GetShaderKindFromFilePath(path, shaderBinaryInfo.shaderKind))
        {
            return false;
        }

        if (!CompileSpvShaderBinaries(path, shaderBinaryInfo.shaderKind, code, shaderBinaryInfo.binaryData))
        {
            return false;
        }

        outShaderBinaryData = &(m_spvBinaryShaderMap[path] = std::move(shaderBinaryInfo));
        return true;
    }
}

bool ShaderLibrary::CompileSpvShaderBinaries(ResourcePath const& path, shaderc_shader_kind const shaderKind, std::string const& code, std::vector<uint32>& outData)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
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

            B2D_LOG_ERROR("Failed to compile shader: {} [Result: {}]\n{}", path, result.GetCompilationStatus(), errorMessage);
        }
        else
        {
            B2D_LOG_ERROR("Failed to compile shader: {} [Result: {}]", path, result.GetCompilationStatus());
        }

        return false;
    }

    outData = std::vector(result.begin(), result.end());

    //Reflect(outData, path);

    return true;
}

bool ShaderLibrary::CrossCompile(std::vector<uint32> const& spvBinaryData, shaderc_shader_kind const shaderKind, EGraphicsAPI const targetApi, std::vector<uint32>& outData)
{
    if (targetApi == EGraphicsAPI::Vulkan)
    {
        outData = spvBinaryData;
        return true;
    }
    else if (targetApi == EGraphicsAPI::OpenGL)
    {
        spirv_cross::CompilerGLSL compilerg(spvBinaryData);
        std::string const openglCode = compilerg.compile();

        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
        options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);

        shaderc::SpvCompilationResult const result = compiler.CompileGlslToSpv(openglCode, shaderKind, "", options);
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

void ShaderLibrary::Reflect(std::vector<uint32> const& data, ResourcePath const& path)
{
    spirv_cross::Compiler compiler(data);
    spirv_cross::ShaderResources resource = compiler.get_shader_resources();

    B2D_LOG_INFO("Reflection of {}:", path);

    if (!resource.sampled_images.empty())
    {
        B2D_LOG_INFO("  [Sampled Images]");
        for (auto const& a : resource.sampled_images)
        {
            B2D_LOG_INFO("    {}", a.name);
        }
    }

    if (!resource.stage_inputs.empty())
    {
        B2D_LOG_INFO("  [Stage Inputs]");
        for (auto const& a : resource.stage_inputs)
        {
            B2D_LOG_INFO("    {}", a.name);
        }
    }

    if (!resource.uniform_buffers.empty())
    {
        B2D_LOG_INFO("  [Uniform Buffers]");
        for (auto const& a : resource.uniform_buffers)
        {
            B2D_LOG_INFO("    {}", a.name);
        }
    }
}

void ShaderLibrary::PreloadAll()
{
    B2D_LOG_INFO("Preloading shaders...");

    EGraphicsAPI const graphicsApi = GameEngine::Instance()->GetGHI()->GetGraphicsAPI();

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
        GetShaderBinaries(graphicsApi, path, data);
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

ShaderLibrary::ShaderBinaryMap& ShaderLibrary::GetTargetBinaryShaderMap(EGraphicsAPI const targetApi)
{
    B2D_STATIC_ASSERT(m_targetBinaryShaderMaps.size() == static_cast<size_t>(EGraphicsAPI::_Size), "m_targetBinaryShaderMaps is not the same size as available graphics api's!");
    return m_targetBinaryShaderMaps[(int)targetApi];
}
