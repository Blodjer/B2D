#include "BoxCollider.h"
#include <SDL2\SDL.h>

BoxCollider::BoxCollider(GameObject* pOwner) : ComponentCollider(pOwner)
{

}

BoxCollider::~BoxCollider()
{

}

void BoxCollider::DrawShape(SDL_Renderer* pRenderer)
{
	SDL_Rect xRect = this->GetRect();
	SDL_RenderDrawRect(pRenderer, &xRect);
}

void BoxCollider::SetSize(float fWidth, float fHeight)
{
	this->Size = Vector2(fWidth, fHeight);
}

SDL_Rect BoxCollider::GetRect()
{
	auto vWorldPosition = this->GetWorldPosition();
	SDL_Rect xRect;
	xRect.x = vWorldPosition.X - this->Size.X * 0.5f;
	xRect.y = vWorldPosition.Y - this->Size.Y * 0.5f;
	xRect.w = this->Size.X;
	xRect.h = this->Size.Y;
	return xRect;
}
