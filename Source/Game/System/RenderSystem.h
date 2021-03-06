#pragma once
#include "Game/Core/System.h"
#include "Game/Component/TransformComponent.h"
#include "Game/Component/SpriteComponent.h"

class RenderSystem : public ISystem<
    Read<SpriteComponent>,
    Read<TransformComponent>
>
{
    DECLARE_SYSTEM(RenderSystem, true);

protected:
    virtual void Update(float deltaTime) override;

};
