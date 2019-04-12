#include "B2D_pch.h"
#include "OpenGLShader.h"

void OpenGLShader::Create(char* code, GLuint type)
{
    if (!CompileShader(code, type, mHandle))
    {
        mHandle = 0;
    }
}

void OpenGLShader::Delete()
{
    glDeleteShader(mHandle);
}

bool OpenGLShader::CompileShader(char* code, GLuint type, GLuint& outHandle)
{
    GLuint id = glCreateShader(type);

    glShaderSource(id, 1, &code, nullptr);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        int messageLength;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &messageLength);

        char* message = static_cast<char*>(alloca(messageLength * sizeof(char)));
        glGetShaderInfoLog(id, 512, nullptr, message);

        glDeleteShader(id);

        B2D_CORE_ERROR("Shader Compile Error: {0}", message);
        return false;
    }

    outHandle = id;
    return true;
}
