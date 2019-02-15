#include "SpriteRenderer.h"

#include "Debug/Debug.h"
#include "GameObject.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include <string>

CSpriteRenderer::CSpriteRenderer(CGameObject* const owner)
	: IComponentRenderer(owner)
{
	//SetSprite("Content/Sprites/MissingTexture.png");
	SetShader(CShader::Load("Content/Shader/SpriteVS.glsl", "Content/Shader/SpriteFS.glsl"));
}

CSpriteRenderer::~CSpriteRenderer()
{
	delete(mTexture);
	delete(mTexture2);
	delete(mShader);
}

void CSpriteRenderer::SetSprite(const std::string& filepath)
{
	if (mTexture != nullptr)
	{
		delete(mTexture);
	}
	
	//glActiveTexture(GL_TEXTURE0);
	mTexture = new CTexture(filepath);

	//glActiveTexture(GL_TEXTURE1);
	mTexture2 = new CTexture("Content/Sprites/diff.png");

	mShader->Use();
	mShader->SetInt("ourTexture", 0);
	mShader->SetInt("ourTextures", 1);
	
	mWidth = mTexture->mWidth;
	mHeight = mTexture->mHeight;
}

void CSpriteRenderer::SetShader(class CShader* shader)
{
	mShader = shader;
}

void CSpriteRenderer::Draw(CGraphics* graphics)
{
	glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	glBindTexture(GL_TEXTURE_2D, mTexture->mHandle);
	glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 0
	glBindTexture(GL_TEXTURE_2D, mTexture2->mHandle);
	//glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0

	mShader->Use();

	/*
	glBindTexture(GL_TEXTURE_2D, mTexture->Handle);

	glBegin(GL_QUADS);

	glTexCoord2d(0, 0); glVertex3f(0 - mWidth /100.f, 0 - mHeight /100.f, 0);
	glTexCoord2d(0, 1); glVertex3f(0 - mWidth /100.f, 0 + mHeight /100.f, 0);
	glTexCoord2d(1, 1); glVertex3f(0 + mWidth /100.f, 0 + mHeight /100.f, 0);
	glTexCoord2d(1, 0); glVertex3f(0 + mWidth /100.f, 0 - mHeight /100.f, 0);
	
	glEnd();
	*/
}
