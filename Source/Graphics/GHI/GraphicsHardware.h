#pragma once

#include "Core/Core.h"
#include "GraphicsCommon.h"

class IGraphicsHardwareInterface;

class GraphicsHardware final
{
public:
    static IGraphicsHardwareInterface* Create(EGraphicsAPI const graphicsAPI);
};