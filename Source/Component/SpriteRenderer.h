#pragma once

#include "IComponentRenderer.h"

class CTexture;
class CShader;

class CSpriteRenderer : public IComponentRenderer
{
public:
	CSpriteRenderer(CGameObject* const owner);
	virtual ~CSpriteRenderer();

	void SetSprite(const std::string& filepath);
	void SetShader(CShader* shader);

	virtual void Draw(CGraphics* graphics) override;

private:
	uint32 mWidth;
	uint32 mHeight;

	CTexture* mTexture;
	CTexture* mTexture2;
	CShader* mShader;
};
