#pragma once

class IEngineModule
{
    friend class ModuleManager;

protected:
    IEngineModule() = default;
    ~IEngineModule() = default;

private:
    IEngineModule(IEngineModule const&) = delete;
    IEngineModule& operator=(IEngineModule const&) = delete;

protected:
    virtual bool Init() = 0;
    virtual void Shutdown() = 0;

    virtual void BeginFrame() = 0;
    virtual void Tick(float deltaTime) = 0;
    virtual void EndFrame() = 0;

public:
    virtual std::string GetName() = 0;

};
