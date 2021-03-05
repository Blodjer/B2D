#include "B2D_pch.h"
#include "CharacterMovementSystem.h"
#include "Game/Component/SpriteComponent.h"

void CharacterMovementSystem::Update(float deltaTime)
{
    for (ComponentSlice slice : ComponentIterator(m_world))
    {
        CharacterMovementComponent& movement = slice.GetWrite<CharacterMovementComponent>();
        TransformComponent& transform = slice.GetWrite<TransformComponent>();

        if (movement.controlInputVector == TVec3::Zero)
            continue;

        transform.position += movement.controlInputVector * deltaTime;
        B2D_INFO("CharacterMovementSystem {} {}", transform.position.x , transform.position.z);
    }
}

