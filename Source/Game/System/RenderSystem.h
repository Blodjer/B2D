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
