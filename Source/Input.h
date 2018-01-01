#pragma once

#include <map>
#include <vector>

enum KeyEvent
{
	KEY_UP,
	KEY_DOWN
};

class Input
{
	friend class GameEngine;

public:
	Input();
	~Input();

	bool IsKeyPressed(SDL_Scancode key);
	bool IsKeyDown(SDL_Scancode key);
	bool IsKeyReleased(SDL_Scancode key);

private:
	void BeginNewFrame();
	void KeyDownEvent(const SDL_Event& event);
	void KeyUpEvent(const SDL_Event& event);

private:
	std::map<SDL_Scancode, bool> KeysPressed;
	std::map<SDL_Scancode, bool> KeysDown;
	std::map<SDL_Scancode, bool> KeysReleased;
};

