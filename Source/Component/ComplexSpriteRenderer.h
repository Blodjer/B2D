#pragma once

#include "IComponentRenderer.h"

class CMaterial;

class CComplexSpriteRenderer : public IComponentRenderer
{
public:
	CComplexSpriteRenderer(CGameObject* const owner);
	virtual ~CComplexSpriteRenderer() = default;

	void SetMaterial(CMaterial* material);
	virtual CMaterial* GetMaterial() const override { return mMaterial; }

private:
	TVec2 mSize;
	
	CMaterial* mMaterial;
};
