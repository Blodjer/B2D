#include "B2D_pch.h"
#include "World.h"

#include "GameEngine.h"
#include "Game/Core/System.h"
#include "Game/CameraEntity.h"
#include "Graphics/Viewport.h"
#include "Platform/GenericWindow.h"

World::World(CGameInstance* const owningGameInstance)
    : mOwningGameInstance(owningGameInstance)
    , mSystemAdmin(this)
{
    mWorldRenderDataInterface = new WorldRenderDataInterface();

    CameraEntity* defaultCamera = AddSystemEntityObject<CameraEntity>();
    GameEngine::Instance()->GetMainWindow()->GetViewport()->SetCamera(defaultCamera);
}

World::~World()
{
    delete mWorldRenderDataInterface;
}

void World::Tick(float deltaTime)
{
    mSystemAdmin.Tick(deltaTime);
}
