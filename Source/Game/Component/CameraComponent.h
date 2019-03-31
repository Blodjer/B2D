#pragma once
#include "Game/Core/Component.h"
#include "Graphics/Viewport.h"

enum class EProjection : uint8
{
    Perspective,
    Orthographic
};

struct CameraComponent : public Component
{
    DECLARE_COMPONENT(CameraComponent, 3);

    EProjection projection = EProjection::Perspective;
    EProjection targetProjection = projection;
    float projectionLerp = 1.0f;
    bool useAspectRatio = false;
    float aspectRatio = 16.0f / 9.0f;
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 99999999.0f;
};

