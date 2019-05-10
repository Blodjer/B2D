#pragma once
#include "Game/Core/System.h"

#include "Game/Component/CharacterMovementComponent.h"
#include "Game/Component/TransformComponent.h"

class CharacterMovementSystem : public System/*<
    Write<CharacterMovementComponent>,
    Write<TransformComponent>
>*/
{
    DECLARE_SYSTEM(CharacterMovementSystem);

public:
    virtual void Update(float deltaTime) override;
};

