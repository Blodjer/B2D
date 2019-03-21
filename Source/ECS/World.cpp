#include "B2D_pch.h"
#include "World.h"
#include "ECS/System/System.h"
#include "CameraEntity.h"
#include "Core/GameEngine.h"
#include "Graphics/Window.h"
#include "Graphics/Viewport.h"

World::World()
{
    CameraEntity* defaultCamera = AddSystemEntityObject<CameraEntity>();
    CGameEngine::Instance()->GetWindow()->GetViewport()->SetCamera(defaultCamera);
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
