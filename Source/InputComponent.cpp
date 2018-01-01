#include <SDL2\SDL.h>
#include "Input.h"
#include "InputComponent.h"
#include "GameObject.h"
#include "PlayerController.h"
#include "Debug.h"

InputComponent::InputComponent(GameObject* pOwner) : IComponent(pOwner)
{
	
}

InputComponent::~InputComponent()
{
	this->ClearAllBindings();

	if (this->AssignedController != NULL)
	{
		this->AssignedController->UnregisterInputComponent(this);
	}
}

void InputComponent::BindController(PlayerController* pController)
{
	if (this->AssignedController != NULL)
	{
		if (this->AssignedController == pController)
			return;

		this->AssignedController->UnregisterInputComponent(this);
	}

	this->AssignedController = pController;
	this->AssignedController->RegisterInputComponent(this);
}

void InputComponent::BindKey(SDL_Scancode eKey, KeyEvent eKeyEvent, KeyDelegate delegate)
{
	this->KeyDelegates[KeyBinding(eKey, eKeyEvent)] = delegate;
}

void InputComponent::BindAxis(SDL_Scancode eKey, AxisDelegate delegate)
{
	this->AxisDelegates[eKey] = delegate;
}

void InputComponent::ClearAllBindings()
{
	this->KeyDelegates.clear();
	this->AxisDelegates.clear();
}

void InputComponent::ProcessInputKey(SDL_Scancode eKey, KeyEvent eKeyEvent)
{
	auto xKeyBinding = KeyBinding(eKey, eKeyEvent);
	if (this->KeyDelegates.find(xKeyBinding) != this->KeyDelegates.end())
	{
		this->KeyDelegates[xKeyBinding]();
	}
}

void InputComponent::ProcessInputAxis()
{

}
