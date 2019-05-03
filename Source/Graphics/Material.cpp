#include "B2D_pch.h"
#include "Material.h"

#include "GameEngine.h"
#include "Graphics/GHI/GHIMaterial.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "Shader.h"
#include "Texture.h"

Material::Material(VertexShaderRef vertexShader, PixelShaderRef pixelShader)
    : mVertexShader(vertexShader)
    , mPixelShader(pixelShader)
{
    if (vertexShader.IsValid() && pixelShader.IsValid())
    {
        mGHIMaterial = GameEngine::Instance()->GetGHI()->CreateMaterial(vertexShader->GetGHIShader(), pixelShader->GetGHIShader());
    }

    vertexShader->RegisterChangeCallback(TResourceChangedDelegate::CREATE(this, &Material::OnShaderChanged));
    pixelShader->RegisterChangeCallback(TResourceChangedDelegate::CREATE(this, &Material::OnShaderChanged));
}

void Material::SetBool(char const* name, bool value)
{
    SetInt(name, static_cast<int32>(value));
}

void Material::SetInt(char const* name, int32 value)
{
    B2D_CORE_ERROR("Material::SetInt not implemented");
}

void Material::SetFloat(char const* name, float value)
{
    B2D_CORE_ERROR("Material::SetFloat not implemented");
}

void Material::SetVector(char const* name, const float* value)
{
    B2D_CORE_ERROR("Material::SetVector not implemented");
}

void Material::SetTexture(uint32 index, TextureRef const& texture)
{
    B2D_ASSERT(index <= mTextures.size());

    if (index < mTextures.size())
    {
        mTextures[index] = texture;
    }
    else
    {
        mTextures.emplace_back(texture);
    }
}

void Material::OnShaderChanged()
{
    mGHIMaterial = GameEngine::Instance()->GetGHI()->CreateMaterial(mVertexShader->GetGHIShader(), mPixelShader->GetGHIShader());
}
