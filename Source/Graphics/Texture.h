#pragma once

#include "Core/Core.h"

#include <GL/glew.h>
#include <string>

class CTexture
{
public:
	CTexture(std::string const& filename);
	~CTexture() = default;

private:
	GLuint LoadTexture(const char* file_name, uint32& width, uint32& height);

public:
	uint32 mWidth;
	uint32 mHeight;

	GLuint mHandle;
};

