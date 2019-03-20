#pragma once
#include "Component.h"

struct HoverComponent : public Component
{
    DECLARE_COMPONENT(HoverComponent, 2);

    float amplitude = 0.001f;
    float speed = 1.0f;
};

