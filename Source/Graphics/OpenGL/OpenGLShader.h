#pragma once

#include "Graphics/GHI/GHIShader.h"
#include <GL/glew.h>

class OpenGLShader : public GHIShader
{
public:
    OpenGLShader() = default;
    ~OpenGLShader() = default;

    void Create(char* code, GLuint type);
    void Delete();

    GLuint GetHandle() const { return mHandle; }

private:
    static bool CompileShader(char* code, GLuint type, GLuint& outHandle);

private:
    GLuint mHandle = 0;
};
