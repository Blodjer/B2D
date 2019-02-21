#include "ComplexSpriteRenderer.h"

#include "GameObject.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include <string>

CComplexSpriteRenderer::CComplexSpriteRenderer(CGameObject* const owner)
	: IComponentRenderer(owner)
{
// 	CShader* shader = CShader::Load("Content/Shader/SpriteVS.glsl", "Content/Shader/SpritePS.glsl");
// 
// 	CTexture* textureDiffuse = new CTexture("Content/Sprites/diff.png");
// 	CTexture* textureNomal = new CTexture("Content/Sprites/norm.png");
// 
// 	mMaterial = new CMaterial(shader);
// 	mMaterial->SetTexture(0, textureDiffuse);
// 	mMaterial->SetTexture(1, textureNomal);
// 
// 	mSize = TVec2(textureDiffuse->mWidth, textureDiffuse->mHeight);
}

void CComplexSpriteRenderer::SetMaterial(CMaterial* material)
{
	if (material == nullptr)
	{
		return;
	}

	mMaterial = material;
}
