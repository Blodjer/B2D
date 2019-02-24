#include "B2D_pch.h"
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
