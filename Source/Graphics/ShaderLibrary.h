#pragma once

#include "GHI/GraphicsCommon.h"
#include "Shader.h"

#include <shaderc/shaderc.hpp>

struct ShaderBinaryInfo
{
    // TODO: Add hash key?
    std::vector<uint32> binaryData;
    shaderc_shader_kind shaderKind;
};

class ShaderLibrary
{
    using ShaderBinaryMap = std::unordered_map<ResourcePath, ShaderBinaryInfo>;

public:
    static bool GetShaderBinaries(EGraphicsAPI const targetApi, ResourcePath const& path, std::vector<uint32>*& outData);
    static void PreloadAll();

private:
    static bool GetOrCompileSpvBinaryShader(ResourcePath const& path, ShaderBinaryInfo const*& outShaderBinaryData);
    static bool CompileSpvShaderBinaries(ResourcePath const& path, shaderc_shader_kind const shaderKind, std::string const& code, std::vector<uint32>& outData);
    
    static bool CrossCompile(std::vector<uint32> const& spSvBinaryData, shaderc_shader_kind const shaderKind, EGraphicsAPI const targetApi, std::vector<uint32>& outData);

    static void Reflect(std::vector<uint32> const& data, ResourcePath const& path);

    static bool ReadShaderRawCodeFromFile(ResourcePath const& path, std::string& outCode);

    static bool GetShaderKindFromFilePath(ResourcePath const& path, shaderc_shader_kind& outShaderKind);
    static shaderc_shader_kind ShaderTypeToShaderKind(EShaderType const shaderType);

    static ShaderBinaryMap& GetTargetBinaryShaderMap(EGraphicsAPI const targetApi);

private:
    INLINE static ShaderBinaryMap m_spvBinaryShaderMap;
    INLINE static std::array<ShaderBinaryMap, static_cast<uint32>(EGraphicsAPI::_Size)> m_targetBinaryShaderMaps;

};

