#pragma once
#include "Game/Core/System.h"
#include "Game/Component/HoverComponent.h"
#include "Game/Component/TransformComponent.h"

class HoverSystem : public ISystem<
    Write<HoverComponent>,
    Write<TransformComponent>
>
{
    DECLARE_SYSTEM(HoverSystem, true);

protected:
    virtual void Update(float deltaTime) override;
};

