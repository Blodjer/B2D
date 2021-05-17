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
    FORCEINLINE void AddSystem() { m_systemAdmin.AddSystem<S>(); }

    template<class C>
    std::array<C, 100000>& GetComponents() const;

    template<class C>
    uint64& GetComponentIndex() const;

public:
    CGameInstance* GetOwningGameInstance() const { return m_owningGameInstance; }

    WorldRenderDataInterface* GetWorldRenderDataInterface() { return m_worldRenderDataInterface; }
    WorldRenderDataInterface const* GetWorldRenderDataInterface() const { return m_worldRenderDataInterface; }

    SystemAdmin& GetSystemAdmin() { return m_systemAdmin; }

private:
    CGameInstance* const m_owningGameInstance;
    WorldRenderDataInterface* m_worldRenderDataInterface;

    std::unordered_map<EntityID, Entity*> m_entities;
    SystemAdmin m_systemAdmin;
};

template<class E>
EntityID World::AddEntity()
{
    B2D_STATIC_ASSERT_TYPE(Entity, E);

    static uint32 id = 1;
    E* entity = new E(id);
    id++;

    m_entities.emplace(entity->GetID(), entity);

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

    m_entities.emplace(se->GetID(), se);

    m_systemAdmin.AddSystem(se);

    se->Setup(p...);

    return se;
}

template<class C, typename... Params>
C& World::AddComponent(EntityID entityId, Params... p)
{
    B2D_STATIC_ASSERT_TYPE(Component, C);

    Entity* const entity = m_entities[entityId];
    B2D_ASSERT(entity);

    if (B2D_CHECK_f(entity->Has<C>(), "Entity already has a component of type {0}! No new component will be added.", C::NAME))
    {
        return entity->Get<C>();
    }

    uint64& i = GetComponentIndex<C>();

    C& c = GetComponents<C>()[i];
    c = C(p...);
    c.owner = entity;
    entity->RegisterComponent(c);
    
    i++;

    return c;
}

template<class C>
std::array<C, 100000>& World::GetComponents() const
{
    B2D_STATIC_ASSERT_TYPE(Component, C);

    static std::array<C, 100000> components;
    return components;
}

template<class C>
uint64& World::GetComponentIndex() const
{
    static uint64 i = 0;
    return i;
}
