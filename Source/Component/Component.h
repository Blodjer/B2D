#pragma once

#include "Core/Vector2.h"

class CGameObject;

class CComponent
{
public:
	CComponent(CGameObject* const owner);
	virtual ~CComponent() = default;
	virtual void Update(float deltaTime) { };

	SVector2 GetWorldPosition() const;
	const SVector2& GetRelativePosition() const;

	void SetRelativePosition(const SVector2& position);

protected:
	CGameObject* const mOwner;

private:
	SVector2 mRelativPosition = SVector2::Zero;

};
