#pragma once
#include "EngineModule.h"

class ModuleManager final
{
    friend class GameEngine;

private:
    ModuleManager() = default;
    ~ModuleManager() = default;

    void Init();
    void Shutdown();

    void ForwardBeginFrame();
    void ForwardTick(float deltaTime);
    void ForwardEndFrame();

public:
    template<class C>
    C* Load()
    {
        B2D_STATIC_ASSERT_TYPE(IEngineModule, C);

        C*& module = GetInternal<C>();
        if (module)
        {
            return module;
        }

        module = new C();

        if (!static_cast<IEngineModule*>(module)->Init())
        {
            B2D_TRAPf("Failed to load \"{}\"", module->GetName());
            delete module;
            module = nullptr;
            return nullptr;
        }

        m_modules.emplace_back(module);

        return module;
    }

    template<class C>
    void Unload()
    {
        B2D_STATIC_ASSERT_TYPE(IEngineModule, C);

        C*& module = GetInternal<C>();
        if (!module)
        {
            return;
        }

        Unload(module);
        module = nullptr;
    }

    template<class C>
    C* Get() const
    {
        B2D_STATIC_ASSERT_TYPE(IEngineModule, C);

        return GetInternal<C>();
    }

private:
    template<class C>
    C*& GetInternal() const
    {
        B2D_STATIC_ASSERT_TYPE(IEngineModule, C);

        static C* module = nullptr;
        return module;
    }

    void Unload(IEngineModule* module);

private:
    std::vector<IEngineModule*> m_modules;

};

