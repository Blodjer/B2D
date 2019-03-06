#pragma once
#include "Component.h"

struct TransformComponent : public Component
{
    TVec3 position = TVec3::Zero;
    TVec3 scale = TVec3::One;

    TMatrix matrix = TMatrix(1.0f);

    static constexpr uint16 MASK = 1;
};

