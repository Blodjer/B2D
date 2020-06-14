#pragma once

#include "Core/Core.h"
#include "Core/Resource.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

class GHIMaterial;

class Material
{
public:
    Material(VertexShaderRef vertexShader, PixelShaderRef pixelShader);

    void SetBool(char const* name, bool value);
    void SetInt(char const* name, int32 value);
    void SetFloat(char const* name, float value);
    void SetVector(char const* name, float const* value);
    void SetTexture(uint32 index, TextureRef const& texture);

    std::vector<TextureRef> const& GetTextures() const { return m_textures; }

    GHIMaterial* GetGHIMaterial() const { return m_ghiMaterial; }

private:
    void OnShaderChanged();

private:
    VertexShaderRef m_vertexShader;
    PixelShaderRef m_pixelShader;

    std::vector<TextureRef> m_textures;

    GHIMaterial* m_ghiMaterial;
};
