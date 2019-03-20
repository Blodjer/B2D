#pragma once
#include "ECS/System/System.h"
#include "ECS/Entity.h"

template<typename... Components>
class SystemEntityObject : public System, public Entity
{
public:
    SystemEntityObject(EntityID id) : Entity(id)
    {
    }

    ~SystemEntityObject() = default;

    void Setup()
    {
        AddComponent<Components...>();

        Initialize();
    }

    virtual void Initialize() {};

    template<typename Component1, typename Component2, typename... Components>
    void AddComponent()
    {
        AddComponent<Component1>();
        AddComponent<Component2, Components...>();
    }

    template<typename C>
    C* AddComponent()
    {
        std::get<C*>(mComponentTuple) = mWorld->AddComponent<C>(GetID());
        return std::get<C*>(mComponentTuple);
    }

    template<typename C>
    FORCEINLINE C* GetComponent() const
    {
        return static_cast<C*>(std::get<C*>(mComponentTuple));
    }

private:
    static constexpr std::size_t NUMBER_OF_COMPONENTS = sizeof...(Components);
    std::tuple<Components*...> mComponentTuple;

};
