#pragma once
#include "Game/Core/System.h"

#include "Game/Component/CharacterMovementComponent.h"
#include "Game/Component/TransformComponent.h"

class CharacterMovementSystem : public ISystem<
    Write<CharacterMovementComponent>,
    Write<TransformComponent>
>
{
    DECLARE_SYSTEM(CharacterMovementSystem);

protected:
    virtual void Update(float deltaTime) override;
};

