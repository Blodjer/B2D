#include "B2D_pch.h"
#include "GenericWindow.h"

#include "GameEngine.h"
#include "Game/GameInstance.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "Graphics/Viewport.h"

GenericWindow::GenericWindow(void* nativeHandle, uint32 width, uint32 height)
{
    m_viewport = new CViewport(width, height);
    m_surface = GameEngine::Instance()->GetGHI()->CreateSurface(nativeHandle, width, height);
}

GenericWindow::~GenericWindow()
{
    // TODO: Destroy surface
    delete m_viewport;
}

void GenericWindow::AssignGameInstance(CGameInstance* gameInstance)
{
    B2D_CHECK_f(m_assignedGameInstance != nullptr, "Window already has a assigned game instance");
    m_assignedGameInstance = gameInstance;
}

void GenericWindow::Present()
{
    //m_surface->Present(TODO);
    B2D_NOT_IMPLEMENTED();
}
