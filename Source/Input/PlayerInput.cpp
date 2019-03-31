#include "B2D_pch.h"
#include "PlayerInput.h"
/*
PlayerInput::PlayerInput()
{
}

PlayerInput::~PlayerInput()
{
	ClearAllBindings();

	if (mAssignedController != nullptr)
	{
		//mAssignedController->UnregisterInputComponent(this);
	}
}

void PlayerInput::BindController(LocalPlayer* const controller)
{
// 	if (mAssignedController != nullptr)
// 	{
// 		if (mAssignedController == controller)
// 			return;
// 
// 		mAssignedController->UnregisterInputComponent(this);
// 	}
// 
// 	mAssignedController = controller;
// 	mAssignedController->RegisterInputComponent(this);
}

void PlayerInput::BindKey(EKey key, EKeyEvent keyEvent, TKeyDelegate delegate)
{
	mKeyDelegates[SKeyBinding(key, keyEvent)] = delegate;
}

void PlayerInput::BindAxis(int axis, TAxisDelegate delegate)
{
	mAxisDelegates[axis] = delegate;
}

void PlayerInput::ClearAllBindings()
{
	mKeyDelegates.clear();
	mAxisDelegates.clear();
}

void PlayerInput::ProcessInputKey(EKey key, EKeyEvent keyEvent)
{
	auto keyBinding = SKeyBinding(key, keyEvent);
	if (mKeyDelegates.find(keyBinding) != mKeyDelegates.end())
	{
		mKeyDelegates[keyBinding]();
	}
}

void PlayerInput::ProcessInputAxis()
{

}
*/