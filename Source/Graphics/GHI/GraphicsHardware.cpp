#include "B2D_pch.h"
#include "GraphicsHardware.h"

#include "Graphics/OpenGL/OpenGLGHI.h"

IGraphicsHardwareInterface* GraphicsHardware::Create(EGraphicsAPI const graphicsAPI)
{
    switch (graphicsAPI)
    {
    case EGraphicsAPI::Vulkan:
        //B2D_LOG_INFO("Creating \"{}\" GHI", "Vulkan");
        B2D_LOG_ERROR("Vulkan is not supported yet! Using OpenGL as fallback.");
        return new OpenGLGHI();
    case EGraphicsAPI::OpenGL:
        B2D_LOG_INFO("Created OpenGL GHI!");
        return new OpenGLGHI();
    }

    B2D_TRAPf("Trying to create unknown GHI!");
    return nullptr;
}
