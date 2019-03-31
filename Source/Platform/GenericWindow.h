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
    CGameInstance* GetAssignedGameInstance() const { return mAssignedGameInstance; }

    CViewport* GetViewport() const { return mViewports[0]; }
    std::vector<CViewport*> const& GetViewports() const { return mViewports; }

    virtual void SetSize(uint32 width, uint32 height) = 0;
    virtual void SetVsync(bool enable) = 0;

    virtual void SetShouldClose(bool close) = 0;
    virtual bool ShouldClose() const = 0;

    virtual void MakeContextCurrent() = 0;
    virtual void Swap() = 0;

private:
    CGameInstance* mAssignedGameInstance = nullptr;

    std::vector<CViewport*> mViewports;
};