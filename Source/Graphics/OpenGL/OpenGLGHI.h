#pragma once
#include "Graphics/GHI/GraphicsHardwareInterface.h"

#include <GL/glew.h>

class OpenGLGHI final : public IGraphicsHardwareInterface
{
public:
    virtual bool Init() override;
    virtual void Shutdown() override;

    EGraphicsAPI GetGraphicsAPI() const override { return EGraphicsAPI::OpenGL; }

public:
    virtual GHISurface* CreateSurface(void* nativeWindowHandle, uint32 width, uint32 height) override;

    virtual GHITexture* CreateTexture(void const* data, uint32 width, uint32 height, uint8 components) override;
    virtual void FreeTexture(GHITexture*& texture) override;

    GHIShader* CreateShader(std::vector<uint32> const& data, GLuint type);
    virtual GHIShader* CreateVertexShader(std::vector<uint32> const& data) override;
    virtual GHIShader* CreatePixelShader(std::vector<uint32> const& data) override;

    virtual void DestroyShader(GHIShader*& shader) override;

    virtual GHIRenderTarget* CreateRenderTarget(uint32 width, uint32 height) override;
    GHIRenderTarget* CreateRenderTarget(GHITexture* texture);
    virtual void DestroyRenderTarget(GHIRenderTarget* renderTarget) override;

    virtual GHIRenderPass* CreateRenderPass(std::vector<GHIRenderTarget*> const& renderTargets) override;
    virtual void DestroyRenderPass(GHIRenderPass* renderPass) override;

    virtual void BeginRenderPass(GHIRenderPass* renderPass, GHICommandList* commandList) override { B2D_NOT_IMPLEMENTED(); }
    virtual void EndRenderPass(GHIRenderPass* renderPass, GHICommandList* commandList) override { B2D_NOT_IMPLEMENTED(); }

    virtual GHICommandList* AllocateCommandBuffer() override { B2D_NOT_IMPLEMENTED(); }
    virtual void FreeCommandBuffer(GHICommandList* commandList) override { B2D_NOT_IMPLEMENTED(); }

    virtual void Submit(std::vector<GHICommandList*>& commandLists) override { B2D_NOT_IMPLEMENTED(); }

protected:
    virtual bool ImGui_Init() override;
    virtual void ImGui_Shutdow() override;
    virtual void ImGui_BeginFrame() override;
    virtual void ImGui_Render(GHICommandList* commandList) override;

};

