#include "B2D_pch.h"
#include "World.h"
#include "ECS/System/System.h"

World::World()
{

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
