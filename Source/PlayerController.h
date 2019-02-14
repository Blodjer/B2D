#pragma once

#include <vector>

class CGameInstance;
class CInput;
class CInputComponent;
enum EKeyEvent;

class CPlayerController
{
	friend CGameInstance;
	friend CInputComponent;
	friend CInput;

	CPlayerController(int Id);
public:
	~CPlayerController();

	const int GetId() const { return mId; };

private:
	void RegisterInputComponent(CInputComponent* component);
	void UnregisterInputComponent(CInputComponent* component);

	void ProcessInputKey(int scancode, EKeyEvent event);
	void ProcessInputAxis();

private:
	const int mId;
	std::vector<CInputComponent*> mInputComponents;
};
