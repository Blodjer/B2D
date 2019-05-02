#include "B2D_pch.h"
#include "World.h"

#include "GameEngine.h"
#include "Game/Core/System.h"
#include "Game/CameraEntity.h"
#include "Graphics/Viewport.h"
#include "Platform/GenericWindow.h"

World::World(CGameInstance* const owningGameInstance)
    : mOwningGameInstance(owningGameInstance)
{
    mWorldRenderDataInterface = new WorldRenderDataInterface();

    CameraEntity* defaultCamera = AddSystemEntityObject<CameraEntity>();
    CGameEngine::Instance()->GetMainWindow()->GetViewport()->SetCamera(defaultCamera);
}

World::~World()
{
    delete mWorldRenderDataInterface;
}

void World::Update(float deltaTime)
{
    for (System* const system : mSystems)
    {
        system->Update(deltaTime);
    }
}
