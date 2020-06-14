#pragma once

#include "Core/Resource.h"
#include "GHI/GHIShader.h"

enum class ShaderType
{
    Vertex,
    Pixel,
    Compute,
    Geometry
};

class Shader : public IResource
{
protected:
    bool Load(ResourcePath const& path, ShaderType type);
    virtual void Free() override;

public:
    GHIShader* GetGHIShader() const { return m_ghiShader; }

private:
    bool ReadShaderFromFile(ResourcePath const& path, std::string& outCode);

private:
    GHIShader* m_ghiShader = nullptr;
};

class VertexShader : public Shader
{
protected:
    virtual bool Load(ResourcePath const& path) override { return Shader::Load(path, ShaderType::Vertex); }
public:
    static constexpr auto GetFallbackResourcePath() { return "Content/Shader/DefaultVS.glsl"; }
};

class PixelShader : public Shader
{
protected:
    virtual bool Load(ResourcePath const& path) override { return Shader::Load(path, ShaderType::Pixel); }
public:
    static constexpr auto GetFallbackResourcePath() { return "Content/Shader/DefaultPS.glsl"; }
};

using VertexShaderRef = ResourcePtr<VertexShader>;
using PixelShaderRef = ResourcePtr<PixelShader>;
