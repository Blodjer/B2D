#pragma once

#include "Core/Core.h"

namespace
{
    uint32 constexpr INVALID_ENTITY_ID = 0;
}

class EntityID
{
    friend struct std::hash<EntityID>;

public:
    EntityID() : mID(INVALID_ENTITY_ID) {}
    EntityID(uint32 id) : mID(id) {}

    bool EntityID::operator==(EntityID const& other) const
    {
        return mID == other.mID;
    }

private:
    uint32 mID;
};

namespace std
{
    template<>
    struct hash<EntityID>
    {
        size_t operator()(EntityID const& other) const
        {
            return hash<uint32>()(other.mID);
        }
    };
}