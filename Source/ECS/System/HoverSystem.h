#pragma once
#include "System.h"

class HoverSystem : public System
{
public:
    HoverSystem();

    virtual void Update(float deltaTime) override;
};

