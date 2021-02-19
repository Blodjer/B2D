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
    EntityID() : m_id(INVALID_ENTITY_ID) {}
    EntityID(uint32 id) : m_id(id) {}

    bool operator==(EntityID const& other) const
    {
        return m_id == other.m_id;
    }

private:
    uint32 m_id;
};

namespace std
{
    template<>
    struct hash<EntityID>
    {
        size_t operator()(EntityID const& other) const
        {
            return hash<uint32>()(other.m_id);
        }
    };
}