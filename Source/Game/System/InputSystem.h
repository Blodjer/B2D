#pragma once
#include "Game/Core/System.h"
#include "Game/Component/InputComponent.h"

class InputSystem : public ISystem<
    Write<InputComponent>
>
{
    DECLARE_SYSTEM(InputSystem, false);

protected:
    virtual void Update(float deltaTime) override;
};

