#pragma once

#include "Core/Core.h"

#include "ECS/Entity.h"
#include "ECS/Component/Component.h"

class System;

class World
{
public:
    World();
    ~World();

    void Update(float deltaTime);

    template<class S>
    void AddSystem()
    {
        B2D_STATIC_ASSERT_TYPE(System, S);

        System* s = mSystems.emplace_back(new S());
        s->mWorld = this;
    }

    template<class E>
    EntityID AddEntity()
    {
        B2D_STATIC_ASSERT_TYPE(Entity, E);

        static uint32 id = 1;
        E* entity = new E(id);
        id++;

        mEntities.emplace(entity->GetID(), entity);

        return entity->GetID();
    }

    template<class C, typename... Args>
    C* AddComponent(EntityID entity, Args... a)
    {
        B2D_STATIC_ASSERT_TYPE(Component, C);
        
        C* c = &GetComponents<C>().emplace_back(C(a...));
        mEntities[entity]->mComponents.emplace_back(c);
        mEntities[entity]->mComponentMask |= C::MASK;
        mEntities[entity]->mComponentIndex.emplace_back(std::make_pair(C::MASK, mEntities[entity]->mComponents.size() - 1));
        c->owner = mEntities[entity];

        return c;
    }

    template<class C>
    std::vector<C>& GetComponents() const
    {
        B2D_STATIC_ASSERT_TYPE(Component, C);

        static std::vector<C> components;
        components.reserve(100000);
        return components;
    }

public:
    std::unordered_map<EntityID, Entity*, EntityID> mEntities;
    std::vector<System*> mSystems;

};

