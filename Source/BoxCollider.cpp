#include "BoxCollider.h"
#include "Graphics.h"

CBoxCollider::CBoxCollider(CGameObject* pOwner) : CComponentCollider(pOwner)
{

}

CBoxCollider::~CBoxCollider()
{

}

void CBoxCollider::DrawShape(CGraphics* pGraphics)
{
	
}

void CBoxCollider::SetSize(float fWidth, float fHeight)
{
	this->Size = SVector2(fWidth, fHeight);
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