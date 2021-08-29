#include "B2D_pch.h"
#include "Shader.h"

#include "GameEngine.h"
#include "Core/Core.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "Graphics/ShaderLibrary.h"
#include "GHI/GHIShader.h"

bool Shader::Load(ResourcePath const& path, EShaderType type)
{
    IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();

    ShaderBinaryData* data = nullptr;
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

    m_shaderLayout = ShaderLibrary::GetShaderLayout(*data);

    return true;
}

void Shader::Free()
{
    IGraphicsHardwareInterface* ghi = GameEngine::Instance()->GetGHI();
    ghi->DestroyShader(m_ghiShader);
    m_ghiShader = nullptr;
}
