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
    virtual GHISurface* CreateSurface(void* nativeWindowHandle, uint32 width, uint32 height) override { B2D_NOT_IMPLEMENTED(); }
    virtual void DestroySurface(GHISurface* surface) override { B2D_NOT_IMPLEMENTED(); }

    virtual GHITexture* CreateTexture(uint32 width, uint32 height, EGHITextureFormat format, EGHITextureUsageFlags usage) { B2D_NOT_IMPLEMENTED(); }
    virtual GHITexture* CreateTexture(void const* data, uint32 width, uint32 height, EGHITextureFormat format) override;
    virtual void DestroyTexture(GHITexture* texture) override;

    virtual GHISampler* CreateSampler() override { B2D_NOT_IMPLEMENTED(); }

    GHIShader* CreateShader(std::vector<uint32> const& data, GLuint type);
    virtual GHIShader* CreateVertexShader(std::vector<uint32> const& data) override;
    virtual GHIShader* CreatePixelShader(std::vector<uint32> const& data) override;

    virtual void DestroyShader(GHIShader*& shader) override;

    virtual GHIGraphicsPipeline* CreateGraphicsPipeline(GHIRenderPass const* targetPass, VertexShaderRef vertexShader, PixelShaderRef pixelShader) override { B2D_NOT_IMPLEMENTED(); }
    virtual void DestroyGraphicsPipeline(GHIGraphicsPipeline* pipeline) override { B2D_NOT_IMPLEMENTED(); }

    virtual GHIResourceSet* CreateResourceSet() override { B2D_NOT_IMPLEMENTED(); }
    virtual void DestroyResourceSet(GHIResourceSet* resourceSet) override { B2D_NOT_IMPLEMENTED(); }

    virtual GHIRenderPass* CreateRenderPass(std::vector<GHITexture*> const& renderTargets, GHITexture const* depthTarget) override { B2D_NOT_IMPLEMENTED(); }
    virtual void DestroyRenderPass(GHIRenderPass* renderPass) override { B2D_NOT_IMPLEMENTED(); }

    virtual void BeginRenderPass(GHIRenderPass* renderPass, GHICommandBuffer* commandBuffer) override { B2D_NOT_IMPLEMENTED(); }
    virtual void EndRenderPass(GHIRenderPass* renderPass, GHICommandBuffer* commandBuffer) override { B2D_NOT_IMPLEMENTED(); }

    virtual GHICommandBuffer* AllocateCommandBuffer() override { B2D_NOT_IMPLEMENTED(); }
    virtual void FreeCommandBuffer(GHICommandBuffer* commandBuffer) override { B2D_NOT_IMPLEMENTED(); }

    virtual void Submit(std::vector<GHICommandBuffer*>& commandBuffers) override { B2D_NOT_IMPLEMENTED(); }

    virtual GHIBuffer* CreateBuffer(EGHIBufferType bufferType, uint size) override { B2D_NOT_IMPLEMENTED(); }
    virtual void DestroyBuffer(GHIBuffer* buffer) { B2D_NOT_IMPLEMENTED(); }

    static GLint Convert(EGHITextureFormat format);

protected:
    virtual bool ImGui_Init() override;
    virtual void ImGui_Shutdow() override;
    virtual void ImGui_BeginFrame() override;
    virtual void ImGui_Render(GHICommandBuffer* commandBuffer) override;

};

