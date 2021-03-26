#include "B2D_pch.h"
#include "GenericWindow.h"

#include "Game/GameInstance.h"
#include "Graphics/Viewport.h"

GenericWindow::GenericWindow(uint32 width, uint32 height)
{
    m_viewports.emplace_back(new CViewport(width, height));
}

GenericWindow::~GenericWindow()
{
    for (CViewport* viewport : m_viewports)
    {
        delete viewport;
    }
}

void GenericWindow::AssignGameInstance(CGameInstance* gameInstance)
{
    B2D_CHECKf(m_assignedGameInstance != nullptr, "Window already has a assigned game instance");
    m_assignedGameInstance = gameInstance;
}