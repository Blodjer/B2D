#include "B2D_pch.h"
#include "World.h"
#include "ECS/System/System.h"
#include "Camera.h"

World::World()
{
    CCamera* defaultCamera = new CCamera();
    defaultCamera->MakeActive();
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
