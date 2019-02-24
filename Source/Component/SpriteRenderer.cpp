#include "SpriteRenderer.h"

#include "GameObject.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include <string>

CSpriteRenderer::CSpriteRenderer(CGameObject* const owner)
	: IComponentRenderer(owner)
	, mSize(100, 100)
{
	CShader* const shader = CShader::Load("Content/Shader/DefaultVS.glsl", "Content/Shader/SimpleSpritePS.glsl");
	mMaterial = new CMaterial(shader);
}

void CSpriteRenderer::SetSprite(const std::string& filepath, bool fitSize)
{
	ResourcePtr<CTexture> texture = IResourceManager::Get<CTexture>(filepath);

	mMaterial->SetTexture(0, texture);

	if (fitSize && texture != nullptr)
	{
		mSize = TVec2(texture->mWidth, texture->mHeight);
	}
}
