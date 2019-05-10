#include "B2D_pch.h"
#include "CharacterMovementSystem.h"

void CharacterMovementSystem::Update(float deltaTime)
{
    for (CharacterMovementComponent& movement : ComponentItr<CharacterMovementComponent, TransformComponent>(mWorld))
    {
        TransformComponent& transform = movement.Sibling<TransformComponent>();

        if (movement.controlInputVector == TVec3::Zero)
            return;

        transform.position += movement.controlInputVector * deltaTime;
        B2D_CORE_INFO("CharacterMovementSystem {} {}", transform.position.X , transform.position.Z);
    }
}

