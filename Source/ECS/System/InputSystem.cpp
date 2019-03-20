#include "B2D_pch.h"
#include "InputSystem.h"

#include "ECS/Component/TransformComponent.h"

InputSystem::InputSystem()
{
}

void InputSystem::Update(float deltaTime)
{
    for (TransformComponent& h : ComponentItr<TransformComponent>(mWorld))
    {
        
    }
}

