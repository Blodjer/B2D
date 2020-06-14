#include "B2D_pch.h"
#include "LocalPlayer.h"

LocalPlayer::LocalPlayer(uint32 id)
    : m_id(id)
{

}

void LocalPlayer::AssignInput(InputDeviceID inputDeviceId)
{
    m_assignedInput.insert(inputDeviceId);
}

void LocalPlayer::DeassignInput(InputDeviceID inputDeviceId)
{
    m_assignedInput.erase(inputDeviceId);
}
