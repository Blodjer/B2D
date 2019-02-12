#include "PlayerController.h"
#include <algorithm>
#include "InputComponent.h"
#include "Debug.h"

CPlayerController::CPlayerController(int Id) : Id(Id)
{

}

CPlayerController::~CPlayerController()
{

}

void CPlayerController::RegisterInputComponent(CInputComponent* pComponent)
{
	this->InputComponents.push_back(pComponent);
}

void CPlayerController::UnregisterInputComponent(CInputComponent* pComponent)
{
	this->InputComponents.erase(std::remove(this->InputComponents.begin(), this->InputComponents.end(), pComponent), this->InputComponents.end());
}

void CPlayerController::ProcessInputKey(int eScancode, EKeyEvent eEvent)
{
	for (auto pInputComponent : this->InputComponents)
	{
		pInputComponent->ProcessInputKey(eScancode, eEvent);
	}
}

void CPlayerController::ProcessInputAxis()
{
	for (auto pInputComponent : this->InputComponents)
	{
		pInputComponent->ProcessInputAxis();
	}
}
