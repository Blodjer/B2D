#pragma once
#include "Game/Core/Component.h"

struct HoverComponent : public Component
{
    DECLARE_COMPONENT(HoverComponent, 2);

    float amplitude = 0.2f;
    float speed = 1.0f;

    float origin = 0.0f;
    bool originSet = false;

    float time = 0.0f;
};

