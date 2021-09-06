#pragma once

#include "GraphicsCommon.h"
#include "Graphics/Shader.h"

class GHIShader;
class GHIMaterial;
class GHITexture;
class GHISurface;
class GHIRenderPass;
class GHICommandList;
class GHIBuffer;
class GHIGraphicsPipeline;
class GHIResourceSet;
enum class EGHIBufferType;
enum class EGHITextureFormat;
enum EGHITextureUsageFlags;

class IGraphicsHardwareInterface
{
protected:
    IGraphicsHardwareInterface() = default;

private:
    IGraphicsHardwareInterface(const IGraphicsHardwareInterface&) = delete;
    void operator=(const IGraphicsHardwareInterface&) = delete;

public:
    virtual bool Init() = 0;
    virtual void Shutdown() = 0;

    virtual EGraphicsAPI GetGraphicsAPI() const = 0;

public:
    virtual GHISurface* CreateSurface(void* nativeWindowHandle, uint32 width, uint32 height) = 0;
    virtual void DestroySurface(GHISurface* surface) = 0;

public:
    virtual GHIShader* CreateVertexShader(std::vector<uint32> const& data) = 0;
    virtual GHIShader* CreatePixelShader(std::vector<uint32> const& data) = 0;
    virtual void DestroyShader(GHIShader*& shader) = 0;

    virtual GHIGraphicsPipeline* CreateGraphicsPipeline(GHIRenderPass const* targetPass, VertexShaderRef vertexShader, PixelShaderRef pixelShader) = 0;
    virtual void DestroyGraphicsPipeline(GHIGraphicsPipeline* pipeline) = 0;

    virtual GHIResourceSet* CreateResourceSet() = 0;
    virtual void DestroyResourceSet(GHIResourceSet* resourceSet) = 0;

    virtual GHITexture* CreateTexture(uint32 width, uint32 height, EGHITextureFormat format, EGHITextureUsageFlags usage) = 0;
    virtual GHITexture* CreateTexture(void const* data, uint32 width, uint32 height, uint8 components) = 0;
    virtual void DestroyTexture(GHITexture* texture) = 0;

    virtual GHIRenderPass* CreateRenderPass(std::vector<GHITexture*> const& renderTargets, GHITexture const* depthTarget) = 0;
    virtual void DestroyRenderPass(GHIRenderPass* renderPass) = 0;

    /* Command */ virtual void BeginRenderPass(GHIRenderPass* renderPass, GHICommandList* commandList) = 0;
    /* Command */ virtual void EndRenderPass(GHIRenderPass* renderPass, GHICommandList* commandList) = 0;

    virtual GHICommandList* AllocateCommandBuffer() = 0;
    virtual void FreeCommandBuffer(GHICommandList* commandList) = 0;
    virtual void Submit(std::vector<GHICommandList*>& commandLists) = 0;

    virtual GHIBuffer* CreateBuffer(EGHIBufferType bufferType, uint size) = 0;

protected:
    friend class EditorModule;
    virtual bool ImGui_Init() = 0;
    virtual void ImGui_Shutdow() = 0;
    virtual void ImGui_BeginFrame() = 0;
    virtual void ImGui_Render(GHICommandList* commandList) = 0;
};
