#pragma once

class IGraphicsHardwareInterface
{
public:
    virtual void Clear() = 0;
    virtual void Swap() = 0;

    virtual void SetViewport() = 0;
    virtual void SetMaterial() = 0;

    virtual void DrawQuad() = 0;

public:
    virtual void CreateTexture() = 0;
    virtual void CreateShader() = 0;

};

// IShader
// ShaderParams

// ITexture