#pragma once
#include "Graphics/GHI/GraphicsHardwareInterface.h"

#include <GL/glew.h>

class OpenGLGHI : public IGraphicsHardwareInterface
{
public:
    virtual bool Init() override;
    virtual void Shutdown() override;

    virtual void Clear(bool color, bool depth, bool stencil) override;

    // Texture

    virtual GHITexture* CreateTexture(void const* data, uint32 width, uint32 height, uint8 components) override;
    virtual void BindTexture(GHITexture const* texture) override;
    virtual void FreeTexture(GHITexture*& texture) override;

    // Shader

    bool CompileShader(char const* code, GLuint type, GLuint& outHandle);
    
    GHIShader* CreateShader(char const* code, GLuint type);
    virtual GHIShader* CreateVertexShader(char const* code) override;
    virtual GHIShader* CreatePixelShader(char const* code) override;

    virtual void DeleteShader(GHIShader*& shader) override;

    // Material

    virtual GHIMaterial* CreateMaterial(GHIShader* vertexShader, GHIShader* pixelShader) override;
    virtual void FreeMaterial(GHIMaterial*& material) override;
    virtual void BindMaterial(GHIMaterial* material) override;

    // RenderTarget

    virtual GHIRenderTarget* CreateRenderTarget() override;
    virtual GHIRenderTarget* CreateRenderTarget(GHITexture* texture) override;
    virtual void DeleteRenderTarget(GHIRenderTarget*& renderTarget, bool freeTexture) override;
    virtual void BindRenderTarget(GHIRenderTarget* renderTarget) override;
    virtual void BindRenderTargetAndClear(GHIRenderTarget* renderTarget) override;

};

