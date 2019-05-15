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
    World* const mWorld = nullptr;
    std::vector<System*> mSystemsRaw;
    std::vector<System*> mSystemsOptimized;
    std::vector<std::future<void>> mOverflowTasks;

    bool mMultithreaded = true;
    bool mUseOptimizedSystems = true;
};

