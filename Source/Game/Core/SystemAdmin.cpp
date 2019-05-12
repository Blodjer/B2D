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

void SystemAdmin::Tick(float deltaTime)
{
    PROFILE_GAME_SYSTEM_NEW_FRAME();
    
    static bool b = true;
    if (b)
    {
        B2D_CORE_INFO("-- System Dump --");
        for (System* const s : mSystems)
        {
            std::bitset<16> readbits(s->GetReadMask());
            std::bitset<16> writebits(s->GetWriteMask());

            B2D_CORE_INFO("{} {}\nREAD  {}\nWRITE {}", s->GetName(), s->IsMultithreaded(), readbits.to_string(), writebits.to_string());
        }

        B2D_CORE_INFO("-- Before --");
        for (System* const s : mSystems)
        {
            B2D_CORE_INFO("{}", s->GetName());
        }

        std::sort(mSystems.begin(), mSystems.end(), [](System const* a, System const* b)
        {
            bool conflict = (a->GetReadMask() & b->GetReadMask()) != 0;
            return !conflict && a->IsMultithreaded() && !b->IsMultithreaded();
        });

        B2D_CORE_INFO("-- After --");
        for (System* const s : mSystems)
        {
            B2D_CORE_INFO("{}", s->GetName());
        }

        b = false;
    }

//     for (System* const s : mSystems)
//     {
//         PROFILE_GAME_SYSTEM(s->GetName());
//         s->Update(deltaTime);
//     }
//     PROFILE_GAME_SYSTEM_END_FRAME();
//     return;

    std::vector<std::future<void>> tasks;

    std::atomic<uint16> currentRead = 0;
    std::atomic<uint16> currentWrite = 0;

    std::array<std::atomic<uint8>, sizeof(uint16) * 8> currentReads;
    for (std::atomic<uint8>& r : currentReads) { r = 0; }

    {
        PROFILE_GAME_SYSTEM("Wait overflow systems");
        for (auto const& task : mOverflowTasks)
        {
            task.wait();
        }
        mOverflowTasks.clear();
    }

    for (System* const s : mSystems)
    {
        uint16 const read = s->GetReadMask();
        uint16 const write = s->GetWriteMask();

        {
            //PROFILE_GAME_SYSTEM("WAIT");

            bool canRead = false;
            bool canWrite = false;
            do {
                canRead = (currentWrite & read) == 0;
                canWrite = true;
                for (uint16 i = 0; i < currentReads.size(); ++i)
                {
                    if ((read >> i) & (currentReads[i] != 0))
                    {
                        canWrite = false;
                        break;
                    }
                }
            } while (!canRead || !canWrite);
        }

        if (!s->IsMultithreaded())
        {
            PROFILE_GAME_SYSTEM(s->GetName());
            s->Update(deltaTime);
            continue;
        }

        currentRead |= read;
        currentWrite |= write;
        for (uint16 i = 0; i < currentReads.size(); ++i)
        {
            currentReads[i] += (read >> i) & 1;
        }

        if (s->GetReadMask() == 3)
        {
            mOverflowTasks.emplace_back(std::async(std::launch::async, [s, deltaTime]() {
                s->Update(deltaTime);
            }));
            continue;
        }

        tasks.emplace_back(std::async(std::launch::async, [this, &currentReads, &currentWrite, s, deltaTime]() {
            PROFILE_GAME_SYSTEM(s->GetName());
            s->Update(deltaTime);

            uint16 const read = s->GetReadMask();
            uint16 const write = s->GetWriteMask();

            for (uint16 i = 0; i < currentReads.size(); ++i)
            {
                currentReads[i] -= (read >> i) & 1;
            }
            currentWrite ^= write;

            //OnSystemFinished(s);
        }));
    }

    for (auto const& task : tasks)
    {
        task.wait();
    }

    PROFILE_GAME_SYSTEM_END_FRAME();
}

void SystemAdmin::OnSystemFinished(System* system)
{

}

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
