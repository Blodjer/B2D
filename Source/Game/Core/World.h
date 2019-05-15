#pragma once

#include "Core/Core.h"

#include "Game/Core/Entity.h"
#include "Game/Core/Component.h"
#include "Game/Core/SystemAdmin.h"
#include "Graphics/RenderObject.h"
#include "Graphics/Renderer/WorldRenderDataInterface.h"

class CGameInstance;
class WorldRenderer;

template<typename... C>
class SystemEntityObject;

class World
{
public:
    World(CGameInstance* const owningGameInstance);
    ~World();

    void Tick(float deltaTime);

    template<class E>
    EntityID AddEntity();

    template<class SE, typename... Params>
    SE* AddSystemEntityObject(Params... p);

    template<class C, typename... Params>
    C& AddComponent(EntityID entity, Params... p);

    template<class S>
    FORCEINLINE void AddSystem() { mSystemAdmin.AddSystem<S>(); }

    template<class C>
    std::vector<C>& GetComponents() const;

public:
    CGameInstance* GetOwningGameInstance() const { return mOwningGameInstance; }

    WorldRenderDataInterface* GetWorldRenderDataInterface() { return mWorldRenderDataInterface; }
    WorldRenderDataInterface const* GetWorldRenderDataInterface() const { return mWorldRenderDataInterface; }

    SystemAdmin& GetSystemAdmin() { return mSystemAdmin; }

public:
    std::unordered_map<EntityID, Entity*> mEntities;

private:
    CGameInstance* const mOwningGameInstance;
    WorldRenderDataInterface* mWorldRenderDataInterface;

    SystemAdmin mSystemAdmin;
};

template<class E>
EntityID World::AddEntity()
{
    B2D_STATIC_ASSERT_TYPE(Entity, E);

    static uint32 id = 1;
    E* entity = new E(id);
    id++;

    mEntities.emplace(entity->GetID(), entity);

    return entity->GetID();
}

template<class SE, typename... Params>
SE* World::AddSystemEntityObject(Params... p)
{
    B2D_STATIC_ASSERT_TYPE(System, SE);
    B2D_STATIC_ASSERT_TYPE(Entity, SE);

    static uint32 id = 9999999;
    SE* se = new SE(id);
    id++;

    mEntities.emplace(se->GetID(), se);

    mSystemAdmin.AddSystem(se);

    se->Setup(p...);

    return se;
}

template<class C, typename... Params>
C& World::AddComponent(EntityID entity, Params... p)
{
    B2D_STATIC_ASSERT_TYPE(Component, C);

    Entity* realEntity = mEntities[entity];
    B2D_ASSERT(realEntity);

    C& c = GetComponents<C>().emplace_back(C(p...));
    c.owner = realEntity;

    realEntity->mComponents.emplace_back(&c);
    realEntity->mComponentMask |= C::MASK;

    std::sort(realEntity->mComponents.begin(), realEntity->mComponents.end(), [](Component const* a, Component const* b)
    {
        return a->GET_MASK() < b->GET_MASK();
    });

    return c;
}

template<class C>
std::vector<C>& World::GetComponents() const
{
    B2D_STATIC_ASSERT_TYPE(Component, C);

    static std::vector<C> components;
    components.reserve(100000);
    return components;
}
