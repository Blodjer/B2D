#pragma once

#include "Component.h"

class CPlayerController;

struct PawnComponent : public Component
{
    DECLARE_COMPONENT(PawnComponent, 5);

    CPlayerController* controller; // TODO: Replace by generic Controller that is used by the player and ai
};