#include "B2D_pch.h"
#include "Material.h"

#include "Shader.h"
#include "Texture.h"

CMaterial::CMaterial(CShader* const shader)
	: mShader(shader)
{

}

CMaterial::CMaterial()
    : mShader(CShader::Load("Content/Shader/DefaultVS.glsl", "Content/Shader/SimpleSpritePS.glsl"))
{

}

void CMaterial::SetTexture(uint32 index, ResourcePtr<CTexture> texture)
{
	if (index < mTextures.size())
	{
		mTextures[index] = texture;
	}
	else
	{
		mTextures.push_back(texture);
	}
}

ResourcePtr<CTexture> CMaterial::GetTexture(uint32 index) const
{
	if (index < mTextures.size())
	{
		return mTextures[index];
	}
	
	return ResourcePtr<CTexture>::InvalidPtr;
}
