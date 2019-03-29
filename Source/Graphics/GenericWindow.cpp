#include "B2D_pch.h"
#include "GenericWindow.h"

#include "Core/GameInstance.h"
#include "Graphics/Viewport.h"

GenericWindow::GenericWindow(uint32 width, uint32 height)
{
    mViewports.emplace_back(new CViewport(0, 0, width, height));
}

GenericWindow::~GenericWindow()
{
    for (CViewport* viewport : mViewports)
    {
        delete viewport;
    }
}

void GenericWindow::AssignGameInstance(CGameInstance* gameInstance)
{
    B2D_CHECKf(mAssignedGameInstance != nullptr, "Window already has a assigned game instance");
    mAssignedGameInstance = gameInstance;
}