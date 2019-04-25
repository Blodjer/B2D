#include "B2D_pch.h"
#include "OpenGLRenderTarget.h"

OpenGLRenderTarget::OpenGLRenderTarget(GLuint handle, GHITexture* targetTexture)
    : mHandle(handle)
    , mTargetTexture(targetTexture)
{

}
