#pragma once

#include <vector>

class CPlayerController
{
	friend class CGameInstance;
	friend class CInputComponent;
	friend class CInput;

	CPlayerController(int Id);
public:
	~CPlayerController();

	const int GetId() const { return this->Id; };

private:
	void RegisterInputComponent(class CInputComponent* component);
	void UnregisterInputComponent(class CInputComponent* component);

	void ProcessInputKey(int scancode, enum EKeyEvent event);
	void ProcessInputAxis();

private:
	const int Id;
	std::vector<class CInputComponent*> InputComponents;
};
