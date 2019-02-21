#pragma once

#include "ComponentCollider.h"

class CBoxCollider : public CComponentCollider
{
public:
	CBoxCollider(CGameObject* pOwner);
	virtual ~CBoxCollider();

	void SetSize(float width, float height);

	//struct SDL_Rect GetRect();

private:
	TVec2 mSize;

};
