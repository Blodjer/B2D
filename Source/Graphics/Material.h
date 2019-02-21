#pragma once

#include "Core/Core.h"

#include <vector>
#include <array>

class CRenderer;
class CShader;
class CTexture;

class CMaterial
{
	friend CRenderer;

public:
	CMaterial(CShader* const shader);
	~CMaterial() = default;

	void SetTexture(uint32 index, CTexture const* texture);

private:
	CShader* const mShader;
	std::vector<CTexture const*> mTextures;
};
