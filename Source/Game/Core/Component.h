#pragma once
#include "Math/BMath.h"

class World;
class Entity;

#define DECLARE_COMPONENT(name, id) \
public: \
    static constexpr uint16 MASK = UMath::Pow2(id); \
protected: \
    virtual uint16 GET_MASK() const override { return MASK; } \
public:

struct Component
{
public:
    Component() = default;
    virtual ~Component() = default;

protected:
    virtual uint16 GET_MASK() const = 0;

public:
    Entity const* owner;

    template<class C>
    C& Sibling() const
    {
        for (Component* const comp : owner->mComponents)
        {
            if (comp->GET_MASK() == C::MASK)
            {
                return *static_cast<C*>(comp);
            }
        }

        B2D_TRAPf("Sibling component not found. This function must not be called if the component is not the a sibling!");
    }
};
