#pragma once

#include <string>

class CTexture
{
public:
	CTexture(std::string filename);
	~CTexture() = default;

	int Width;
	int Height;

	int Handle;
};

