#pragma once

#include "Game/Core/Component.h"
#include <bitset>

class Entity
{
public:
    Entity(EntityID id);
    ~Entity();

    EntityID GetID() const { return mID; }

    FORCEINLINE bool Has(uint16 const mask) const
    {
        return (mComponentMask & mask) == mask;
    }

public:
    std::vector<Component*> mComponents;
    uint16 mComponentMask = 0;

private:
    EntityID mID;

};

