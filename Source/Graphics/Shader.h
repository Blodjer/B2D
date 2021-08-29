#pragma once

#include "Core/Resource.h"
#include "Graphics/ShaderLayout.h"

class GHIShader;

class Shader : public IResource
{
protected:
    bool Load(ResourcePath const& path, EShaderType type);
    virtual void Free() override;

public:
    GHIShader* GetGHIShader() const { return m_ghiShader; }
    ShaderLayout const& GetLayout() const { return m_shaderLayout; }

private:
    GHIShader* m_ghiShader = nullptr;
    ShaderLayout m_shaderLayout;
};

class VertexShader : public Shader
{
protected:
    virtual bool Load(ResourcePath const& path) override { return Shader::Load(path, EShaderType::Vertex); }
public:
    static constexpr auto GetFallbackResourcePath() { return "Content/Shader/Default.vs.glsl"; }
};

class PixelShader : public Shader
{
protected:
    virtual bool Load(ResourcePath const& path) override { return Shader::Load(path, EShaderType::Pixel); }
public:
    static constexpr auto GetFallbackResourcePath() { return "Content/Shader/Default.fs.glsl"; }
};

using VertexShaderRef = ResourcePtr<VertexShader>;
using PixelShaderRef = ResourcePtr<PixelShader>;
