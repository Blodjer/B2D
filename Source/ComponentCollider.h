#pragma once

#include "IComponent.h"

// sweep
// only rootcomponent sweeps
// setposition(Vector2 positon, bool sweep)

class ComponentCollider : public IComponent
{
public:
	ComponentCollider(GameObject* pOwner);
	virtual ~ComponentCollider();

	virtual void DrawShape(struct SDL_Renderer* pRenderer) = 0;

private:
	void OnContact() {}

};

