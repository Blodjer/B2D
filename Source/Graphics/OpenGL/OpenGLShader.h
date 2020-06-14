#pragma once

#include "Graphics/GHI/GHIShader.h"
#include <GL/glew.h>

class OpenGLShader : public GHIShader
{
public:
    OpenGLShader(GLuint handle);

    GLuint GetHandle() const { return m_handle; }

private:
    GLuint m_handle = 0;
};
