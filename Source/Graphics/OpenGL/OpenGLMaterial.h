#pragma once
#include "Graphics/GHI/GHIMaterial.h"
#include "GL/glew.h"

class OpenGLShader;

class OpenGLMaterial : public GHIMaterial
{
public:
    OpenGLMaterial(OpenGLShader* vertextShader, OpenGLShader* pixelShader);
    ~OpenGLMaterial();

    GLuint GetHandle() const { return mHandle; }

private:
    GLuint mHandle;
};

