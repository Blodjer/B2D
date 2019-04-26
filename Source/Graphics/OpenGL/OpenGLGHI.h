#pragma once
#include "Graphics/GHI/GraphicsHardwareInterface.h"

class OpenGLGHI : public IGraphicsHardwareInterface
{
public:
    virtual bool Init() override;
    virtual void Shutdown() override;

    virtual void Clear(bool color, bool depth, bool stencil) override;

    virtual GHITexture* CreateTexture(void* data, uint32 width, uint32 height, uint8 components) override;
    virtual void BindTexture(GHITexture const* texture) override;
    virtual void FreeTexture(GHITexture* texture) override;

    virtual GHIShader* CreateVertexShader(char* code) override;
    virtual GHIShader* CreatePixelShader(char* code) override;
    virtual void DeleteShader(GHIShader* shader) override;

    virtual GHIMaterial* CreateMaterial(GHIShader* vertexShader, GHIShader* pixelShader) override;
    virtual void BindMaterial(GHIMaterial* material) override;

    virtual GHIRenderTarget* CreateRenderTarget() override;
    virtual void DeleteRenderTarget(GHIRenderTarget* renderTarget) override;
    virtual void BindRenderTarget(GHIRenderTarget* renderTarget) override;
    virtual void BindRenderTargetAndClear(GHIRenderTarget* renderTarget) override;

};

