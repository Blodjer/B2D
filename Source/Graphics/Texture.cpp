#include "B2D_pch.h"
#include "Texture.h"

#include "GameEngine.h"
#include "Graphics/GHI/GraphicsHardwareInterface.h"
#include "Graphics/GHI/GHITexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

bool CTexture::Load(ResourcePath const& path)
{
	stbi_set_flip_vertically_on_load(1);

	if (!stbi_info(path.c_str(), &mWidth, &mHeight, &mComponents))
	{
		return false;
	}

	void* imgPtr = stbi_load(path.c_str(), &mWidth, &mHeight, &mComponents, STBI_default);
	if (imgPtr == nullptr)
	{
		return false;
	}

    mGHITexture = GameEngine::Instance()->GetGHI()->CreateTexture(imgPtr, mWidth, mHeight, mComponents);
    
    stbi_image_free(imgPtr);

    if (mGHITexture == nullptr)
    {
        return false;
    }

    return true;
}

void CTexture::Free()
{
    GameEngine::Instance()->GetGHI()->FreeTexture(mGHITexture);
}
