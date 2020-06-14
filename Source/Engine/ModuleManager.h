#pragma once
#include "EngineModule.h"

class ModuleManager final
{
    friend class GameEngine;

private:
    ModuleManager();
    ~ModuleManager();

    void ForwardBeginFrame();
    void ForwardTick(float deltaTime);
    void ForwardEndFrame();

public:
    template<class C>
    C* Load()
    {
        B2D_STATIC_ASSERT_TYPE(IEngineModule, C);

        C*& module = Get<C>();
        if (module)
        {
            return module;
        }

        module = new C();
        m_modules.emplace_back(module);

        static_cast<IEngineModule*>(module)->Init();

        return module;
    }

    template<class C>
    void Unload()
    {
        B2D_STATIC_ASSERT_TYPE(IEngineModule, C);

        C*& module = Get<C>();
        if (!module)
        {
            return;
        }

        Unload(module);
    }

    template<class C>
    C*& Get() const
    {
        B2D_STATIC_ASSERT_TYPE(IEngineModule, C);

        static C* module = nullptr;
        return module;
    }

private:
    void Unload(IEngineModule*& module)
    {
        module->Shutdown();

        m_modules.erase(std::remove(m_modules.begin(), m_modules.end(), module));

        delete module;

        module = nullptr;
    }

private:
    std::vector<IEngineModule*> m_modules;

};

