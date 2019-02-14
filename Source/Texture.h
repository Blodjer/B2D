#pragma once

#include <string>

class CTexture
{
public:
	CTexture(std::string const& filename);
	~CTexture() = default;

	int mWidth;
	int mHeight;

	int mHandle;
};

