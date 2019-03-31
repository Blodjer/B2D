#pragma once

#include "Game/Core/Component.h"
#include <bitset>

class Entity
{
public:
    Entity(EntityID id);
    ~Entity();

    EntityID GetID() const { return mID; }

    bool Has(uint16 const mask) const
    {
        return (mComponentMask.to_ulong() & mask) == mask;
    }

public:
    EntityID mID;
    std::vector<Component*> mComponents;
    std::bitset<16> mComponentMask;
};

