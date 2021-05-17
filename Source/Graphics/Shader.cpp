#include "B2D_pch.h"
#include "Shader.h"

#include "Core/Core.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "GameEngine.h"

#include <fstream>
#include <iostream>
#include <sstream>

bool Shader::Load(ResourcePath const& path, ShaderType type)
{
//     std::string code;
//     if (!ReadShaderFromFile(path, code))
//     {
//         return false;
//     }

    std::vector<char> data;
    if (!ReadShaderFromFile(path, data))
    {
        return false;
    }


    IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();
    switch (type)
    {
        case ShaderType::Vertex:
            m_ghiShader = ghi->CreateVertexShader(data);
            break;
        case ShaderType::Pixel:
            m_ghiShader = ghi->CreatePixelShader(data);
            break;
        default:
            B2D_BREAK_f("ShaderType {} not implemented", static_cast<int32>(type));
            break;
    }

    if (m_ghiShader == nullptr)
    {
        return false;
    }

    return true;
}

void Shader::Free()
{
    IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();
    ghi->DeleteShader(m_ghiShader);
    m_ghiShader = nullptr;
}

bool Shader::ReadShaderFromFile(ResourcePath const& path, std::string& outCode)
{
    std::string code;

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
        B2D_LOG_ERROR("Shader Read Error: {0}", path);
    }

    return false;
}

bool Shader::ReadShaderFromFile(ResourcePath const& path, std::vector<char>& outData)
{
    std::string code;

    std::ifstream fileStream(path, std::ios::in | std::ios::binary);
    fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    if (fileStream.is_open())
    {
        fileStream.seekg(0, std::ios::end);
        auto size = fileStream.tellg();
        fileStream.seekg(0, std::ios::beg);

        auto& data = outData;
        //data.resize(size / sizeof(uint32));
        data.resize(size);
        fileStream.read((char*)data.data(), size);
        return true;
    }

    try
    {

    }
    catch (std::ifstream::failure e)
    {
        B2D_LOG_ERROR("Shader Read Error: {0}", path);
    }

    return false;
}
