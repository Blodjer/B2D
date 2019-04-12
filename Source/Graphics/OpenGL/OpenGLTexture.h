#pragma once

#include "Graphics/GHI/GHITexture.h"
#include <GL/glew.h>

class OpenGLTexture : public GHITexture
{
public:
    OpenGLTexture() = default;

    virtual void Create(void* data, uint32 width, uint32 height) override;
    virtual void Free() override;

    GLuint GetHandle() const { return mHandle; }

private:
    GLuint mHandle = 0;
};

