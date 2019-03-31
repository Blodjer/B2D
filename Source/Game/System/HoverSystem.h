#pragma once
#include "Game/Core/System.h"

class HoverSystem : public System
{
    DECLARE_SYSTEM(HoverSystem);

public:
    HoverSystem();

    virtual void Update(float deltaTime) override;
};

