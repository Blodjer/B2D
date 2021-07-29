#pragma once
#include "Graphics/GHI/GraphicsHardwareInterface.h"

#include <GL/glew.h>

class OpenGLGHI final : public IGraphicsHardwareInterface
{
public:
    virtual bool Init() override;
    virtual void Shutdown() override;
    virtual void BeginRenderPass() override { B2D_NOT_IMPLEMENTED(); };
    virtual void EndRenderPass() override { B2D_NOT_IMPLEMENTED(); };

    EGraphicsAPI GetGraphicsAPI() const override { return EGraphicsAPI::OpenGL; }

public:
    virtual GHISurface* CreateSurface(void* nativeWindowHandle, uint32 width, uint32 height) override;

    virtual void Clear(bool color, bool depth, bool stencil) override;

    // Texture

    virtual GHITexture* CreateTexture(void const* data, uint32 width, uint32 height, uint8 components) override;
    virtual void BindTexture(GHITexture const* texture) override;
    virtual void FreeTexture(GHITexture*& texture) override;

    // Shader

    GHIShader* CreateShader(std::vector<uint32> const& data, GLuint type);
    virtual GHIShader* CreateVertexShader(std::vector<uint32> const& data) override;
    virtual GHIShader* CreatePixelShader(std::vector<uint32> const& data) override;

    virtual void DeleteShader(GHIShader*& shader) override;

    // Material

    virtual GHIMaterial* CreateMaterial(GHIShader* vertexShader, GHIShader* pixelShader) override;
    virtual void FreeMaterial(GHIMaterial*& material) override;
    virtual void BindMaterial(GHIMaterial* material) override;

    // RenderTarget

    virtual GHIRenderTarget* CreateRenderTarget() override;
    virtual GHIRenderTarget* CreateRenderTarget(GHITexture* texture) override;
    virtual void ResizeRenderTarget(GHIRenderTarget*& renderTarget, uint32 width, uint32 height);
    virtual void DeleteRenderTarget(GHIRenderTarget*& renderTarget, bool freeTexture) override;
    virtual void BindRenderTarget(GHIRenderTarget* renderTarget) override;
    virtual void BindRenderTargetAndClear(GHIRenderTarget* renderTarget) override;

protected:
    virtual bool ImGui_Init() override;
    virtual void ImGui_Shutdow() override;
    virtual void ImGui_BeginFrame() override;
    virtual void ImGui_Render() override;

};

