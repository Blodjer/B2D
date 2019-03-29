#pragma once

#include "Core/Core.h"

#include "ECS/Entity.h"
#include "ECS/Component/Component.h"
#include "SystemEntityObject.h"

class CGameInstance;
class System;

template<typename... C>
class SystemEntityObject;

class World
{
public:
    World(CGameInstance* const owningGameInstance);
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

    template<class SE, typename... Params>
    SE* AddSystemEntityObject(Params... p)
    {
        B2D_STATIC_ASSERT_TYPE(System, SE);
        B2D_STATIC_ASSERT_TYPE(Entity, SE);

        static uint32 id = 9999999;
        SE* se = new SE(id);
        id++;

        mEntities.emplace(se->GetID(), se);

        System* s = mSystems.emplace_back(se);
        s->mWorld = this;

        se->Setup(p...);

        return se;
    }

    template<class C, typename... Params>
    C* AddComponent(EntityID entity, Params... p)
    {
        B2D_STATIC_ASSERT_TYPE(Component, C);
        
        C* c = &GetComponents<C>().emplace_back(C(p...));
        mEntities[entity]->mComponents.emplace_back(c);
        mEntities[entity]->mComponentMask |= C::MASK;
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
    CGameInstance* GetOwningGameInstance() const { return mOwningGameInstance; }

public:
    std::unordered_map<EntityID, Entity*> mEntities;
    std::vector<System*> mSystems;

private:
    CGameInstance* const mOwningGameInstance;

};

