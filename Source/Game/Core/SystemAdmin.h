#pragma once
#include <future>

class System;
class World;

class SystemAdmin final
{
    friend class World;
    friend class GameSystemView;

protected:
    SystemAdmin(World* const world);
    ~SystemAdmin();

    void AddSystem(System* system);

    template<class S>
    void AddSystem()
    {
        B2D_STATIC_ASSERT_TYPE(System, S);
        AddSystem(new S());
    }

    void Tick(float deltaTime);

private:
    FORCEINLINE void TickSingleThread(float const deltaTime);
    FORCEINLINE void TickMultiThread(float const deltaTime);

    FORCEINLINE void TickSystem(float const deltaTime);

    void OptimizeSystemOrder();

private:
    World* const m_world = nullptr;
    std::vector<System*> m_systemsRaw;
    std::vector<System*> m_systemsOptimized;
    std::vector<std::future<void>> m_overflowTasks;

    bool m_multithreaded = true;
    bool m_useOptimizedSystems = true;
};

