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
    World* m_world;
};

template<class TPrimary, uint16 READ_MASK, uint16 WRITE_MASK>
class ComponentSlice
{
private:
    TPrimary& m_primaryComponent;

public:
    ComponentSlice(TPrimary& primaryComponent)
        : m_primaryComponent(primaryComponent)
    {

    }

    template<class T>
    FORCEINLINE T const& GetRead() const
    {
        B2D_STATIC_ASSERT_TYPE(Component, T);
        B2D_STATIC_ASSERT(T::MASK & READ_MASK, "Component is not registered!");
        B2D_STATIC_ASSERT(!(T::MASK & WRITE_MASK), "Component is set as writable! Change the accessor if you only want to read.");
        return GetSibling<T>();
    }

    template<class T>
    FORCEINLINE T& GetWrite() const
    {
        B2D_STATIC_ASSERT_TYPE(Component, T);
        B2D_STATIC_ASSERT(T::MASK & READ_MASK, "Component is not registered!");
        B2D_STATIC_ASSERT(T::MASK & WRITE_MASK, "Cannot get component as writeable that is set as read only!");
        return GetSibling<T>();
    }

    template<>
    FORCEINLINE TPrimary const& GetRead<TPrimary>() const
    {
        B2D_STATIC_ASSERT(!(TPrimary::MASK & WRITE_MASK), "Component is set as writable! Change the accessor if you only want to read.");
        return m_primaryComponent;
    }

    template<>
    FORCEINLINE TPrimary& GetWrite<TPrimary>() const
    {
        B2D_STATIC_ASSERT(TPrimary::MASK & WRITE_MASK, "Cannot get component as writeable that is set as read only!");
        return m_primaryComponent;
    }

private:
    template<class T>
    FORCEINLINE T& GetSibling() const
    {
        return m_primaryComponent.owner->Get<T>();
    }
};

template<class TPrimaryComponent, uint16 MASK>
class ComponentIterator
{
public:
    template<class TPrimaryComponent, uint16 MASK>
    struct itr
    {
        uint64 m_idx = 0;

        std::array<TPrimaryComponent, 100000>& m_componentList;
        uint64 const m_size = 0;

        itr(std::array<TPrimaryComponent, 100000>& componentList, uint64 size, uint64 idx)
            : m_componentList(componentList)
            , m_size(size)
            , m_idx(idx)
        {

        }

        itr& operator++()
        {
            ++m_idx;
            for (; m_idx < m_size; ++m_idx)
            {
                TPrimaryComponent const& component = m_componentList[m_idx];
                if (component.owner->Has(MASK))
                {
                    break;
                }
            }

            return *this;
        }

        TPrimaryComponent& operator*()
        {
            return m_componentList[m_idx];
        }

        TPrimaryComponent* operator->()
        {
            return &m_componentList[m_idx];
        }

        bool operator!=(itr const& other)
        {
            return m_idx != other.m_idx;
        }
    };

    std::array<TPrimaryComponent, 100000>& m_componentList;
    uint64 const m_size = 0;

public:
    ComponentIterator(World const* const world)
        : m_componentList(world->GetComponents<TPrimaryComponent>())
        , m_size(world->GetComponentIndex<TPrimaryComponent>())
    {
        
    }

    itr<TPrimaryComponent, MASK> begin() const
    {
        return itr<TPrimaryComponent, MASK>(m_componentList, m_size, 0);
    }

    itr<TPrimaryComponent, MASK> end() const
    {
        return itr<TPrimaryComponent, MASK>(m_componentList, m_size, m_size);
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