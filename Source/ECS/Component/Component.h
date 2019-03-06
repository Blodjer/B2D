#pragma once

class World;
class Entity;

struct Component
{
public:
    Component() = default;
    virtual ~Component() = default;

    Entity* owner;

    static constexpr uint16 MASK = 0;

public:
    template<class C>
    C* Sibling() const
    {
        std::vector<Component*> const& v = owner->mComponents;

        for (std::pair<uint16, size_t> const& p : owner->mComponentIndex)
        {
            if (p.first == C::MASK)
            {
                return static_cast<C*>(v[p.second]);
            }
        }

        B2D_BREAK();
        return nullptr;
    }
};

