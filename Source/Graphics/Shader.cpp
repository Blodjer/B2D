#include "B2D_pch.h"
#include "Shader.h"

#include "Core/Core.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "GameEngine.h"

#include <fstream>
#include <iostream>
#include <sstream>

//  GLuint ul = glGetUniformLocation(mID, name);
// 	if (ul == -1)
// 	{
// 		B2D_WARNING("Cannot find shader uniform location: {0}", name);
// 		return;
// 	}
//  glUniform1i(ul, value);
//  glUniform1f(ul, value);
//  glUniformMatrix4fv(ul, 1, GL_FALSE, value);

bool Shader::Load(ResourcePath const& path, ShaderType type)
{
    std::string code;
    if (!ReadShaderFromFile(path, code))
    {
        return false;
    }

    IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();
    switch (type)
    {
        case ShaderType::Vertex:
            m_ghiShader = ghi->CreateVertexShader(code.data());
            break;
        case ShaderType::Pixel:
            m_ghiShader = ghi->CreatePixelShader(code.data());
            break;
        default:
            B2D_BREAKf("ShaderType {} not implemented", static_cast<int32>(type));
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
        B2D_ERROR("Shader Read Error: {0}", path);
    }

    return false;
}
