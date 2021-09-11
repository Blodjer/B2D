#pragma once

#include "Core/Resource.h"

class GHITexture;
enum class EGHITextureFormat;

class CTexture : public IResource
{
protected:
    virtual bool Load(ResourcePath const& filePath) override;
    virtual void Free() override;

public:
    static constexpr auto GetFallbackResourcePath() { return "Content/Textures/MissingTexture.png"; }

    GHITexture const* GetGHITexture() const { return m_ghiTexture; }

private:
	int32 m_width;
	int32 m_height;
	int32 m_components;
    EGHITextureFormat m_format;

	GHITexture* m_ghiTexture;
};

using TextureRef = ResourcePtr<CTexture>;
