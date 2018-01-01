#pragma once

#include <map>
#include "IComponent.h"
#include "Delegate.h"

DECLARE_DELEGATE(KeyDelegate);
DECLARE_DELEGATE(AxisDelegate, float);

class InputComponent : public IComponent
{
	friend class PlayerController;

public:
	InputComponent(GameObject* pOwner);
	virtual ~InputComponent();

	void BindController(class PlayerController* pController);
	void BindKey(SDL_Scancode eKey, KeyEvent eKeyEvent, KeyDelegate delegate);
	void BindAxis(SDL_Scancode eKey, AxisDelegate delegate);
	void ClearAllBindings();

private:
	void ProcessInputKey(SDL_Scancode eKey, KeyEvent eKeyEvent);
	void ProcessInputAxis();

private:
	PlayerController* AssignedController;

	struct KeyBinding
	{
		SDL_Scancode Scancode;
		KeyEvent Event;

		KeyBinding(SDL_Scancode eScancode, KeyEvent eKeyEvent)
			: Scancode(eScancode), Event(eKeyEvent) {}

		bool const operator==(const KeyBinding& o) const
		{
			return Scancode == o.Scancode && Event == o.Event;
		}

		bool const operator<(const KeyBinding& o) const
		{
			return std::tie(Scancode, Event) < std::tie(o.Scancode, o.Event);
			return Scancode < o.Scancode || (Scancode == o.Scancode && Event < o.Event);
		}
	};

	std::map<KeyBinding, KeyDelegate> KeyDelegates;
	std::map<SDL_Scancode, AxisDelegate> AxisDelegates;
};
