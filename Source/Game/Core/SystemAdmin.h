#pragma once
#include <future>

class System;
class World;

class SystemAdmin final
{
    friend class World;

protected:
    SystemAdmin(World* const world);
    ~SystemAdmin();

public:
    template<class S>
    void AddSystem()
    {
        B2D_STATIC_ASSERT_TYPE(System, S);
        AddSystem(new S());
    }

    void AddSystem(System* system);

    void Tick(float deltaTime);

private:
    void OnSystemFinished(System* system);

 private:
     World* const mWorld;
 
     std::vector<System*> mSystems;

     std::vector<std::future<void>> mOverflowTasks;

};

