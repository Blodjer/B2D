#pragma once
#include "Graphics/GHI/GraphicsHardwareInterface.h"

class OpenGLGHI : public IGraphicsHardwareInterface
{
public:
    OpenGLGHI();
    ~OpenGLGHI();

    virtual bool Init() override;
    virtual void Shutdown() override;

    virtual void Clear() override;

    virtual GHITexture* CreateTexture(void* data, uint32 width, uint32 height) override;
    virtual void FreeTexture(GHITexture* texture) override;

    virtual GHIShader* CreateVertexShader(char* code) override;
    virtual GHIShader* CreatePixelShader(char* code) override;
};

