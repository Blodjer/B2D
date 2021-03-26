#include "B2D_pch.h"
#include "RenderSystem.h"

#include "Game/Component/SpriteComponent.h"
#include "Game/Component/TransformComponent.h"
#include "Game/Core/World.h"
#include "Graphics/RenderObject.h"

void RenderSystem::Update(float deltaTime)
{
    WorldRenderDataInterface* wrdi = m_world->GetWorldRenderDataInterface();
    if (wrdi == nullptr)
    {
        return;
    }

    wrdi->ClearAndSetRenderData([&](RenderObjectBuffer<QuadRenderObject>& buffer) {

        for (ComponentSlice slice : ComponentIterator(m_world))
        {
            buffer.Add(
                slice.GetRead<TransformComponent>().matrix,
                slice.GetRead<SpriteComponent>().material
            );
        }
    });
}
