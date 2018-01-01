#pragma once

#include "Vector2.h"
#include "GameObject.h"

class IComponent
{
protected:
	class GameObject* const Owner;

public:
	IComponent(GameObject* pOwner) : Owner(pOwner) { };
	virtual ~IComponent() { };
	virtual void Update(float deltaTime) { };

	Vector2 GetWorldPosition() { return this->Owner->GetPosition() + this->RelativPosition; };
	Vector2 RelativPosition = Vector2::Zero;
};