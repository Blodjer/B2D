#include "Material.h"

#include "Shader.h"
#include "Texture.h"

CMaterial::CMaterial(CShader* const shader)
	: mShader(shader)
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
