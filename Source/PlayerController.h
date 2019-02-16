#pragma once

#include "Core/Core.h"

#include <vector>

class CGameInstance;
class CInput;
class CInputComponent;
enum class EKeyEvent : uint8;

class CPlayerController
{
	friend CGameInstance;
	friend CInputComponent;
	friend CInput;

	CPlayerController(uint32 Id);
public:
	~CPlayerController();

	const uint32 GetId() const { return mId; };

private:
	void RegisterInputComponent(CInputComponent* component);
	void UnregisterInputComponent(CInputComponent* component);

	void ProcessInputKey(int scancode, EKeyEvent event);
	void ProcessInputAxis();

private:
	const uint32 mId;
	std::vector<CInputComponent*> mInputComponents;
};
