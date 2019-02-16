#pragma once

#include "Core/Core.h"

#include <map>
#include <vector>

class CGameEngine;
struct GLFWwindow;

enum class EKeyEvent : uint8
{
	KEY_UP,
	KEY_DOWN
};

class CInput
{
	friend CGameEngine;

public:
	CInput();
	~CInput();

private:
	static void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);

};

