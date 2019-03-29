#include "B2D_pch.h"
#include "World.h"
#include "ECS/System/System.h"
#include "CameraEntity.h"
#include "Core/GameEngine.h"
#include "Graphics/GenericWindow.h"
#include "Graphics/Viewport.h"

World::World(CGameInstance* const owningGameInstance)
    : mOwningGameInstance(owningGameInstance)
{
    CameraEntity* defaultCamera = AddSystemEntityObject<CameraEntity>();
    CGameEngine::Instance()->GetMainWindow()->GetViewport()->SetCamera(defaultCamera);
}

World::~World()
{

}

void World::Update(float deltaTime)
{
    for (System* const system : mSystems)
    {
        system->Update(deltaTime);
    }
}
