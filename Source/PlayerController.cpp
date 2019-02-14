#include "PlayerController.h"
#include <algorithm>
#include "InputComponent.h"
#include "Debug.h"

CPlayerController::CPlayerController(int id) : mId(id)
{

}

CPlayerController::~CPlayerController()
{

}

void CPlayerController::RegisterInputComponent(CInputComponent* component)
{
	mInputComponents.push_back(component);
}

void CPlayerController::UnregisterInputComponent(CInputComponent* component)
{
	mInputComponents.erase(std::remove(mInputComponents.begin(), mInputComponents.end(), component), mInputComponents.end());
}

void CPlayerController::ProcessInputKey(int scancode, EKeyEvent event)
{
	for (CInputComponent* const inputComponent : mInputComponents)
	{
		inputComponent->ProcessInputKey(scancode, event);
	}
}

void CPlayerController::ProcessInputAxis()
{
	for (CInputComponent* const inputComponent : mInputComponents)
	{
		inputComponent->ProcessInputAxis();
	}
}
