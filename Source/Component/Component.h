#pragma once

#include "Core/Core.h"

class CGameObject;

class CComponent
{
public:
	CComponent(CGameObject* const owner);
	virtual ~CComponent() = default;
	virtual void Update(float deltaTime) { };

	TVec2 GetWorldPosition() const;
	const TVec2& GetRelativePosition() const;

	void SetRelativePosition(const TVec2& position);

protected:
	CGameObject* const mOwner;

private:
	TVec2 mRelativPosition = TVec2::Zero;

};
