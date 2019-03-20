#pragma once
#include "ECS/Component/Component.h"
#include "ECS/World.h"

#define DECLARE_SYSTEM(systemname) \
public: \
    static constexpr char const* const NAME = #systemname; \
    virtual char const* const GET_NAME() const override { return NAME; } \
private:

class System
{
public:
    System() = default;

    virtual void Update(float deltaTime) = 0;
    virtual char const* const GET_NAME() const = 0;

public:
    World* mWorld;
};

template<typename PrimComp, typename... SecoComps>
class ComponentItr
{
private:
    using iterator = typename std::vector<PrimComp>::iterator;
    using reverse_iterator = typename std::vector<PrimComp>::reverse_iterator;
    
    std::vector<PrimComp>& mComponentList;
    iterator mIt;
    iterator mEndIt;

public:
    ComponentItr(World const* const world)
        : mComponentList(world->GetComponents<PrimComp>())
    {
        reverse_iterator it = mComponentList.rbegin();
        for (; it != mComponentList.rend(); ++it)
        {
            PrimComp const& c = *it;
            if (c.owner->Has(MASK))
            {
                mEndIt = it.base();
                return;
            }
        }

        mEndIt = mComponentList.begin();
    }

    template<typename Comp>
    static constexpr uint16 GetBit()
    {
        return Comp::MASK;
    }

    template<typename Comp1, typename Comp2, typename... Comps>
    static constexpr uint16 GetBit()
    {
        return GetBit<Comp1>() | GetBit<Comp2, Comps...>();
    }

    static constexpr uint16 MASK = GetBit<PrimComp, SecoComps...>();

    iterator begin()
    {
        mIt = mComponentList.begin();
        return getNext();
    }

    void operator++()
    {
        ++mIt;
        getNext();
    }

    FORCEINLINE iterator getNext()
    {
        for (; mIt != end(); ++mIt)
        {
            PrimComp& c = *mIt;
            if (c.owner->Has(MASK))
            {
                return mIt;
            }
        }

        return end();
    }

    iterator end() const
    {
        return mEndIt;
    }

    template<typename Comp>
    Comp* Sibling()
    {
        Entity const* const entity = *mIt;
        auto const& components = entity->mComponents;

        for (auto const& componentIndex : entity->mComponentIndex)
        {
            if (componentIndex.first == Comp::MASK)
            {
                return static_cast<Comp*>(components[componentIndex.first]);
            }
        }

        B2D_BREAK();
        return nullptr;
    }

    PrimComp& operator*()
    {
        return *mIt;
    }

    bool operator!=(const ComponentItr<PrimComp, SecoComps...>& other)
    {
        return mIt != other.mIt;
    }
};
