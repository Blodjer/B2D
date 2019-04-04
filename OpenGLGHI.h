#pragma once
#include "Graphics/GraphicsHardwareInterface.h"

class OpenGLGHI : public IGraphicsHardwareInterface
{
public:
    OpenGLGHI();
    ~OpenGLGHI();

    virtual void Clear() override;
    virtual void Swap() override;

};

