#pragma once

#include "Graphics/GHI/GHITexture.h"
#include <GL/glew.h>

class OpenGLTexture : public GHITexture
{
public:
    OpenGLTexture(GLuint handle);

    FORCEINLINE GLuint GetHandle() const { return mHandle; }

private:
    GLuint mHandle = 0;
};

