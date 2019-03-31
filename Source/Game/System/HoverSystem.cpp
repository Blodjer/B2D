#include "B2D_pch.h"
#include "HoverSystem.h"

#include "Game/Component/HoverComponent.h"
#include "Game/Component/TransformComponent.h"

HoverSystem::HoverSystem()
{
}

void HoverSystem::Update(float deltaTime)
{
    static float f = 0;
    f += deltaTime;

    for (HoverComponent const& hover : ComponentItr<HoverComponent, TransformComponent>(mWorld))
    {
        TransformComponent& transform = hover.Sibling<TransformComponent>();

        transform.position.Y = UMath::Sin(f * hover.speed) * hover.amplitude;
        transform.matrix = TMatrix::Translate(transform.matrix, TVec3(0, transform.position.Y, 0));
    }
}

