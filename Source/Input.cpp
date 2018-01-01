#include <SDL2\SDL.h>
#include "Input.h"
#include <algorithm>
#include "Debug.h"

Input::Input()
{

}

Input::~Input()
{

}

void Input::BeginNewFrame()
{
	this->KeysPressed.clear();
	this->KeysDown.clear();
	this->KeysReleased.clear();
}

void Input::KeyDownEvent(const SDL_Event & event)
{
	this->KeysPressed[event.key.keysym.scancode] = true;
	this->KeysDown[event.key.keysym.scancode] = true;
}

void Input::KeyUpEvent(const SDL_Event & event)
{
	this->KeysReleased[event.key.keysym.scancode] = true;
	this->KeysDown[event.key.keysym.scancode] = false;
}

bool Input::IsKeyPressed(SDL_Scancode key)
{
	return this->KeysPressed[key];
}

bool Input::IsKeyDown(SDL_Scancode key)
{
	return this->KeysDown[key];
}

bool Input::IsKeyReleased(SDL_Scancode key)
{
	return this->KeysReleased[key];
}
