#pragma once
#include "Game/Core/Component.h"
#include "Game/Core/World.h"

// #define DECLARE_SYSTEM(systemname) DECLARE_SYSTEMM(systemname, false);
// #define DECLARE_SYSTEMM(systemname, multithread) \
// public: \
//     static constexpr char const* const NAME = #systemname; \
//     virtual char const* const GetName() const override { return NAME; } \
//     virtual bool IsMultithreaded() const { return multithread; } \
// private:

#define DECLARE_SYSTEM(systemname, multithread) \
public: \
    static constexpr char const* const NAME = #systemname; \
    virtual char const* const GetName() override { return NAME; } \
    static constexpr bool MULTITHREADED = multithread; \
    virtual bool IsMultithreaded() const override { return MULTITHREADED; } \
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
struct Read : public AccessSpecefier<T>
{
    static constexpr bool WRITE = false;
};

template<class T>
struct Write : public AccessSpecefier<T>
{
    static constexpr bool WRITE = true;
};

class System
{
    friend class SystemAdmin;

public:
    virtual char const* const GetName() = 0;
    virtual bool IsMultithreaded() const = 0;

    virtual uint16 GetReadMask() const = 0;
    virtual uint16 GetWriteMask() const = 0;

protected:
    virtual void Update(float deltaTime) = 0;

protected:
    World* mWorld;
};

// template<class TPrimary, uint16 READ_MASK, uint16 WRITE_MASK>
// class ComponentTuple
// {
// public:
//     template<class T>
//     T const& GetRead() const
//     {
//         B2D_STATIC_ASSERT_TYPE(Component, T);
//         B2D_STATIC_ASSERT(T::MASK & READ_MASK, "Type is not registered!");
//         B2D_STATIC_ASSERT(!(T::MASK & WRITE_MASK), "Type is marked as writable! For optimizations please change the accessor if you only want to read.");
//         static T af;
//         return af;
//     }
// 
//     template<>
//     TPrimary const& GetRead<TPrimary>() const
//     {
//         return *mPrimaryComponent;
//     }
// 
//     template<class T>
//     T& GetWrite() const
//     {
// 
//     }
// 
// private:
//     TPrimary::TYPE* mPrimaryComponent;
// };

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
    static constexpr uint16 GetAccessMask(bool forWrite) { return GetAccessMask<T1>(forWrite) | GetAccessMask<T2, TRest...>(forWrite); }

public:
    static constexpr uint16 READ_MASK = GetAccessMask<TComponents...>(false);
    static constexpr uint16 WRITE_MASK = GetAccessMask<TComponents...>(true);

    virtual uint16 GetReadMask() const override { return READ_MASK; }
    virtual uint16 GetWriteMask() const override { return WRITE_MASK; }

   // using Tpppple = ComponentTuple<TPrim, READ_MASK, WRITE_MASK>;

};

template<>
class ISystem<void> : public System
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
    using iterator = typename std::array<PrimComp, 100000>::iterator;

    std::array<PrimComp, 100000>& mComponentList;
    uint64 idx = 0;
    uint64 size = 0;

public:
    ComponentItr(World const* const world)
        : mComponentList(world->GetComponents<PrimComp>())
    {
        size = world->GetComponentIndex<PrimComp>();
    }

    template<typename Comp>
    static constexpr uint16 GetBit()
    {
        return Comp::MASK;
    }

    template<typename Comp1, typename Comp2, typename... Comps>
    static constexpr uint16 GetBit() { return GetBit<Comp1>() | GetBit<Comp2, Comps...>(); }

    static constexpr uint16 MASK = GetBit<PrimComp, SecoComps...>();

    iterator begin()
    {
        idx = 0;
        return mComponentList.begin();
    }

    void operator++()
    {
        ++idx;
        getNext();
    }

    FORCEINLINE uint64 getNext()
    {
        for (; idx < size; ++idx)
        {
            PrimComp const& c = mComponentList[idx];
            if (c.owner->Has(MASK))
            {
                return idx;
            }
        }

        return size;
    }

    iterator end() const
    {
        return mComponentList.begin() + size;
    }

    PrimComp& operator*()
    {
        return mComponentList[idx];
    }

    bool operator!=(ComponentItr<PrimComp, SecoComps...> const& other)
    {
        return idx != other.idx;
    }
};

// template<class... _Rest>
// class TestTuple;
// 
// template<>
// class TestTuple<>
// {
// public:
//     constexpr TestTuple() noexcept
//     {	// default construct
//     }
// 
//     constexpr TestTuple(const TestTuple&) noexcept	// TRANSITION, for binary compatibility
//     {	// copy construct
//     }
// };
// 
// // TODO: hold references only
// 
// template<class _This, class... _Rest>
// class TestTuple<_This, _Rest...> : private TestTuple<_Rest...>
// {
//     typename _This::TYPE thisData;
// };

// template<class... D>
// class SBase
// {
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
// };
