#include "B2D_pch.h"
#include "Shader.h"

#include "Core/Core.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "Graphics/ShaderLibrary.h"
#include "GameEngine.h"

#include <fstream>
#include <iostream>
#include <sstream>

bool Shader::Load(ResourcePath const& path, EShaderType type)
{
    IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();

    std::vector<uint32>* data = nullptr;
    if (!ShaderLibrary::GetShaderBinaries(ghi->GetGraphicsAPI(), path, data))
    {
        return false;
    }

    switch (type)
    {
        case EShaderType::Vertex:
            m_ghiShader = ghi->CreateVertexShader(*data);
            break;
        case EShaderType::Pixel:
            m_ghiShader = ghi->CreatePixelShader(*data);
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
