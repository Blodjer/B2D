#pragma once

#include <map>
#include <vector>

enum EKeyEvent
{
	KEY_UP,
	KEY_DOWN
};

class CInput
{
	friend class CGameEngine;

public:
	CInput();
	~CInput();

private:
	static void OnKey(struct GLFWwindow* window, int key, int scancode, int action, int mods);

};

