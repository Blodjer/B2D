#include "Texture.h"

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

GLuint CTexture::LoadTexture(std::string const& filename, int32& width, int32& height)
{
	stbi_set_flip_vertically_on_load(1);

	int tmpWidth;
	int tmpHeight;
	int comp;

	unsigned char* imgPtr = stbi_load(filename.c_str(), &tmpWidth, &tmpHeight, &comp, STBI_rgb_alpha);
	if (imgPtr == nullptr)
	{
		return GL_INVALID_VALUE;
	}

	// Generate the OpenGL texture object
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tmpWidth, tmpHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgPtr);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// clean up
	stbi_image_free(imgPtr);

	width = tmpWidth;
	height = tmpHeight;
	return texture;
}

CTexture::CTexture(std::string const& filename)
{
	mHandle = LoadTexture(filename.c_str(), mWidth, mHeight);
}
