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

class System
{
    friend class SystemAdmin;

public:
    virtual char const* const GetName() = 0;

    virtual uint16 GetReadMask() const = 0;
    virtual uint16 GetWriteMask() const = 0;

protected:
    virtual void Update(float deltaTime) = 0;

protected:
    World* mWorld;
};

template<class... TComponents>
class ISystem : public System
{
    friend class SystemAdmin;

protected:
    ISystem() = default;

protected:
    template<class T = void>
    static constexpr uint16 GetAccessMask(bool forWrite)
    {
        B2D_STATIC_ASSERT_TYPE(AccessSpecefierBase, T);
        B2D_STATIC_ASSERT_TYPE(Component, T::TYPE);

        if (T::WRITE || forWrite == T::WRITE)
        {
            return T::TYPE::MASK;
        }

        return 0;
    }

    template<>
    static constexpr uint16 GetAccessMask<>(bool forWrite) { return 0; }

    template<class T1, class T2, class... TRest>
    static constexpr uint16 GetAccessMask(bool forWrite)
    {
        return GetAccessMask<T1>(forWrite) | GetAccessMask<T2, TRest...>(forWrite);
    }

public:
    static constexpr uint16 READ_MASK = GetAccessMask<TComponents...>(false);
    static constexpr uint16 WRITE_MASK = GetAccessMask<TComponents...>(true);

    virtual uint16 GetReadMask() const override { return READ_MASK; }
    virtual uint16 GetWriteMask() const override { return WRITE_MASK; }
};

template<>
class ISystem<> : public System
{
public:
    static constexpr uint16 READ_MASK = 0;
    static constexpr uint16 WRITE_MASK = 0;

    virtual uint16 GetReadMask() const override { return 0; }
    virtual uint16 GetWriteMask() const override { return 0; }
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

    // Deprecated
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
