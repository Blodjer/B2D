#pragma once

#include "Core/Core.h"
#include "Core/Resource.h"

class CRenderer;
class CShader;
class CTexture;
class RenderSystem;

class CMaterial
{
    friend CRenderer;
    friend RenderSystem;

public:
    CMaterial();
    CMaterial(CShader* const shader);
	~CMaterial() = default;

	void SetTexture(uint32 index, ResourcePtr<CTexture> texture);
	ResourcePtr<CTexture> GetTexture(uint32 index) const;

private:
	CShader* const mShader;
	std::vector<ResourcePtr<CTexture>> mTextures;
};
