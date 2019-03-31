#include "B2D_pch.h"
#include "LocalPlayer.h"

LocalPlayer::LocalPlayer(uint32 id)
    : mId(id)
{

}

void LocalPlayer::AssignInput(InputDeviceID inputDeviceId)
{
    mAssignedInput.insert(inputDeviceId);
}

void LocalPlayer::DeassignInput(InputDeviceID inputDeviceId)
{
    mAssignedInput.erase(inputDeviceId);
}
