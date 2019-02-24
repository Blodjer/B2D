#include "B2D_pch.h"
#include "InputComponent.h"

#include "GameObject.h"
#include "PlayerController.h"

CInputComponent::CInputComponent(CGameObject* const owner)
	: CComponent(owner)
{
	
}

CInputComponent::~CInputComponent()
{
	ClearAllBindings();

	if (mAssignedController != nullptr)
	{
		mAssignedController->UnregisterInputComponent(this);
	}
}

void CInputComponent::BindController(CPlayerController* const controller)
{
	if (mAssignedController != nullptr)
	{
		if (mAssignedController == controller)
			return;

		mAssignedController->UnregisterInputComponent(this);
	}

	mAssignedController = controller;
	mAssignedController->RegisterInputComponent(this);
}

void CInputComponent::BindKey(EKey key, EKeyEvent keyEvent, TKeyDelegate delegate)
{
	mKeyDelegates[SKeyBinding(key, keyEvent)] = delegate;
}

void CInputComponent::BindAxis(int axis, TAxisDelegate delegate)
{
	mAxisDelegates[axis] = delegate;
}

void CInputComponent::ClearAllBindings()
{
	mKeyDelegates.clear();
	mAxisDelegates.clear();
}

void CInputComponent::ProcessInputKey(EKey key, EKeyEvent keyEvent)
{
	auto keyBinding = SKeyBinding(key, keyEvent);
	if (mKeyDelegates.find(keyBinding) != mKeyDelegates.end())
	{
		mKeyDelegates[keyBinding]();
	}
}

void CInputComponent::ProcessInputAxis()
{

}
