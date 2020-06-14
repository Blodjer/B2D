#pragma once
#include "Graphics/GHI/GHIMaterial.h"
#include <GL/glew.h>

class OpenGLMaterial : public GHIMaterial
{
public:
    OpenGLMaterial(GLuint handle);

    GLuint GetHandle() const { return m_handle; }

private:
    GLuint m_handle;
};

