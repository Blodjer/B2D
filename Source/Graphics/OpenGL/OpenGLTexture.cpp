#include "B2D_pch.h"
#include "OpenGLTexture.h"

OpenGLTexture::OpenGLTexture(GLuint handle, uint32 width, uint32 height)
    : GHITexture(width, height)
    , m_handle(handle)
{

}
