#pragma once

#include "Component/Component.h"
#include "Input.h"

#include <map>

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
	void BindKey(EKey key, EKeyEvent keyEvent, TKeyDelegate delegate);
	void BindAxis(int axis, TAxisDelegate delegate);
	void ClearAllBindings();

private:
	void ProcessInputKey(EKey key, EKeyEvent keyEvent);
	void ProcessInputAxis();

private:
	CPlayerController* mAssignedController;

	struct SKeyBinding
	{
		EKey mKey;
		EKeyEvent mEvent;

		SKeyBinding(EKey key, EKeyEvent event)
			: mKey(key), mEvent(event) {}

		bool const operator==(const SKeyBinding& o) const
		{
			return mKey == o.mKey && mEvent == o.mEvent;
		}

		bool const operator<(const SKeyBinding& o) const
		{
			return std::tie(mKey, mEvent) < std::tie(o.mKey, o.mEvent);
			return mKey < o.mKey || (mKey == o.mKey && mEvent < o.mEvent);
		}
	};

	std::map<SKeyBinding, TKeyDelegate> mKeyDelegates;
	std::map<int, TAxisDelegate> mAxisDelegates;
};
