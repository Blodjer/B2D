#pragma once

#include "Core/Resource.h"

class GHITexture;

class CTexture : public IResource
{
protected:
    virtual bool Load(ResourcePath const& filePath) override;
    virtual void Free() override;

public:
    static constexpr auto GetFallbackResourcePath() { return "Content/Sprites/MissingTexture.png"; }

    GHITexture const* GetGHITexture() const { return mGHITexture; }

private:
	int32 mWidth;
	int32 mHeight;
	int32 mComponents;

	GHITexture* mGHITexture;
};

using TextureRef = ResourcePtr<CTexture>;
