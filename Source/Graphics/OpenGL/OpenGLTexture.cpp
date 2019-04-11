#include "B2D_pch.h"
#include "OpenGLTexture.h"

#include <GL/glew.h>

void OpenGLTexture::Create(void* data, uint32 width, uint32 height)
{
    glGenTextures(1, &mHandle);
    glBindTexture(GL_TEXTURE_2D, mHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    B2D_ASSERT(mHandle != GL_INVALID_VALUE);
}

void OpenGLTexture::Free()
{
    glDeleteTextures(1, &mHandle);
}
