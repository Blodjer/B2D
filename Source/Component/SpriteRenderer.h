#pragma once

#include "IComponentRenderer.h"

#include <string>

class CMaterial;
class CTexture;

class CSpriteRenderer : public IComponentRenderer
{
public:
	CSpriteRenderer(CGameObject* const owner);
	virtual ~CSpriteRenderer() = default;

	void SetSprite(const std::string& filepath);

	virtual CMaterial* GetMaterial() const override { return mMaterial; }
	CTexture const* GetTexture() const { return mTexture; }

private:
	TVec2 mSize;
	
	CMaterial* mMaterial;
	CTexture const* mTexture;
};
