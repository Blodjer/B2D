#pragma once

#include "GHI/GraphicsCommon.h"
#include "Core/Resource.h"
#include "Graphics/ShaderLayout.h"

#include <shaderc/shaderc.hpp>

using ShaderBinaryData = std::vector<uint32>;

class ShaderLibrary
{
private:
    struct ShaderSpvData
    {
        // TODO: Add hash key?
        ShaderBinaryData binaryData;
        shaderc_shader_kind shaderKind;
    };

    using ShaderSpvDataMap = std::unordered_map<ResourcePath, ShaderSpvData>;

public:
    static bool GetShaderBinaries(EGraphicsAPI const targetApi, ResourcePath const& path, ShaderBinaryData*& outData);
    static ShaderLayout const GetShaderLayout(ShaderBinaryData const& shaderBinaryData);

    static void PreloadAll(EGraphicsAPI const targetApi);

private:
    static bool GetOrCompileSpvBinaryShader(ResourcePath const& path, ShaderSpvData const*& outSpvData);
    static bool CompileSpvShaderBinaries(shaderc_shader_kind const shaderKind, std::string const& code, ShaderBinaryData& outData);
    
    static bool CrossCompile(ShaderSpvData const& spvData, EGraphicsAPI const targetApi, ShaderBinaryData& outData);

    static void Reflect(ShaderBinaryData const& shaderBinaryData, ShaderLayout& outShaderLayout);

    static bool ReadShaderRawCodeFromFile(ResourcePath const& path, std::string& outCode);

    static bool GetShaderKindFromFilePath(ResourcePath const& path, shaderc_shader_kind& outShaderKind);
    static shaderc_shader_kind ShaderTypeToShaderKind(EShaderType const shaderType);

    static ShaderSpvDataMap& GetTargetBinaryShaderMap(EGraphicsAPI const targetApi);

private:
    INLINE static ShaderSpvDataMap m_spvBinaryShaderMap;
    INLINE static std::array<ShaderSpvDataMap, static_cast<uint32>(EGraphicsAPI::_Size)> m_targetBinaryShaderMaps;

};

