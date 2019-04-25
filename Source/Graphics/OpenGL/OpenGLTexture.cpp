#include "B2D_pch.h"
#include "OpenGLTexture.h"

#include <GL/glew.h>

void OpenGLTexture::Create(void* data, uint32 width, uint32 height, uint8 components)
{
    GLenum format;
    switch (components)
    {
        case 1:
            format = GL_RED;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            B2D_CORE_ERROR("Cannot create texture with {} components", components);
            return;
    }

    glGenTextures(1, &mHandle);
    glBindTexture(GL_TEXTURE_2D, mHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    B2D_ASSERT(mHandle != GL_INVALID_VALUE);
}

void OpenGLTexture::Free()
{
    glDeleteTextures(1, &mHandle);
}
