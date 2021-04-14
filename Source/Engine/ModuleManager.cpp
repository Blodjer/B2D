#include "B2D_pch.h"
#include "ModuleManager.h"

void ModuleManager::Init()
{

}

void ModuleManager::Shutdown()
{
    for (IEngineModule*& module : m_modules)
    {
        Unload(module);
    }
}

void ModuleManager::ForwardBeginFrame()
{
    for (IEngineModule* const module : m_modules)
    {
        module->BeginFrame();
    }
}

void ModuleManager::ForwardTick(float deltaTime)
{
    for (IEngineModule* const module : m_modules)
    {
        module->Tick(deltaTime);
    }
}

void ModuleManager::ForwardEndFrame()
{
    for (IEngineModule* const module : m_modules)
    {
        module->EndFrame();
    }
}

void ModuleManager::Unload(IEngineModule* module)
{
    module->Shutdown();

    m_modules.erase(std::remove(m_modules.begin(), m_modules.end(), module));

    delete module;
}
