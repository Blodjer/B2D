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

template<class TPrimary, uint16 READ_MASK, uint16 WRITE_MASK>
class ComponentSlice
{
private:
    TPrimary& mPrimaryComponent;

public:
    ComponentSlice(TPrimary& primaryComponent)
        : mPrimaryComponent(primaryComponent)
    {

    }

    template<class T>
    FORCEINLINE T const& GetRead() const
    {
        B2D_STATIC_ASSERT_TYPE(Component, T);
        B2D_STATIC_ASSERT(T::MASK & READ_MASK, "Component is not registered!");
        B2D_STATIC_ASSERT(!(T::MASK & WRITE_MASK), "Component is marked as writable! Change the accessor if you only want to read.");
        return GetSibling<T>();
    }

    template<class T>
    FORCEINLINE T& GetWrite() const
    {
        B2D_STATIC_ASSERT_TYPE(Component, T);
        B2D_STATIC_ASSERT(T::MASK & READ_MASK, "Component is not registered!");
        B2D_STATIC_ASSERT(T::MASK & WRITE_MASK, "Cannot get component as writeable that is marked as read only!");
        return GetSibling<T>();
    }

    template<>
    FORCEINLINE TPrimary const& GetRead<TPrimary>() const
    {
        B2D_STATIC_ASSERT(!(TPrimary::MASK & WRITE_MASK), "Component is marked as writable! Change the accessor if you only want to read.");
        return mPrimaryComponent;
    }

    template<>
    FORCEINLINE TPrimary& GetWrite<TPrimary>() const
    {
        B2D_STATIC_ASSERT(TPrimary::MASK & WRITE_MASK, "Cannot get component as writeable that is marked as read only!");
        return mPrimaryComponent;
    }

private:
    template<class T>
    FORCEINLINE T& GetSibling() const
    {
        Entity const* const owner = mPrimaryComponent.owner;

        // TODO: enforce to always have an Transform component which can be returned immediately. (But at the same time, not all components should require a transform e.g. data/manager components)
        if constexpr (T::MASK == 1 && TransformComponent::MASK == 1)
        {
            return *static_cast<T* const>(*owner->mComponents._Myfirst());
        }

        size_t constexpr INVERTED_MASK = T::MASK - 1;

        size_t index = INVERTED_MASK & owner->mComponentMask;
        index = CountBits(index);

        Component* const c = owner->mComponents[index];
        B2D_ASSERT(dynamic_cast<T* const>(c));

        return *static_cast<T* const>(c);
    }

    FORCEINLINE static size_t CountBits(size_t mask) noexcept
    {
        constexpr char const* const _Bitsperbyte =
            "\0\1\1\2\1\2\2\3\1\2\2\3\2\3\3\4"
            "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
            "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
            "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
            "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
            "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
            "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
            "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
            "\1\2\2\3\2\3\3\4\2\3\3\4\3\4\4\5"
            "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
            "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
            "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
            "\2\3\3\4\3\4\4\5\3\4\4\5\4\5\5\6"
            "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
            "\3\4\4\5\4\5\5\6\4\5\5\6\5\6\6\7"
            "\4\5\5\6\5\6\6\7\5\6\6\7\6\7\7\x8";

        unsigned char const* _Ptr = &reinterpret_cast<unsigned char const&>(mask);
        unsigned char const* const _End = _Ptr + sizeof(mask);

        size_t _Val = 0;
        for (; _Ptr != _End; ++_Ptr)
            _Val += _Bitsperbyte[*_Ptr];

        return (_Val);
    }
};

template<class TPrimaryComponent, uint16 MASK>
class ComponentIterator
{
public:
    template<class TPrimaryComponent, uint16 MASK>
    struct itrrr
    {
        //using iterator_category = _Category;
        using value_type = TPrimaryComponent;
        //using difference_type = _Diff;
        using pointer = TPrimaryComponent*;
        using reference = TPrimaryComponent&;

        uint64 mIdx = 0;

        std::array<TPrimaryComponent, 100000>& mComponentList;
        uint64 const mSize = 0;

        itrrr(std::array<TPrimaryComponent, 100000>& componentList, uint64 size, uint64 idx)
            : mComponentList(componentList)
            , mSize(size)
            , mIdx(idx)
        {

        }

        itrrr& operator++()
        {
            ++mIdx;
            for (; mIdx < mSize; ++mIdx)
            {
                TPrimaryComponent const& c = mComponentList[mIdx];
                if (c.owner->Has(MASK))
                {
                    break;
                }
            }

            return *this;
        }

        TPrimaryComponent& operator*()
        {
            return mComponentList[mIdx];
        }

        TPrimaryComponent* operator->()
        {
            return &mComponentList[mIdx];
        }

        bool operator!=(itrrr const& other)
        {
            return mIdx != other.mIdx;
        }
    };

    std::array<TPrimaryComponent, 100000>& mComponentList;
    uint64 const mSize = 0;

public:
    ComponentIterator(World const* const world)
        : mComponentList(world->GetComponents<TPrimaryComponent>())
        , mSize(world->GetComponentIndex<TPrimaryComponent>())
    {
        
    }

    itrrr<TPrimaryComponent, MASK> begin() const
    {
        return itrrr<TPrimaryComponent, MASK>(mComponentList, mSize, 0);
    }

    itrrr<TPrimaryComponent, MASK> end() const
    {
        return itrrr<TPrimaryComponent, MASK>(mComponentList, mSize, mSize);
    }
};

template<class TPrimaryComponent = void, class... TComponents>
class ISystem : public System
{
    friend class SystemAdmin;

protected:
    ISystem() = default;

protected:
    template<class T>
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
    static constexpr uint16 GetAccessMask<void>(bool forWrite) { return 0; }

    template<class T1, class T2, class... TRest>
    static constexpr uint16 GetAccessMask(bool forWrite) { return GetAccessMask<T1>(forWrite) | GetAccessMask<T2, TRest...>(forWrite); }

public:
    static constexpr uint16 READ_MASK = GetAccessMask<TPrimaryComponent, TComponents...>(false);
    static constexpr uint16 WRITE_MASK = GetAccessMask<TPrimaryComponent, TComponents...>(true);

    virtual uint16 GetReadMask() const override { return READ_MASK; }
    virtual uint16 GetWriteMask() const override { return WRITE_MASK; }

    using ComponentSlice = const ComponentSlice<typename TPrimaryComponent::TYPE, READ_MASK, WRITE_MASK>;
    using ComponentIterator = ComponentIterator<typename TPrimaryComponent::TYPE, READ_MASK>;

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