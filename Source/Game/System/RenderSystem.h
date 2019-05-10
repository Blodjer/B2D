#pragma once
#include "Game/Core/System.h"

class RenderSystem : public System/*<
    Read<TransformComponent>,
    Read<SpriteComponent>
>*/
{
    DECLARE_SYSTEM(RenderSystem);

public:
    virtual void Update(float deltaTime) override;

};
