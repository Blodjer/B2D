#pragma once
#include "Graphics/GHI/GHIRenderTarget.h"
#include <GL/glew.h>

class OpenGLRenderTarget : public GHIRenderTarget
{
public:
    OpenGLRenderTarget(GLuint handle, GHITexture* targetTexture);
    
    GLuint GetHandle() const { return mHandle; }

    virtual GHITexture* GetTexture() const override { return mTargetTexture; };

private:
    GLuint mHandle;
    GHITexture* mTargetTexture;

};

