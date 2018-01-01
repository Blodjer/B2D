#include <SDL2\SDL.h>
#include "PlayerController.h"
#include <algorithm>
#include "InputComponent.h"
#include "Debug.h"

PlayerController::PlayerController(int Id) : Id(Id)
{

}

PlayerController::~PlayerController()
{

}

void PlayerController::RegisterInputComponent(InputComponent* pComponent)
{
	this->InputComponents.push_back(pComponent);
}

void PlayerController::UnregisterInputComponent(InputComponent* pComponent)
{
	this->InputComponents.erase(std::remove(this->InputComponents.begin(), this->InputComponents.end(), pComponent), this->InputComponents.end());
}

void PlayerController::ProcessInputKey(SDL_Scancode eScancode, KeyEvent eEvent)
{
	for (auto pInputComponent : this->InputComponents)
	{
		pInputComponent->ProcessInputKey(eScancode, eEvent);
	}
}

void PlayerController::ProcessInputAxis()
{
	for (auto pInputComponent : this->InputComponents)
	{
		pInputComponent->ProcessInputAxis();
	}
}
