#include "B2D_pch.h"
#include "SystemAdmin.h"
#include "Game/Core/World.h"
#include "Game/Core/System.h"
#include <future>

SystemAdmin::SystemAdmin(World* const world)
    : m_world(world)
{

}

SystemAdmin::~SystemAdmin()
{

}

void SystemAdmin::AddSystem(System* system)
{
    system->m_world = m_world;
    m_systemsRaw.emplace_back(system);
    OptimizeSystemOrder();
}

void SystemAdmin::Tick(float deltaTime)
{
    PROFILE_GAME_SYSTEM_BEGIN_FRAME();
    
    std::vector<System*>& systems = m_useOptimizedSystems ? m_systemsOptimized : m_systemsRaw;

    if (!m_multithreaded)
    {
        for (System* const s : systems)
        {
            PROFILE_GAME_SYSTEM(s->GetName());
            s->Update(deltaTime);
        }
        PROFILE_GAME_SYSTEM_END_FRAME();
        return;
    }

    std::vector<std::future<void>> tasks;

    std::atomic<uint16> currentRead = 0;
    std::atomic<uint16> currentWrite = 0;

    std::array<std::atomic<uint8>, sizeof(uint16) * 8> currentReads;
    for (std::atomic<uint8>& r : currentReads) { r.store(0, std::memory_order_relaxed); }

    {
        PROFILE_GAME_SYSTEM_THREAD_OVERHEAD("Wait for overflow systems");
        for (auto const& task : m_overflowTasks)
        {
            task.wait();
        }
        m_overflowTasks.clear();
    }

    for (System* const s : systems)
    {
        uint16 const read = s->GetReadMask();
        uint16 const write = s->GetWriteMask();

        {
            //PROFILE_GAME_SYSTEM_THREAD_OVERHEAD("Dependency Block");

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

        // TODO: Check if this is the last system affecting these masks
        if (write == 0 && read == 3)
        {
            m_overflowTasks.emplace_back(std::async(std::launch::async, [s, deltaTime]() {
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
        }));
    }

    for (auto const& task : tasks)
    {
        task.wait();
    }

    PROFILE_GAME_SYSTEM_END_FRAME();
}

void SystemAdmin::OptimizeSystemOrder()
{
    m_systemsOptimized = m_systemsRaw;
    std::sort(m_systemsOptimized.begin(), m_systemsOptimized.end(), [](System const* a, System const* b)
    {
        bool conflict = (a->GetReadMask() & b->GetReadMask()) != 0;
        return !conflict && a->IsMultithreaded() && !b->IsMultithreaded();
    });
}
