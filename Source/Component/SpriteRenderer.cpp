#include "SpriteRenderer.h"

#include "GameObject.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include <string>

CSpriteRenderer::CSpriteRenderer(CGameObject* const owner)
	: IComponentRenderer(owner)
{
	CShader* const shader = CShader::Load("Content/Shader/SimpleSpriteVS.glsl", "Content/Shader/SimpleSpritePS.glsl");
	mMaterial = new CMaterial(shader);

	SetSprite("Content/Sprites/MissingTexture.png");
}

void CSpriteRenderer::SetSprite(const std::string& filepath)
{
	CTexture* texture = new CTexture(filepath);
	mSize = TVec2(texture->mWidth, texture->mHeight);

	mMaterial->SetTexture(0, texture);
}
