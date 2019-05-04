#pragma once

#include "Graphics/GHI/GHIShader.h"
#include <GL/glew.h>

class OpenGLShader : public GHIShader
{
public:
    OpenGLShader(GLuint handle);

    GLuint GetHandle() const { return mHandle; }

private:
    GLuint mHandle = 0;
};
