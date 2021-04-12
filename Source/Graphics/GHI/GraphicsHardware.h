#pragma once

#include "Core/Core.h"

enum class EGraphicsAPI
{
    OpenGL,
    Vulkan
};

class IGraphicsHardwareInterface;

class GraphicsHardware final
{
public:
    static IGraphicsHardwareInterface* Create(EGraphicsAPI const graphicsAPI);
};