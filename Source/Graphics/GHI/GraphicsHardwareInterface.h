#pragma once

#include "Core/Core.h"
#include "GraphicsCommon.h"

class GHIShader;
class GHIMaterial;
class GHIRenderTarget;
class GHITexture;

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
    //virtual void SetViewport() = 0;
    //virtual void SetMaterial() = 0;

    //virtual void DrawQuad() = 0;

    virtual void Clear(bool color, bool depth, bool stencil) = 0;

    virtual GHITexture* CreateTexture(void const* data, uint32 width, uint32 height, uint8 components) = 0;
    virtual void BindTexture(GHITexture const* texture) = 0;
    virtual void FreeTexture(GHITexture*& texture) = 0;

    virtual GHIShader* CreateVertexShader(std::vector<char> const& data) = 0;
    virtual GHIShader* CreatePixelShader(std::vector<char> const& data) = 0;
    virtual void DeleteShader(GHIShader*& shader) = 0;

    virtual GHIMaterial* CreateMaterial(GHIShader* vertexShader, GHIShader* pixelShader) = 0;
    virtual void FreeMaterial(GHIMaterial*& material) = 0;
    virtual void BindMaterial(GHIMaterial* material) = 0;

    virtual GHIRenderTarget* CreateRenderTarget() = 0;
    virtual GHIRenderTarget* CreateRenderTarget(GHITexture* texture) = 0;
    virtual void ResizeRenderTarget(GHIRenderTarget*& renderTarget, uint32 width, uint32 height) = 0;
    virtual void DeleteRenderTarget(GHIRenderTarget*& renderTarget, bool freeTexture) = 0;
    virtual void BindRenderTarget(GHIRenderTarget* renderTarget) = 0;
    virtual void BindRenderTargetAndClear(GHIRenderTarget* renderTarget) = 0;

protected:
    friend class EditorModule;
    virtual bool ImGui_Init() = 0;
    virtual void ImGui_Shutdow() = 0;
    virtual void ImGui_BeginFrame() = 0;
    virtual void ImGui_Render() = 0;
};

// IShader
// ShaderParams