#pragma once
#include "Game/Core/Entity.h"
#include "Game/Core/System.h"

#define DECLARE_SYSTEMENTITY(entityname) \
public: \
    entityname(EntityID id) : SystemEntityObject(id) {}; \
private: \
    DECLARE_SYSTEM(entityname, false) \

template<typename... Components>
class SystemEntityObject : public ISystem<>, public Entity
{
public:
    SystemEntityObject(EntityID id) : Entity(id) {};
    ~SystemEntityObject() = default;

    void Setup()
    {
        AddComponent<Components...>();

        Initialize();
    }

    virtual void Initialize() = 0;

    template<typename C1, typename C2, typename... C>
    void AddComponent()
    {
        AddComponent<C1>();
        AddComponent<C2, C...>();
    }

    template<typename C>
    C& AddComponent()
    {
        C& c = m_world->AddComponent<C>(GetID());
        std::get<C*>(m_componentTuple) = &c;
        return c;
    }

    template<typename C>
    FORCEINLINE C& GetComponent() const
    {
        B2D_ASSERT(std::get<C*>(m_componentTuple));
        return *static_cast<C*>(std::get<C*>(m_componentTuple));
    }

private:
    static constexpr std::size_t NUMBER_OF_COMPONENTS = sizeof...(Components);
    std::tuple<Components*...> m_componentTuple;

};
