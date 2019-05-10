#pragma once
#include "Game/Core/System.h"

class HoverSystem : public System/*<
    Write<HoverComponent>,
    Write<TransformComponent>
>*/
{
    DECLARE_SYSTEM(HoverSystem);

public:
    virtual void Update(float deltaTime) override;
};

