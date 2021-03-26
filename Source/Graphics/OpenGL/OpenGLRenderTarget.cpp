#include "B2D_pch.h"
#include "OpenGLRenderTarget.h"

OpenGLRenderTarget::OpenGLRenderTarget(GLuint handle, GHITexture* targetTexture, GHITexture* depthTexture)
    : m_handle(handle)
    , m_targetTexture(targetTexture)
    , m_depthTexture(depthTexture)
{

}
