#pragma once

#include <map>

#include "Component.h"
#include "Delegate.h"

DECLARE_DELEGATE(TKeyDelegate);
DECLARE_DELEGATE(TAxisDelegate, float);

class CPlayerController;

class CInputComponent : public CComponent
{
	friend CPlayerController;

public:
	CInputComponent(CGameObject* const owner);
	virtual ~CInputComponent();

	void BindController(CPlayerController* const controller);
	void BindKey(int key, EKeyEvent keyEvent, TKeyDelegate delegate);
	void BindAxis(int key, TAxisDelegate delegate);
	void ClearAllBindings();

private:
	void ProcessInputKey(int key, EKeyEvent keyEvent);
	void ProcessInputAxis();

private:
	CPlayerController* mAssignedController;

	struct SKeyBinding
	{
		int Scancode;
		EKeyEvent Event;

		SKeyBinding(int eScancode, EKeyEvent eKeyEvent)
			: Scancode(eScancode), Event(eKeyEvent) {}

		bool const operator==(const SKeyBinding& o) const
		{
			return Scancode == o.Scancode && Event == o.Event;
		}

		bool const operator<(const SKeyBinding& o) const
		{
			return std::tie(Scancode, Event) < std::tie(o.Scancode, o.Event);
			return Scancode < o.Scancode || (Scancode == o.Scancode && Event < o.Event);
		}
	};

	std::map<SKeyBinding, TKeyDelegate> mKeyDelegates;
	std::map<int, TAxisDelegate> mAxisDelegates;
};
