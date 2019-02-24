#pragma once

#include "Core/Core.h"

class CGameInstance;
class CInput;
class CInputComponent;
enum class EKey : int16;
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

	void ProcessInputKey(EKey key, EKeyEvent event);
	void ProcessInputAxis();

private:
	const uint32 mId;
	std::vector<CInputComponent*> mInputComponents;
};
