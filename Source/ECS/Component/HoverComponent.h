#pragma once
#include "Component.h"

struct HoverComponent : public Component
{
    float amplitude = 1.0f;
    float speed = 1.0f;

    static constexpr uint16 MASK = 2;
};

