#pragma once

#include "ComponentCollider.h"

class CBoxCollider : public CComponentCollider
{
public:
	CBoxCollider(CGameObject* pOwner);
	virtual ~CBoxCollider();

	void DrawShape(CGraphics* pGraphics);

	void SetSize(float fWidth, float fHeight);

	//struct SDL_Rect GetRect();

private:
	SVector2 Size;

};
