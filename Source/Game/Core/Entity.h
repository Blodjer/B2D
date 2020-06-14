#pragma once

#include "Game/Core/Component.h"
#include <bitset>

class Entity
{
public:
    Entity(EntityID id);
    ~Entity();

    EntityID GetID() const { return m_id; }

    FORCEINLINE bool Has(uint16 const mask) const
    {
        return (m_componentMask & mask) == mask;
    }

public:
    std::vector<Component*> m_components;
    uint16 m_componentMask = 0;

private:
    EntityID m_id;

};

