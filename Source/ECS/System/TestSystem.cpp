#include "B2D_pch.h"
#include "TestSystem.h"

#include "ECS/Component/TestComponent.h"
#include "ECS/Component/TransformComponent.h"

void TestSystem::Update(float deltaTime)
{
    for (TestComponent& t : ComponentItr<TestComponent, TransformComponent>(mWorld))
    {
        t.fdata += deltaTime;
    }
}
