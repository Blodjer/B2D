#include "BoxCollider.h"

CBoxCollider::CBoxCollider(CGameObject* owner) : CComponentCollider(owner)
{

}

CBoxCollider::~CBoxCollider()
{

}

void CBoxCollider::SetSize(float width, float height)
{
	mSize = TVec2(width, height);
}
/*
SDL_Rect CBoxCollider::GetRect()
{
	auto vWorldPosition = this->GetWorldPosition();
	SDL_Rect xRect;
	xRect.x = vWorldPosition.X - this->Size.X * 0.5f;
	xRect.y = vWorldPosition.Y - this->Size.Y * 0.5f;
	xRect.w = this->Size.X;
	xRect.h = this->Size.Y;
	return xRect;
}
*/