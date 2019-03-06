#include "B2D_pch.h"
#include "HoverSystem.h"

#include "ECS/Component/HoverComponent.h"
#include "ECS/Component/TransformComponent.h"

HoverSystem::HoverSystem()
{
}

void HoverSystem::Update(float deltaTime)
{
    static float f = 0;
    f += deltaTime;

    for (HoverComponent& h : ComponentItr<HoverComponent, TransformComponent>(mWorld))
    {
        TransformComponent& c = *h.Sibling<TransformComponent>();

        c.position.Y = UMath::Sin(f * h.speed) * h.amplitude;
        c.matrix = TMatrix::Translate(c.matrix, TVec3(0, c.position.Y, 0));
    }
}

