#include "B2D_pch.h"
#include "HoverSystem.h"

#include "Game/Component/HoverComponent.h"
#include "Game/Component/TransformComponent.h"

void HoverSystem::Update(float deltaTime)
{
    for (ComponentSlice slice : ComponentIterator(m_world))
    {
        HoverComponent& hover = slice.GetWrite<HoverComponent>();
        TransformComponent& transform = slice.GetWrite<TransformComponent>();

        if (!hover.originSet)
        {
            hover.origin = transform.position.y;
            hover.originSet = true;
        }

        hover.time += deltaTime;

        transform.position.y = hover.origin + UMath::Sin(hover.time * hover.speed) * hover.amplitude;
        transform.matrix.SetPosition(transform.position);
    }
}

