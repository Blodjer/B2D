#include "B2D_pch.h"
#include "Texture.h"

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

CTexture::~CTexture()
{
	glDeleteTextures(1, &mHandle);
}

bool CTexture::Load(ResourcePath const& path)
{
	stbi_set_flip_vertically_on_load(1);

	if (!stbi_info(path.c_str(), &mWidth, &mHeight, &mComponents))
	{
		return false;
	}

	unsigned char* imgPtr = stbi_load(path.c_str(), &mWidth, &mHeight, &mComponents, STBI_rgb_alpha);
	if (imgPtr == nullptr)
	{
		return false;
	}

	// Generate the OpenGL texture object
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgPtr);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// clean up
	stbi_image_free(imgPtr);

	mHandle = texture;
	return mHandle != GL_INVALID_VALUE;
}
