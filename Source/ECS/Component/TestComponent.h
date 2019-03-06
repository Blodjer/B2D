#pragma once
#include "Component.h"

struct TestComponent : Component
{
    TestComponent() {};
    ~TestComponent() {};

    float fdata = 0.0f;

    static constexpr uint16 MASK = 8;
};

