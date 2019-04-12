#pragma once

#include "Core/Core.h"

class GHIShader;
class GHITexture;

class IGraphicsHardwareInterface
{
public:
    virtual bool Init() = 0;
    virtual void Shutdown() = 0;

public:
    virtual void Clear() = 0;

    //virtual void SetViewport() = 0;
    //virtual void SetMaterial() = 0;

    //virtual void DrawQuad() = 0;

    virtual GHITexture* CreateTexture(void* data, uint32 width, uint32 height) = 0;
    virtual void FreeTexture(GHITexture* texture) = 0;

    virtual GHIShader* CreateVertexShader(char* code) = 0;
    virtual GHIShader* CreatePixelShader(char* code) = 0;

    //virtual void SetRenderTarget() = 0;

};

// IShader
// ShaderParams