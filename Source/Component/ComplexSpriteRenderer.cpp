#include "B2D_pch.h"
#include "ComplexSpriteRenderer.h"

#include "GameObject.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

CComplexSpriteRenderer::CComplexSpriteRenderer(CGameObject* const owner)
	: IComponentRenderer(owner)
{

}

void CComplexSpriteRenderer::SetMaterial(CMaterial* material)
{
	if (material == nullptr)
	{
		return;
	}

	mMaterial = material;
}
