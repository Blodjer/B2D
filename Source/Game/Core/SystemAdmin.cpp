#include "B2D_pch.h"
#include "SystemAdmin.h"
#include "Game/Core/World.h"
#include "Game/Core/System.h"
#include <future>

SystemAdmin::SystemAdmin(World* const world)
    : mWorld(world)
{

}

SystemAdmin::~SystemAdmin()
{

}

void SystemAdmin::AddSystem(System* system)
{
    system->mWorld = mWorld;
    mSystems.emplace_back(system);
}

template<class... _Rest>
class TestTuple;

template<>
class TestTuple<>
{
public:
    constexpr TestTuple() noexcept
    {	// default construct
    }

    constexpr TestTuple(const TestTuple&) noexcept	// TRANSITION, for binary compatibility
    {	// copy construct
    }
};

// TODO: hold references only

template<class _This, class... _Rest>
class TestTuple<_This, _Rest...> : private TestTuple<_Rest...>
{
    typename _This::TYPE thisData;
};

struct BASEC
{

};

struct AC : BASEC
{
    static constexpr uint16 MASK = 1;

    float aaaData = 0.5f;
};

struct TC : BASEC
{
    static constexpr uint16 MASK = 2;

    float x;
    float y;
};

struct SC : BASEC
{
    static constexpr uint16 MASK = 4;

    std::string name;
};

struct EC : BASEC
{
    static constexpr uint16 MASK = 8;

    int electron = 100;
};

#define FILL(...) \
    static constexpr uint16 READ_MASK2 = GetAccessMask<__VA_ARGS__>(false); \
    static constexpr uint16 WRITE_MASK2 = GetAccessMask<__VA_ARGS__>(true);

template<class... D>
class SBase
{
protected:
    template<class D>
    static constexpr uint16 GetAccessMask(bool forWrite)
    {
        B2D_STATIC_ASSERT_TYPE(AccessSpecefierBase, D);
        B2D_STATIC_ASSERT_TYPE(BASEC, D::TYPE);

        if (D::WRITE || forWrite == D::WRITE)
        {
            return D::TYPE::MASK;
        }
        else
        {
            return 0;
        }
    }

    template<class D1, class D2, class... D>
    static constexpr uint16 GetAccessMask(bool forWrite)
    {
        return GetAccessMask<D1>(forWrite) | GetAccessMask<D2, D...>(forWrite);
    }

public:
    static constexpr uint16 READ_MASK = GetAccessMask<D...>(false);
    static constexpr uint16 WRITE_MASK = GetAccessMask<D...>(true);

    TestTuple<D...> currentTupleData;

    template<class T>
    T const& GetRead()
    {
        B2D_STATIC_ASSERT(T::MASK & READ_MASK, "Type is not registered!");
        B2D_STATIC_ASSERT(!(T::MASK & WRITE_MASK), "Type is marked as writable! For optimizations please change the accessor if you only want to read.");
        static T af;
        return af;
    }

    template<class T>
    T& GetWrite()
    {
        B2D_STATIC_ASSERT(T::MASK & READ_MASK, "Type is not registered!");
        B2D_STATIC_ASSERT(T::MASK & WRITE_MASK, "Cannot get component as writeable that is marked as read only!");
        static T af;
        return af;
    }
};

class TestS : public SBase<
    Read<TC>,
    Read<SC>,
    Write<AC>,
    Write<EC>
>
{
    FILL(
        //Read<TC>,
        Read<SC>,
        Write<AC>,
        Write<EC>
    )

    bool b;
};

void SystemAdmin::Tick(float deltaTime)
{
     PROFILE_GAME_SYSTEM_NEW_FRAME();

     std::bitset<16> read(TestS::READ_MASK);
     std::bitset<16> write(TestS::WRITE_MASK);

     const uint32 nn = std::log(EC::MASK) / std::log(2);
     std::bitset<16> one(EC::MASK - 1);
     one = one & read;

     int o = one.to_ulong();
     int ib = std::log(o) / std::log(2);

     int index = one.count();
     
     TestS sss;
     SC const& aas = sss.GetRead<SC>();

     TestTuple<Write<TC>, Read<EC>> testttt;

//     for (System* const s : mSystems)
//     {
//         PROFILE_GAME_SYSTEM(s->GetName());
//         s->Update(deltaTime);
//     }

    static std::vector<std::future<void>> futures;
    if (futures.empty())
    {
        futures.reserve(mSystems.size());
        futures.resize(mSystems.size());
    }

    int ii = 0;
    for (System* s : mSystems)
    {
        futures[ii++] = std::async(ii > 3 ? std::launch::async : std::launch::deferred, [s, deltaTime]() {
            PROFILE_GAME_SYSTEM(s->GetName());
            s->Update(deltaTime);
        });
    }

    int i = 0;
    for (auto const& f : futures)
    {
        f.wait();
        i++;
        if (i >= 4)
            break;
    }

    PROFILE_GAME_SYSTEM_END_FRAME();
}
