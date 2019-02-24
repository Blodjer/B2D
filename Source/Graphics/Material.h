#pragma once

#include "Core/Core.h"
#include "Core/Resource.h"

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

	void SetTexture(uint32 index, ResourcePtr<CTexture> texture);
	ResourcePtr<CTexture> GetTexture(uint32 index) const;

private:
	CShader* const mShader;
	std::vector<ResourcePtr<CTexture>> mTextures;
};
