#pragma once

#include "IComponentRenderer.h"
#include "Core/Resource.h"

class CMaterial;
class CTexture;

class CSpriteRenderer : public IComponentRenderer
{
public:
	CSpriteRenderer(CGameObject* const owner);
	virtual ~CSpriteRenderer() = default;

	void SetSprite(const std::string& filepath, bool fitSize = true);

	virtual CMaterial* GetMaterial() const override { return mMaterial; }
	ResourcePtr<CTexture> GetTexture() const { return mTexture; }

private:
	TVec2 mSize;

	CMaterial* mMaterial;
	ResourcePtr<CTexture> mTexture;
};
