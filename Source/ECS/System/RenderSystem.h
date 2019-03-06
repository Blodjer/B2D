#pragma once
#include "System.h"

class RenderSystem : public System
{
public:
    RenderSystem() = default;

    virtual void Update(float deltaTime) override;

};

