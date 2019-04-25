#pragma once
#include "Game/Core/System.h"
#include "Core/Thread.h"

class RenderSystem : public System
{
    DECLARE_SYSTEM(RenderSystem);

public:
    RenderSystem() = default;

    virtual void Update(float deltaTime) override;

};

class RenderThread : public Thread
{

public:
    virtual bool Init() override;

    virtual void Run() override;

    virtual void Stop() override;

};