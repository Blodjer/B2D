#include "B2D_pch.h"
#include "ModuleManager.h"

ModuleManager::ModuleManager()
{

}

ModuleManager::~ModuleManager()
{
    for (IEngineModule*& module : mModules)
    {
        Unload(module);
    }
}

void ModuleManager::ForwardBeginFrame()
{
    for (IEngineModule* const module : mModules)
    {
        module->BeginFrame();
    }
}

void ModuleManager::ForwardTick(float deltaTime)
{
    for (IEngineModule* const module : mModules)
    {
        module->Tick(deltaTime);
    }
}

void ModuleManager::ForwardEndFrame()
{
    for (IEngineModule* const module : mModules)
    {
        module->EndFrame();
    }
}
