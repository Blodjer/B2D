#include "B2D_pch.h"
#include "ModuleManager.h"

ModuleManager::ModuleManager()
{

}

ModuleManager::~ModuleManager()
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
