#include "B2D_pch.h"
#include "World.h"

#include "GameEngine.h"
#include "Game/Core/System.h"
#include "Game/CameraEntity.h"
#include "Graphics/Viewport.h"
#include "Platform/GenericWindow.h"

World::World(CGameInstance* const owningGameInstance, WorldRenderer* const worldRenderer)
    : mOwningGameInstance(owningGameInstance)
    , mWorldRenderer(worldRenderer)
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
