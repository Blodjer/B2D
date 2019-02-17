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
	GLuint LoadTexture(std::string const&, int32& width, int32& height);

public:
	int32 mWidth;
	int32 mHeight;

	GLuint mHandle;
};

