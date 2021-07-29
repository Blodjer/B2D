#pragma once

#include "Core/Core.h"
#include "Graphics/GHI/GHISurface.h"

class CGameInstance;
class CViewport;
class IPlatformApplicationInterface;

class GenericWindow
{
public:
    GenericWindow(void* nativeHandle, uint32 width, uint32 height);
    virtual ~GenericWindow();

    void AssignGameInstance(CGameInstance* gameInstance);
    CGameInstance* GetAssignedGameInstance() const { return m_assignedGameInstance; }

    CViewport* GetViewport() const { return m_viewport; }

    virtual void SetSize(uint32 width, uint32 height) = 0; // TODO: Rename to "request"?
    virtual void SetVsync(bool enable) = 0;

    virtual void SetShouldClose(bool close) = 0;
    virtual bool ShouldClose() const = 0;

    virtual void* GetNativeHandle() const = 0;
    virtual void MakeContextCurrent() = 0; // TODO: Remove

    virtual void Present();

    GHISurface* GetSurface() const { return m_surface; }

private:
    // GetGHI()->CreateSurface(this)
    // GraphicsResource<GHISurface>* surface
    //    surface.IsValid()
    //       surface->Resize()

    GHISurface* m_surface = nullptr;

    CGameInstance* m_assignedGameInstance = nullptr;
    CViewport* m_viewport; // TODO: Remove
};