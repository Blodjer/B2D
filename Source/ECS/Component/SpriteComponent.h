#pragma once
#include "Component.h"

#include "Core/Core.h"
#include "Graphics/Texture.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"

struct SpriteComponent : public Component
{
    SpriteComponent() {};
    SpriteComponent(CShader* const shader) : material(shader) {};

    CMaterial material;

    static constexpr uint16 MASK = 4;
};

