#pragma once

#include "Core/Core.h"
#include "Core/Resource.h"

#include <string>

class CTexture : public IResource
{
public:
	~CTexture();

	virtual bool Load(ResourcePath const& filePath) override;
	static ResourcePath const GetFallbackResourcePath() { return "Content/Sprites/MissingTexture.png"; }

public:
	int32 mWidth;
	int32 mHeight;
	int32 mComponents;

	uint32 mHandle;
};