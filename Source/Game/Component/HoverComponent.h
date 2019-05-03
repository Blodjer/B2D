#pragma once
#include "Game/Core/Component.h"

struct HoverComponent : public Component
{
    DECLARE_COMPONENT(HoverComponent, 2);

    float amplitude = 0.002f;
    float speed = 1.0f;
};

