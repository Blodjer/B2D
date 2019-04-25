#pragma once
#include "Game/Core/Component.h"

#include "Core/Core.h"
#include "Graphics/Texture.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"

struct SpriteComponent : public Component
{
    DECLARE_COMPONENT(SpriteComponent, 1);

    SpriteComponent() {};
    SpriteComponent(Material* const m) : material(m) {};

    Material* material;
};

