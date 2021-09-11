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

    int actualComponents;
    if (!stbi_info(path.c_str(), &m_width, &m_height, &actualComponents))
	{
		return false;
	}

    // TODO: Handle textures with less components
    m_components = STBI_rgb_alpha;

	void* imgPtr = stbi_load(path.c_str(), &m_width, &m_height, &actualComponents, m_components);
	if (imgPtr == nullptr)
	{
		return false;
	}

    switch (m_components)
    {
    case 4:
        m_format = EGHITextureFormat::RGBA8;
        break;
    default:
        B2D_BREAK_f("Format not supported!");
        return false;
    }

    m_ghiTexture = GameEngine::Instance()->GetGHI()->CreateTexture(imgPtr, m_width, m_height, m_format);
    
    stbi_image_free(imgPtr);

    if (m_ghiTexture == nullptr)
    {
        return false;
    }

    return true;
}

void CTexture::Free()
{
    GameEngine::Instance()->GetGHI()->DestroyTexture(m_ghiTexture);
}
