#pragma once

#include "Core/Core.h"

class CGameInstance;
class CViewport;
class IPlatformApplicationInterface;

class GenericWindow
{
public:
    GenericWindow(uint32 width, uint32 height);
    virtual ~GenericWindow();

    void AssignGameInstance(CGameInstance* gameInstance);
    CGameInstance* GetAssignedGameInstance() const { return m_assignedGameInstance; }

    CViewport* GetViewport() const { return m_viewports[0]; }
    std::vector<CViewport*> const& GetViewports() const { return m_viewports; }

    virtual void SetSize(uint32 width, uint32 height) = 0;
    virtual void SetVsync(bool enable) = 0;

    virtual void SetShouldClose(bool close) = 0;
    virtual bool ShouldClose() const = 0;

    virtual void* GetNativeHandle() const = 0;

    virtual void MakeContextCurrent() = 0;
    virtual void Swap() = 0;

private:
    CGameInstance* m_assignedGameInstance = nullptr;

    std::vector<CViewport*> m_viewports;
};