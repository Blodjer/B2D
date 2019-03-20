#pragma once
#include "Component.h"

struct TransformComponent : public Component
{
    DECLARE_COMPONENT(TransformComponent, 0);

    TVec3 position = TVec3::Zero;
    TVec3 scale = TVec3::One;

    TMatrix matrix = TMatrix(1.0f);
};

