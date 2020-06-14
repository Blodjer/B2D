#include "B2D_pch.h"
#include "World.h"

#include "GameEngine.h"
#include "Game/Core/System.h"
#include "Game/CameraEntity.h"
#include "Graphics/Viewport.h"
#include "Platform/GenericWindow.h"

World::World(CGameInstance* const owningGameInstance)
    : m_owningGameInstance(owningGameInstance)
    , m_systemAdmin(this)
{
    m_worldRenderDataInterface = new WorldRenderDataInterface();

    CameraEntity* defaultCamera = AddSystemEntityObject<CameraEntity>();
    GameEngine::Instance()->GetMainWindow()->GetViewport()->SetCamera(defaultCamera);
}

World::~World()
{
    delete m_worldRenderDataInterface;
}

void World::Tick(float deltaTime)
{
    m_systemAdmin.Tick(deltaTime);
}
