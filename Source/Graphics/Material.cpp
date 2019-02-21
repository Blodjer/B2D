#include "Material.h"

#include "Shader.h"
#include "Texture.h"

CMaterial::CMaterial(CShader* const shader)
	: mShader(shader)
{

}

void CMaterial::SetTexture(uint32 index, CTexture const* texture)
{
	mShader->Use();
	
	std::string s = "texture";
	s.append(std::to_string(index));

	if (texture == nullptr)
	{
		mShader->SetInt(s, -1);
		return;
	}

	mShader->SetInt(s, texture->GetHandle());

	if (index < mTextures.size())
	{
		mTextures[index] = texture;
	}
	else
	{
		mTextures.push_back(texture);
	}
}
