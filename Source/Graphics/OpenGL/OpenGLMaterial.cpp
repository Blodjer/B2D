#include "B2D_pch.h"
#include "OpenGLMaterial.h"
#include "OpenGLShader.h"

OpenGLMaterial::OpenGLMaterial(OpenGLShader* vertextShader, OpenGLShader* pixelShader)
{
    GLuint handle = glCreateProgram();

    glAttachShader(handle, vertextShader->GetHandle());
    glAttachShader(handle, pixelShader->GetHandle());
    glLinkProgram(handle);
    glValidateProgram(handle);

    glDetachShader(handle, vertextShader->GetHandle());
    glDetachShader(handle, pixelShader->GetHandle());

    mHandle = handle;
}

OpenGLMaterial::~OpenGLMaterial()
{
    glDeleteProgram(mHandle);
}
