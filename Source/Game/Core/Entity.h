#pragma once

#include "Game/Core/Component.h"

class Entity
{
    friend class World;

public:
    Entity(EntityID id);
    ~Entity();

    EntityID GetID() const { return m_id; }

    FORCEINLINE bool Has(uint16 const mask) const
    {
        return (m_componentMask & mask) == mask;
    }

    template<class C>
    FORCEINLINE bool Has() const
    {
        B2D_STATIC_ASSERT_TYPE(Component, C);
        return (m_componentMask & C::MASK) > 0;
    }

    template<class C>
    FORCEINLINE C& Get() const
    {
        B2D_STATIC_ASSERT_TYPE(Component, C);

        if constexpr (C::MASK == 1)
        {
            return *static_cast<C*>(*m_components._Myfirst());
        }

        size_t const idx = GetIndexInMask<C>(m_componentMask);
        Component* component = m_components[idx];

#ifdef B2D_BUILD_DEBUG
        B2D_ASSERT(dynamic_cast<C*>(component));
#endif
        return *static_cast<C*>(component);
    }

private:
    template<class C>
    void RegisterComponent(C& component)
    {
        B2D_STATIC_ASSERT_TYPE(Component, C);
        
        if (C::MASK > m_componentMask)
        {
            m_components.emplace_back(&component);
        }
        else
        {
            size_t const idx = GetIndexInMask<C>(m_componentMask);
            m_components.emplace(m_components.begin() + idx, &component);
        }

        m_componentMask |= C::MASK;
    }

    template<class C>
    FORCEINLINE static size_t GetIndexInMask(uint16 const mask)
    {
        B2D_STATIC_ASSERT_TYPE(Component, C);

        uint16 constexpr reversedMask = C::MASK - 1;
        uint16 const trimmedMask = reversedMask & mask;
        size_t const idx = CountBits(trimmedMask);
        return idx;
    }

private:
    std::vector<Component*> m_components;
    uint16 m_componentMask = 0;

    EntityID const m_id;

};

