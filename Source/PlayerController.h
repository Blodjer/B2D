#pragma once

#include <vector>

class PlayerController
{
	friend class InputComponent;
	friend class GameInstance;

	PlayerController(int Id);
public:
	~PlayerController();

	const int GetId() const { return this->Id; };

private:
	void RegisterInputComponent(class InputComponent* component);
	void UnregisterInputComponent(class InputComponent* component);

	void ProcessInputKey(SDL_Scancode scancode, enum KeyEvent event);
	void ProcessInputAxis();

private:
	const int Id;
	std::vector<class InputComponent*> InputComponents;
};
