#pragma once

#include "Core/Core.h"

namespace
{
    uint32 constexpr INVALID_ENTITY_ID = 0;
}

class EntityID
{
public:
    EntityID()
        : mID(INVALID_ENTITY_ID)
    {

    }

    EntityID(uint32 id)
        : mID(id)
    {

    }

    size_t operator()(const EntityID& other) const
    {
        return std::hash<int>()(other.mID);
    }

    bool EntityID::operator==(EntityID const& other) const
    {
        return mID == other.mID;
    }

private:
    uint32 mID;
};