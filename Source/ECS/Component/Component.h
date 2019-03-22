#pragma once
#include "Math/BMath.h"

class World;
class Entity;

struct Component
{
public:
    Component() = default;
    virtual ~Component() = default;

    Entity const* owner;

    virtual uint16 GET_MASK() const = 0;

public:
    template<class C>
    C* Sibling() const
    {
        for (Component* const comp : owner->mComponents)
        {
            if (comp->GET_MASK() == C::MASK)
            {
                return static_cast<C*>(comp);
            }
        }

        B2D_TRAP("Sibling component not found. This function must not be called if the component is not the a sibling!");
        return nullptr;
    }
};

#define DECLARE_COMPONENT(name, id) \
    static constexpr uint16 MASK = UMath::Pow2(id); \
    virtual uint16 GET_MASK() const override { return MASK; }
