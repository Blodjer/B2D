#pragma once

#include "Core/Core.h"

#include <string>

typedef unsigned int GLuint;

class CTexture
{
public:
	CTexture(std::string const& filename);
	~CTexture() = default;

	GLuint GetHandle() const { return mHandle; }

private:
	GLuint LoadTexture(std::string const&, int32& width, int32& height);

public:
	int32 mWidth;
	int32 mHeight;

	GLuint mHandle;
};

