#pragma once

#include "Component.h"

struct CharacterMovementComponent : public Component
{
    DECLARE_COMPONENT(CharacterMovementComponent, 4)

    TVec3 controlInputVector = TVec3::Zero;
};