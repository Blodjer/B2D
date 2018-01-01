#pragma once

#include "ComponentCollider.h"

class BoxCollider : public ComponentCollider
{
public:
	BoxCollider(GameObject* pOwner);
	virtual ~BoxCollider();

	void DrawShape(struct SDL_Renderer* pRenderer);

	void SetSize(float fWidth, float fHeight);

	struct SDL_Rect GetRect();

private:
	Vector2 Size;

};

