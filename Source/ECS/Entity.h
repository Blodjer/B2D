#pragma once

#include "ECS/Component/Component.h"
#include <bitset>

class Entity
{
public:
    Entity(EntityID id);
    ~Entity();

    EntityID GetID() const { return mID; }

    bool Has(std::bitset<16> mask) const
    {
        return (mComponentMask & mask) == mask;
    }

public:
    EntityID mID;
    std::vector<std::pair<uint16, size_t>> mComponentIndex;
    std::vector<Component*> mComponents;
    std::bitset<16> mComponentMask;
};

