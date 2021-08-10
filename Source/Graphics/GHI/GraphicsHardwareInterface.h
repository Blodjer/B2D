#pragma once

#include "GraphicsCommon.h"

class GHIShader;
class GHIMaterial;
class GHIRenderTarget;
class GHITexture;
class GHISurface;
class GHIRenderPass;
class GHICommandList;

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

public:
    virtual GHIShader* CreateVertexShader(std::vector<uint32> const& data) = 0;
    virtual GHIShader* CreatePixelShader(std::vector<uint32> const& data) = 0;
    virtual void DestroyShader(GHIShader*& shader) = 0;

public:
    virtual GHITexture* CreateTexture(void const* data, uint32 width, uint32 height, uint8 components) = 0;
    virtual void FreeTexture(GHITexture*& texture) = 0;

    virtual GHIRenderTarget* CreateRenderTarget(uint32 width, uint32 height) = 0;
    virtual void DestroyRenderTarget(GHIRenderTarget* renderTarget) = 0;

    virtual GHIRenderPass* CreateRenderPass(std::vector<GHIRenderTarget*> const& renderTargets) = 0;
    virtual void DestroyRenderPass(GHIRenderPass* renderPass) = 0;

    /* Command */ virtual void BeginRenderPass(GHIRenderPass* renderPass, GHICommandList* commandList) = 0;
    /* Command */ virtual void EndRenderPass(GHIRenderPass* renderPass, GHICommandList* commandList) = 0;

    virtual GHICommandList* AllocateCommandBuffer() = 0;
    virtual void FreeCommandBuffer(GHICommandList* commandList) = 0;
    virtual void Submit(std::vector<GHICommandList*>& commandLists) = 0;

protected:
    friend class EditorModule;
    virtual bool ImGui_Init() = 0;
    virtual void ImGui_Shutdow() = 0;
    virtual void ImGui_BeginFrame() = 0;
    virtual void ImGui_Render(GHICommandList* commandList) = 0;
};
