#pragma once
#include "Game/Core/Component.h"
#include "Game/Core/World.h"

#define DECLARE_SYSTEM(systemname) \
public: \
    static constexpr char const* const NAME = #systemname; \
    virtual char const* const GetName() override { return NAME; }; \
private:

struct AccessSpecefierBase
{
private:
    AccessSpecefierBase() = delete;
};

template<class T>
struct AccessSpecefier : AccessSpecefierBase
{
    using TYPE = T;
};

template<class T>
struct Read : AccessSpecefier<T>
{
    static constexpr bool WRITE = false;
};

template<class T>
struct Write : AccessSpecefier<T>
{
    static constexpr bool WRITE = true;
};

// template<class... TComponents>
// class System;
// 
// template<>
// class System<>
// {
// 
// };

//template<class T, class... TComponents>
class System//<T, TComponents...> : System<TComponents...>
{
    friend class SystemAdmin;

protected:
    System() = default;

    virtual void Update(float deltaTime) = 0;
// 
// protected:
//     template<class T>
//     static constexpr uint16 GetAccessMask(bool forWrite)
//     {
//         B2D_STATIC_ASSERT_TYPE(AccessSpecefierBase, T);
//         B2D_STATIC_ASSERT_TYPE(Component, T::TYPE);
// 
//         if (T::WRITE || forWrite == T::WRITE)
//         {
//             return T::TYPE::MASK;
//         }
// 
//         return 0;
//     }
// 
//     template<class TComponent1, class TComponent2, class... TComponents>
//     static constexpr uint16 GetAccessMask(bool forWrite)
//     {
//         return GetAccessMask<TComponent1>(forWrite) | GetAccessMask<TComponent2, TComponents...>(forWrite);
//     }
// 
// public:
//     static constexpr uint16 READ_MASK = GetAccessMask<TComponents...>(false);
//     static constexpr uint16 WRITE_MASK = GetAccessMask<TComponents...>(true);
// 
//     virtual uint16 GetReadMask() const { return READ_MASK; }
//     virtual uint16 GetWriteMask() const { return WRITE_MASK; }

//     template<class T>
//     T const& GetRead()
//     {
//         B2D_STATIC_ASSERT(T::MASK & READ_MASK, "Type is not registered!");
//         B2D_STATIC_ASSERT(!(T::MASK & WRITE_MASK), "Type is marked as writable! For optimizations please change the accessor if you only want to read.");
//         static T af;
//         return af;
//     }
// 
//     template<class T>
//     T& GetWrite()
//     {
//         B2D_STATIC_ASSERT(T::MASK & READ_MASK, "Type is not registered!");
//         B2D_STATIC_ASSERT(T::MASK & WRITE_MASK, "Cannot get component as writeable that is marked as read only!");
//         static T af;
//         return af;
//     }

public:
    virtual char const* const GetName() = 0;

protected:
    World* mWorld;
};


// TODO: Bitwise index calculation
// vector<Component*> (size 4)
//             
//      MASK 010111
//      COMP 000010
//      Bits before 
//      e.g. 16 (id: 4) Index in Vector: 0,1, -> 2 <-
//      log(x) * (1/log(2)) // or just save save the id as index AND pow2

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

    template<typename C>
    C* Sibling()
    {
        Entity const* const entity = *mIt;
        auto const& components = entity->mComponents;

        for (auto const& componentIndex : entity->mComponentIndex)
        {
            if (componentIndex.first == C::MASK)
            {
                return static_cast<C*>(components[componentIndex.first]);
            }
        }

        B2D_TRAPf("Sibling component not found. This function must not be called if the component is not the a sibling!");
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
